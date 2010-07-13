/***************************************************************************
 *   Copyright 2008-2010 Stefan Majewsky <majewsky.stefan@ages-skripte.org>
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

QString colorKey(KDiamond::Color color)
{
	QString colors[] = {
		QString::fromLatin1("kdiamond-selection"),
		QString::fromLatin1("kdiamond-red"),
		QString::fromLatin1("kdiamond-green"),
		QString::fromLatin1("kdiamond-blue"),
		QString::fromLatin1("kdiamond-yellow"),
		QString::fromLatin1("kdiamond-white"),
		QString::fromLatin1("kdiamond-black"),
		QString::fromLatin1("kdiamond-orange")
	};
	return colors[(color < 0 || color > KDiamond::ColorsCount) ? 0 : color];
}


Diamond::Diamond(int xPos, int yPos, KDiamond::Color color, Board *board)
	: KGameRenderedItem(Renderer::self()->renderer(), colorKey(color))
	, m_board(board)
	, m_color(color)
	, m_pos(xPos, yPos)
{
	board->addItem(this);
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

void Diamond::updateGeometry()
{
	//resize diamond (ensure at least 1 pixel edge length to avoid problems with zero size)
	const qreal diamondEdgeLength = qMax(1, m_board->diamondEdgeLength());
	const QRectF itemBounds(QPointF(), renderSize());
	const QRectF sceneBounds = mapToScene(itemBounds).boundingRect();
	scale(diamondEdgeLength / sceneBounds.width(), diamondEdgeLength / sceneBounds.height());
	//change position
	setPosInBoardCoords(m_pos);
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
		if (qAbs(dx) > qAbs(dy))
		{
			if (qAbs(dx) >= diamondSize.width() * draggingFuzziness)
			{
				m_board->dragDiamond(this, dx < 0 ? -1 : 1, 0);
				m_mouseDown = false; //mouse action has been handled
			}
		}
		else
		{
			if (qAbs(dy) >= diamondSize.height() * draggingFuzziness)
			{
				m_board->dragDiamond(this, 0, dy < 0 ? -1 : 1);
				m_mouseDown = false;
			}
		}
	}
}

void Diamond::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	if (m_mouseDown && boundingRect().contains(event->pos()))
	{
		m_board->clickDiamond(this);
		m_mouseDown = false;
	}
}

#include "diamond.moc"
