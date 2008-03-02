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

void Diamond::updateGeometry()
{
    prepareGeometryChange();
    //update pixmap
    setPixmap(Renderer::diamond(m_color));
    //resize
    QRectF bounds = sceneBoundingRect();
    qreal diamondEdgeLength = m_board->diamondEdgeLength();
    scale(diamondEdgeLength / bounds.width(), diamondEdgeLength / bounds.height());
    //change position
    setPos(m_board->boardToScene(m_pos));
}

void Diamond::setPosInBoardCoords(const QPointF &pos)
{
    m_pos = pos;
    setPos(m_board->boardToScene(m_pos).toPoint());
}

void Diamond::mousePressEvent(QGraphicsSceneMouseEvent *)
{
    if (!m_board->isTimeUp())
        m_board->mouseOnDiamond(m_xIndex, m_yIndex);
}

#include "diamond.moc"
