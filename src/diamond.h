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

#ifndef KDIAMOND_DIAMOND_H
#define KDIAMOND_DIAMOND_H

#include <KGameRenderedObjectItem>

namespace KDiamond
{
//registered colors of diamonds
enum Color {
    NoColor = -1,  //use this if no actual color can be named (e.g. for a null Diamond pointer)
    Selection = 0, //actually no diamond type, but this allows to reuse the Diamond class' code for the selection marker
    RedDiamond = 1,
    GreenDiamond,
    BlueDiamond,
    YellowDiamond,
    WhiteDiamond,
    BlackDiamond,
    OrangeDiamond,
    ColorsCount
};
}

class Diamond : public KGameRenderedObjectItem
{
    Q_OBJECT
public:
    Diamond(KDiamond::Color color, KGameRenderer *renderer, QGraphicsItem *parent = 0);

    KDiamond::Color color() const;
Q_SIGNALS:
    void clicked();
    void dragged(const QPoint &direction);
protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *);
private:
    KDiamond::Color m_color;
    bool m_mouseDown;
    QPointF m_mouseDownPos; //position of last mouse-down event in local coordinates
};

#endif //KDIAMOND_DIAMOND_H
