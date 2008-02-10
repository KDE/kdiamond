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
    : QGraphicsSvgItem()
{
    //init internal values
    m_xIndex = xIndex;
    m_yIndex = yIndex;
    m_color = color;
    m_board = board;
    //init QGraphicsSvgItem
    setSharedRenderer(Renderer::diamond(color));
    setVisible(true);
    //scale to width = height = 1, set position of center
    QRectF bounds = sceneBoundingRect();
    scale(1.0 / bounds.width(), 1.0 / bounds.height());
    setPos(QPointF(xPos, yPos));
    //selection markers do not react to mouse events; they should also appear behind diamonds
    if (color == KDiamond::Selection)
    {
        setAcceptedMouseButtons(0);
        setZValue(1);
    }
    else
        setZValue(2);
    //add to board
    board->addItem(this);
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
    //position differences
    QPointF current = pos();
    qreal dx = qAbs(current.x() - target.x());
    qreal dy = qAbs(current.y() - target.y());
    //timeline
    int duration = KDiamond::MoveDuration * qMax(dx, dy); //MoveDuration is the duration per scene unit
    QTimeLine *timer = new QTimeLine(duration);
    timer->setFrameRange(0, duration / KDiamond::MoveInterval);
    timer->setCurveShape(QTimeLine::LinearCurve);
    connect(timer, SIGNAL(finished()), this, SLOT(moveComplete()), Qt::DirectConnection);
    //animation
    QGraphicsItemAnimation *animation = new QGraphicsItemAnimation;
    animation->setItem(this);
    animation->setTimeLine(timer);
    animation->setPosAt(0.0, current);
    animation->setPosAt(1.0, target);
    timer->start();
    //This connection can be used by the board to determine whether there are animations in progress. The board's job queue is not processed during animations to make the movements on the screen clearer for the user.
    connect(m_board, SIGNAL(animationInProgress()), this, SLOT(animationInProgress()));
}

void Diamond::moveComplete()
{
    disconnect(m_board, SIGNAL(animationInProgress()), this, SLOT(animationInProgress()));
}

void Diamond::mouseReleaseEvent(QGraphicsSceneMouseEvent *)
{
    m_board->mouseOnDiamond(m_xIndex, m_yIndex);
}

#include "diamond.moc"
