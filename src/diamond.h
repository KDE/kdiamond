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

#ifndef KDIAMOND_DIAMOND_H
#define KDIAMOND_DIAMOND_H

#ifndef KDIAMOND_BOARD_H
	class Board;
#endif

#include <QGraphicsPixmapItem>

namespace KDiamond
{
	//registered colors of diamonds
	enum Color
	{
		NoColor = -1,  //use this if no actual color can be named (e.g. for a null Diamond pointer)
		Selection = 0, //actually no diamond type, but this allows to reuse the Diamond class' code for the selection marker
		RedDiamond = 1,
		GreenDiamond,
		BlueDiamond,
		YellowDiamond,
		WhiteDiamond,
		BlackDiamond,
		OrangeDiamond
	};
}

class Diamond : public QObject, public QGraphicsPixmapItem
{
	Q_OBJECT
	public:
		Diamond(int xIndex, int yIndex, int xPos, int yPos, KDiamond::Color color, Board *board);

		KDiamond::Color color() const;
		int xIndex() const;
		int yIndex() const;

		void setXIndex(int xIndex);
		void setYIndex(int yIndex);
		void setPosInBoardCoords(const QPointF &pos);
	public slots:
		void updateGeometry();
	protected:
		virtual void mousePressEvent(QGraphicsSceneMouseEvent *);
		virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *);
		virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *);
	private:
		Board *m_board;

		KDiamond::Color m_color;
		int m_xIndex, m_yIndex; //the index of the diamond in the Board's internal array (used for communication with Board)
		QPointF m_pos, m_target; //current position of diamond in board coordinates (see Board::boardToScene for details)
		bool m_mouseDown;
		QPointF m_mouseDownPos; //position of last mouse-down event in diamond coordinates
};

#endif //KDIAMOND_DIAMOND_H
