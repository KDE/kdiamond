/***************************************************************************
 *   Copyright 2008-2010 Stefan Majewsky <majewsky@gmx.net>
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
	return colors[(color < 0 || color >= KDiamond::ColorsCount) ? 0 : color];
}

Diamond::Diamond(KDiamond::Color color, KGameRenderer* renderer, QGraphicsItem* parent)
	: KGameRenderedItem(renderer, colorKey(color), parent)
	, m_color(color)
	, m_renderSize(0)
{
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

QPointF Diamond::pos() const
{
	return m_pos;
}

void Diamond::setPos(const QPointF& pos)
{
	m_pos = pos;
	QGraphicsItem::setPos(m_pos * m_renderSize);
}

void Diamond::setRenderSize(int renderSize)
{
	if (m_renderSize == renderSize)
		return;
	m_renderSize = renderSize;
	KGameRenderedItem::setRenderSize(QSize(renderSize, renderSize));
	//adjust position to changed grid coordinates
	QGraphicsItem::setPos(m_pos * m_renderSize);
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
				emit dragged(QPoint(dx < 0 ? -1 : 1, 0));
				m_mouseDown = false; //mouse action has been handled
			}
		}
		else
		{
			if (qAbs(dy) >= diamondSize.height() * draggingFuzziness)
			{
				emit dragged(QPoint(0, dy < 0 ? -1 : 1));
				m_mouseDown = false;
			}
		}
	}
}

void Diamond::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	if (m_mouseDown && boundingRect().contains(event->pos()))
	{
		emit clicked();
		m_mouseDown = false;
	}
}

#include "diamond.moc"
