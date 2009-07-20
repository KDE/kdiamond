/***************************************************************************
 *   Copyright 2008-2009 Stefan Majewsky <majewsky.stefan@ages-skripte.org>
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

#include <QGraphicsSceneMouseEvent>

Diamond::Diamond(int xIndex, int yIndex, int xPos, int yPos, KDiamond::Color color, Board *board)
	: QGraphicsPixmapItem(0, board)
	, m_board(board)
	, m_color(color)
	, m_xIndex(xIndex)
	, m_yIndex(yIndex)
	, m_pos(xPos, yPos)
{
	//connect to board
	connect(board, SIGNAL(boardResized()), this, SLOT(updateGeometry()));
	//selection markers do not react to mouse events; they should also appear behind diamonds
	if (color == KDiamond::Selection)
	{
		setAcceptedMouseButtons(0);
		setZValue(-1);
	}
	else
		setAcceptedMouseButtons(Qt::LeftButton);
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
	setPixmap(Renderer::self()->diamond(m_color));
	//resize diamond (ensure at least 1 pixel edge length to avoid problems with zero size)
	const qreal diamondEdgeLength = qMax(1, m_board->diamondEdgeLength());
	const QRectF itemBounds(offset(), pixmap().size());
	const QRectF sceneBounds = mapToScene(itemBounds).boundingRect();
	scale(diamondEdgeLength / sceneBounds.width(), diamondEdgeLength / sceneBounds.height());
	//change position
	setPos(m_board->boardToScene(m_pos));
}

void Diamond::setPosInBoardCoords(const QPointF &pos)
{
	m_pos = pos;
	setPos(m_board->boardToScene(m_pos));
}

void Diamond::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	m_mouseDown = true;
	m_mouseDownPos = event->pos();
}

void Diamond::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	if (m_mouseDown)
	{
		//check if diamond was dragged onto another one
		const QPointF pos = event->pos();
		const qreal dx = pos.x() - m_mouseDownPos.x(), dy = pos.y() - m_mouseDownPos.y();
		const QSizeF diamondSize = boundingRect().size();
		static const qreal draggingFuzziness = 2.0 / 3.0;
		if (qAbs(dx) >= diamondSize.width() * draggingFuzziness)
		{
			//do not proceed if the diamond where this one was dragged on does not exist (i.e. this diamond is on one of the board edges)
			if (dx < 0 && m_xIndex == 0)
				return;
			if (dx > 0 && m_xIndex == m_board->diamondCountOnEdge() - 1)
				return;
			//dragged in X direction - simulate two clicks (on this one, then on the other one -> that is logically the same operation)
			m_board->clearSelection();
			m_board->mouseOnDiamond(m_xIndex, m_yIndex);
			m_board->mouseOnDiamond(m_xIndex + (dx < 0 ? -1 : 1), m_yIndex);
			m_mouseDown = false; //mouse action has been handled
		}
		if (qAbs(dy) >= diamondSize.height() * draggingFuzziness)
		{
			if (dy < 0 && m_yIndex == 0)
				return;
			if (dy > 0 && m_yIndex == m_board->diamondCountOnEdge() - 1)
				return;
			m_board->clearSelection();
			m_board->mouseOnDiamond(m_xIndex, m_yIndex);
			m_board->mouseOnDiamond(m_xIndex, m_yIndex + (dy < 0 ? -1 : 1));
			m_mouseDown = false;
		}
	}
}

void Diamond::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	if (m_mouseDown && boundingRect().contains(event->pos()))
	{
		m_board->mouseOnDiamond(m_xIndex, m_yIndex);
		m_mouseDown = false;
	}
}

#include "diamond.moc"
