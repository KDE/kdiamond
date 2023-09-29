/*
    SPDX-FileCopyrightText: 2008-2010 Stefan Majewsky <majewsky@gmx.net>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

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

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
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
    QList<Diamond *> m_diamonds;
    QList<Diamond *> m_activeSelectors, m_inactiveSelectors;
    QList<QAbstractAnimation *> m_runningAnimations;
};
}

#endif // KDIAMOND_BOARD_H
