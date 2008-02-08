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

#ifndef KDIAMOND_DIAMOND_H
#define KDIAMOND_DIAMOND_H

#ifndef KDIAMOND_BOARD_H
    class Board;
#endif

#include <QWidget>

namespace KDiamond //auf die entsprechenden Header-Dateien verteilen
{
    //duration of a move animation in milliseconds
    const qreal MoveDuration = 200.0;
    //registered colors of diamonds
    enum Color
    {
        RedDiamond = 1,
        GreenDiamond,
        BlueDiamond,
        YellowDiamond,
        WhiteDiamond,
        BlackDiamond,
        OrangeDiamond
    };
    //convert int into the KDiamond::Color that is represented by this int
    KDiamond::Color colorFromNumber(int number);
};

class Diamond : public QWidget
{
    Q_OBJECT
    public:
        Diamond(int xIndex, int yIndex, qreal xPos, qreal yPos, KDiamond::Color color, Board *board);

        KDiamond::Color color() const;
        int xIndex() const;
        int yIndex() const;
        qreal xPos() const;
        qreal yPos() const;

        void setXIndex(int xIndex);
        void setYIndex(int yIndex);
    public slots:
        void move(qreal xPos, qreal yPos);
        void select(bool selected);
    protected:
        virtual void paintEvent(QPaintEvent *);
        virtual void mouseReleaseEvent(QMouseEvent *);
    private slots:
        void moveAnimation(int milliseconds);
    private:
        Board *m_board;

        KDiamond::Color m_color;
        int m_xIndex, m_yIndex;
        qreal m_xPos, m_yPos, m_xTarget, m_yTarget;
        bool m_moving, m_selected;
};

#endif //KDIAMOND_DIAMOND_H
