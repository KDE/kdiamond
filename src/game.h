/*
    SPDX-FileCopyrightText: 2008-2010 Stefan Majewsky <majewsky@gmx.net>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KDIAMOND_GAME_H
#define KDIAMOND_GAME_H

class Diamond;
#include "game-state.h"

#include <QGraphicsScene>
class KGamePopupItem;
class KGameGraphicsViewRenderer;

namespace KDiamond
{
//jobs to be done during the board update
enum Job {
    SwapDiamondsJob = 1, //swap selected diamonds
    RemoveRowsJob, //remove complete rows of diamonds and add points
    RevokeSwapDiamondsJob, //revoke swapping of diamonds (will be requested by the RemoveRowsJob if no rows have been formed)
    FillGapsJob,
    UpdateAvailableMovesJob, //find and count available moves after the board has been changed
    EndGameJob //announce end of game
};

class Board;
}

class Game : public QGraphicsScene
{
    Q_OBJECT
public:
    Game(KDiamond::GameState *state, KGameGraphicsViewRenderer *renderer);
public Q_SLOTS:
    void updateGraphics();

    void clickDiamond(const QPoint &point);
    void dragDiamond(const QPoint &point, const QPoint &direction);

    void animationFinished();
    void message(const QString &message);
    void stateChange(KDiamond::State state);
    void showHint();
Q_SIGNALS:
    void boardResized();
    void numberMoves(int moves);
    void pendingAnimationsFinished();
protected:
    void timerEvent(QTimerEvent *event) override;
    void drawBackground(QPainter *painter, const QRectF &rect) override;
private:
    QList<QPoint> findCompletedRows();
    void getMoves();
private:
    QList<KDiamond::Job> m_jobQueue;
    QList<QPoint> m_availableMoves, m_swappingDiamonds;
    int m_timerId;

    KDiamond::Board *m_board;
    KDiamond::GameState *m_gameState;

    KGamePopupItem *m_messenger;
    QPixmap m_backgroundPixmap;
};

#endif //KDIAMOND_GAME_H
