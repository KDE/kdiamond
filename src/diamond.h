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

class QGraphicsItemAnimation;
#include <QGraphicsPixmapItem>
class QTimeLine;

namespace KDiamond
{
    //duration of a move animation (per coordinate unit) in milliseconds
    const int MoveDuration = 250;
    //update interval during a move animation (KDiamond::MoveDuration should be divideable by KDiamond::MoveInterval)
    const int MoveInterval = 25;
    //duration of a move animation (frame count is determined by the theme)
    const int RemoveDuration = 200;
    //registered colors of diamonds
    enum Color
    {
        Selection = 0, //that is actually no diamond type, but gives the chance to reuse the Diamond class' code for the selection marker
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
}

class Diamond : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
    public:
        Diamond(int xIndex, int yIndex, qreal xPos, qreal yPos, KDiamond::Color color, Board *board);
        ~Diamond();

        KDiamond::Color color() const;
        int xIndex() const;
        int yIndex() const;
        static int animationsInProgress();

        void setXIndex(int xIndex);
        void setYIndex(int yIndex);
        void setPosInBoardCoords(const QPointF &pos);
    public slots:
        void updateGeometry();
        void move(const QPointF &target);
        void remove();
    protected:
        virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *);
    protected slots:
        void moveComplete();
        void setRemoveAnimFrame(int frame);
        void removeComplete();
    private:
        Board *m_board;
        QGraphicsItemAnimation *m_animation; //pointer to the animation currently in progress
        QTimeLine *m_timer;

        KDiamond::Color m_color;
        int m_xIndex, m_yIndex; //the index of the diamond in the Board's internal array (used for communication with Board)
        QPointF m_pos, m_target; //current position of diamond in board coordinates (see Board::boardToScene for details)

        int m_currentRemoveFrame; //current frame in the remove animation (needed when updating the pixmap because of resize events); -1 means that there is no remove animation at the moment
};

#endif //KDIAMOND_DIAMOND_H
