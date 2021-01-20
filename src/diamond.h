/*
    SPDX-FileCopyrightText: 2008-2010 Stefan Majewsky <majewsky@gmx.net>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

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
    explicit Diamond(KDiamond::Color color, KGameRenderer *renderer, QGraphicsItem *parent = nullptr);

    KDiamond::Color color() const;
Q_SIGNALS:
    void clicked();
    void dragged(const QPoint &direction);
protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *) override;
private:
    KDiamond::Color m_color;
    bool m_mouseDown;
    QPointF m_mouseDownPos; //position of last mouse-down event in local coordinates
};

#endif //KDIAMOND_DIAMOND_H
