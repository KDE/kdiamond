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

#ifndef KDIAMOND_BOARD_H
#define KDIAMOND_BOARD_H

class Diamond;

class QAbstractAnimation;
#include <QGraphicsItem>
class KGameRenderer;

namespace KDiamond
{
class Board : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit Board(KGameRenderer *renderer);

    int gridSize() const;
    Diamond *diamond(const QPoint &point) const;

    bool hasDiamond(const QPoint &point) const;
    bool hasRunningAnimations() const;
    QList<QPoint> selections() const;
    bool hasSelection(const QPoint &point) const;
    void setSelection(const QPoint &point, bool selected);
    void clearSelection();

    void removeDiamond(const QPoint &point);
    void swapDiamonds(const QPoint &point1, const QPoint &point2);
    void fillGaps();

    KGameRenderer *renderer() const;

    virtual QRectF boundingRect() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
public Q_SLOTS:
    void setPaused(bool paused);
Q_SIGNALS:
    void animationsFinished();
    void clicked(const QPoint &point);
    void dragged(const QPoint &point, const QPoint &direction);
private Q_SLOTS:
    void slotAnimationFinished();
    void slotClicked();
    void slotDragged(const QPoint &direction);
private:
    struct MoveAnimSpec {
        Diamond *diamond;
        QPointF from, to;
    };
    QPoint findDiamond(Diamond *diamond) const;
    Diamond *&rDiamond(const QPoint &point);
    Diamond *spawnDiamond(int color);
    void spawnMoveAnimations(const QList<MoveAnimSpec> &specs);

    static const int MoveDuration;
    static const int RemoveDuration;

    int m_difficultyIndex, m_size, m_colorCount;
    QList<QPoint> m_selections;
    bool m_paused;

    KGameRenderer *m_renderer;
    QVector<Diamond *> m_diamonds;
    QList<Diamond *> m_activeSelectors, m_inactiveSelectors;
    QList<QAbstractAnimation *> m_runningAnimations;
};
}

#endif // KDIAMOND_BOARD_H
