/***************************************************************************
 *   Copyright (C) 2008 Stefan Majewsky <majewsky.stefan@ages-skripte.org>
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public
 *   License as published by the Free Software Foundation; either
 *   version 2 of the License, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 ***************************************************************************/

#include "diamond.h"
#include "board.h"
#include "renderer.h"

#include <QGraphicsItemAnimation>
#include <QTimeLine>

int g_animationsInProgress = 0; //This counter can be used by the board to determine whether there are animations in progress. The board's job queue is not processed during animations to make the movements on the screen clearer for the user.

KDiamond::Color KDiamond::colorFromNumber(int number)
{
    switch (number)
    {
        case 1: return KDiamond::RedDiamond;
        case 2: return KDiamond::GreenDiamond;
        case 3: return KDiamond::BlueDiamond;
        case 4: return KDiamond::YellowDiamond;
        case 5: return KDiamond::WhiteDiamond;
        case 6: return KDiamond::BlackDiamond;
        case 7: return KDiamond::OrangeDiamond;
        default: return KDiamond::Selection;
    }
}

Diamond::Diamond(int xIndex, int yIndex, qreal xPos, qreal yPos, KDiamond::Color color, Board *board)
    : QGraphicsPixmapItem(0, board)
{
    //init internal values
    m_xIndex = xIndex;
    m_yIndex = yIndex;
    m_color = color;
    m_board = board;
    m_pos = QPointF(xPos, yPos);
    m_animation = 0;
    m_timer = new QTimeLine;
    m_timer->setCurveShape(QTimeLine::EaseInCurve);
    m_currentRemoveFrame = -1;
    //connect to board
    connect(board, SIGNAL(boardResized()), this, SLOT(updateGeometry()));
    //init QGraphicsPixmapItem (pixmap and geometry is initialized after the first resize event has occured)
    setVisible(true);
    //selection markers do not react to mouse events; they should also appear behind diamonds
    if (color == KDiamond::Selection)
    {
        setAcceptedMouseButtons(0);
        setZValue(3);
    }
    else
    {
        setAcceptedMouseButtons(Qt::LeftButton);
        setZValue(4);
    }
}

Diamond::~Diamond()
{
    if (m_animation != 0)
        moveComplete(); //cleanup the current move animation
    delete m_timer;
}

KDiamond::Color Diamond::color() const
{
    return m_color;
}

int Diamond::xIndex() const
{
    return m_xIndex;
}

int Diamond::yIndex() const
{
    return m_yIndex;
}

int Diamond::animationsInProgress()
{
    return g_animationsInProgress;
}

void Diamond::setXIndex(int xIndex)
{
    m_xIndex = xIndex;
}

void Diamond::setYIndex(int yIndex)
{
    m_yIndex = yIndex;
}

void Diamond::move(const QPointF &target)
{
    //store target for resize events
    m_target = target;
    //position differences
    qreal dx = qAbs(m_pos.x() - target.x());
    qreal dy = qAbs(m_pos.y() - target.y());
    //timeline
    int duration = KDiamond::MoveDuration * qMax(dx, dy); //MoveDuration is the duration per board unit
    m_timer->setDuration(duration);
    m_timer->setUpdateInterval(KDiamond::MoveInterval);
    connect(m_timer, SIGNAL(finished()), this, SLOT(moveComplete()), Qt::DirectConnection);
    //animation
    m_animation = new QGraphicsItemAnimation;
    m_animation->setItem(this);
    m_animation->setTimeLine(m_timer);
    m_animation->setPosAt(0.0, m_board->boardToScene(m_pos).toPoint());
    m_animation->setPosAt(1.0, m_board->boardToScene(m_target).toPoint());
    m_timer->start();
    ++g_animationsInProgress;
}

void Diamond::moveComplete()
{
    --g_animationsInProgress;
    delete m_animation;
    m_animation = 0;
    delete m_timer;
    m_timer = new QTimeLine;
    m_pos = m_target; //the target has now been reached - use this as position for further resize events
}

void Diamond::remove()
{
    //timeline
    m_timer->setDuration(KDiamond::RemoveDuration);
    m_timer->setFrameRange(0, Renderer::removeAnimFrameCount() - 1);
    connect(m_timer, SIGNAL(frameChanged(int)), this, SLOT(setRemoveAnimFrame(int)));
    connect(m_timer, SIGNAL(finished()), this, SLOT(removeComplete()));
    m_timer->start();
    //see Diamond::move for details on the following connection
    ++g_animationsInProgress;
}

void Diamond::setRemoveAnimFrame(int frame)
{
    m_currentRemoveFrame = frame;
    setPixmap(Renderer::removeFrame(m_color, frame));
}

void Diamond::removeComplete()
{
    --g_animationsInProgress;
}

void Diamond::updateGeometry()
{
    prepareGeometryChange();
    //update pixmap
    if (m_currentRemoveFrame == -1)
        setPixmap(Renderer::diamond(m_color));
    else
        setPixmap(Renderer::removeFrame(m_color, m_currentRemoveFrame));
    //resize
    QRectF bounds = sceneBoundingRect();
    qreal diamondEdgeLength = m_board->diamondEdgeLength();
    scale(diamondEdgeLength / bounds.width(), diamondEdgeLength / bounds.height());
    //change position
    if (m_animation == 0)
    {
        //no animation in progress - just change current position
        setPos(m_board->boardToScene(m_pos));
    }
    else
    {
        //animation in progress - change fix points of animation
        m_animation->setPosAt(0.0, m_board->boardToScene(m_pos).toPoint());
        m_animation->setPosAt(1.0, m_board->boardToScene(m_target).toPoint());
    }
}

void Diamond::setPosInBoardCoords(const QPointF &pos)
{
    if (m_animation == 0)
    {
        m_pos = pos;
        setPos(m_board->boardToScene(m_pos).toPoint());
    }
    else
    {
        //make the animation head towards this new position
        m_target = pos;
        m_animation->setPosAt(1.0, m_board->boardToScene(m_target).toPoint());
    }
}

void Diamond::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
{
    //propagate the mouse click to the board (to change the selection); clicking is blocked during movement
    if (m_animation == 0)
        m_board->mouseOnDiamond(m_xIndex, m_yIndex);
}

#include "diamond.moc"
