/*
    SPDX-FileCopyrightText: 2008-2010 Stefan Majewsky <majewsky@gmx.net>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "diamond.h"

#include <QGraphicsSceneMouseEvent>

QString colorKey(KDiamond::Color color)
{
    QString colors[] = {
        QLatin1String("kdiamond-selection"),
        QLatin1String("kdiamond-red"),
        QLatin1String("kdiamond-green"),
        QLatin1String("kdiamond-blue"),
        QLatin1String("kdiamond-yellow"),
        QLatin1String("kdiamond-white"),
        QLatin1String("kdiamond-black"),
        QLatin1String("kdiamond-orange")
    };
    return colors[(color < 0 || color >= KDiamond::ColorsCount) ? 0 : color];
}

Diamond::Diamond(KDiamond::Color color, KGameGraphicsViewRenderer *renderer, QGraphicsItem *parent)
    : KGameRenderedGraphicsObject(renderer, colorKey(color), parent)
    , m_color(color)
{
    //selection markers do not react to mouse events; they should also appear behind diamonds
    if (color == KDiamond::Selection) {
        setAcceptedMouseButtons({});
        setZValue(-1);
    } else {
        setAcceptedMouseButtons(Qt::LeftButton);
    }
}

KDiamond::Color Diamond::color() const
{
    return m_color;
}

void Diamond::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    m_mouseDown = true;
    m_mouseDownPos = event->pos();
}

void Diamond::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_mouseDown) {
        //check if diamond was dragged onto another one
        const QPointF pos = event->pos();
        const qreal dx = pos.x() - m_mouseDownPos.x(), dy = pos.y() - m_mouseDownPos.y();
        const QSizeF diamondSize = boundingRect().size();
        static const qreal draggingFuzziness = 2.0 / 3.0;
        if (qAbs(dx) > qAbs(dy)) {
            if (qAbs(dx) >= diamondSize.width() * draggingFuzziness) {
                Q_EMIT dragged(QPoint(dx < 0 ? -1 : 1, 0));
                m_mouseDown = false; //mouse action has been handled
            }
        } else {
            if (qAbs(dy) >= diamondSize.height() * draggingFuzziness) {
                Q_EMIT dragged(QPoint(0, dy < 0 ? -1 : 1));
                m_mouseDown = false;
            }
        }
    }
}

void Diamond::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_mouseDown && boundingRect().contains(event->pos())) {
        Q_EMIT clicked();
        m_mouseDown = false;
    }
}

#include "moc_diamond.cpp"
