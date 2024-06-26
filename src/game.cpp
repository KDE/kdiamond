/*
    SPDX-FileCopyrightText: 2008-2010 Stefan Majewsky <majewsky@gmx.net>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "game.h"
#include "board.h"
#include "diamond.h"
#include "settings.h"

#include <cmath>
#include <QPainter>
#include <QRandomGenerator>
#include <QTimerEvent>
#include <KGamePopupItem>
#include <KGameGraphicsViewRenderer>
#include <KGameTheme>
#include <KGameThemeProvider>
#include <KNotification>

const int UpdateInterval = 40;

Game::Game(KDiamond::GameState *state, KGameGraphicsViewRenderer *renderer)
    : m_timerId(-1)
    , m_board(new KDiamond::Board(renderer))
    , m_gameState(state)
    , m_messenger(new KGamePopupItem)
{
    connect(m_board, &KDiamond::Board::animationsFinished, this, &Game::animationFinished);
    connect(m_board, &KDiamond::Board::clicked, this, &Game::clickDiamond);
    connect(m_board, &KDiamond::Board::dragged, this, &Game::dragDiamond);
    //init scene (with some default scene size that makes board coordinates equal scene coordinates)
    const int minSize = m_board->gridSize();
    setSceneRect(0.0, 0.0, minSize, minSize);
    connect(this, &Game::sceneRectChanged, this, &Game::updateGraphics);
    connect(renderer->themeProvider(), &KGameThemeProvider::currentThemeChanged, this, &Game::updateGraphics);
    addItem(m_board);
    //init messenger
    m_messenger->setMessageOpacity(0.8);
    m_messenger->setHideOnMouseClick(false);
    addItem(m_messenger);
    //init time management
    m_timerId = startTimer(UpdateInterval);
    //schedule late initialisation
    m_jobQueue << KDiamond::UpdateAvailableMovesJob;
}

//Checks amount of possible moves remaining
void Game::getMoves()
{
#define C(X, Y) (m_board->hasDiamond(QPoint(X, Y)) ? m_board->diamond(QPoint(X, Y))->color() : KDiamond::Selection)
    m_availableMoves.clear();
    KDiamond::Color curColor;
    const int gridSize = m_board->gridSize();
    for (int x = 0; x < gridSize; ++x) {
        for (int y = 0; y < gridSize; ++y) {
            curColor = C(x, y);
            if (curColor == C(x + 1, y)) {
                if (curColor == C(x - 2, y)) {
                    m_availableMoves.append(QPoint(x - 2, y));
                }
                if (curColor == C(x - 1, y - 1)) {
                    m_availableMoves.append(QPoint(x - 1, y - 1));
                }
                if (curColor == C(x - 1, y + 1)) {
                    m_availableMoves.append(QPoint(x - 1, y + 1));
                }
                if (curColor == C(x + 3, y)) {
                    m_availableMoves.append(QPoint(x + 3, y));
                }
                if (curColor == C(x + 2, y - 1)) {
                    m_availableMoves.append(QPoint(x + 2, y - 1));
                }
                if (curColor == C(x + 2, y + 1)) {
                    m_availableMoves.append(QPoint(x + 2, y + 1));
                }
            }
            if (curColor == C(x + 2, y)) {
                if (curColor == C(x + 1, y - 1)) {
                    m_availableMoves.append(QPoint(x + 1, y - 1));
                }
                if (curColor == C(x + 1, y + 1)) {
                    m_availableMoves.append(QPoint(x + 1, y + 1));
                }
            }
            if (curColor == C(x, y + 1)) {
                if (curColor == C(x, y - 2)) {
                    m_availableMoves.append(QPoint(x, y - 2));
                }
                if (curColor == C(x - 1, y - 1)) {
                    m_availableMoves.append(QPoint(x - 1, y - 1));
                }
                if (curColor == C(x + 1, y - 1)) {
                    m_availableMoves.append(QPoint(x + 1, y - 1));
                }
                if (curColor == C(x, y + 3)) {
                    m_availableMoves.append(QPoint(x, y + 3));
                }
                if (curColor == C(x - 1, y + 2)) {
                    m_availableMoves.append(QPoint(x - 1, y + 2));
                }
                if (curColor == C(x + 1, y + 2)) {
                    m_availableMoves.append(QPoint(x + 1, y + 2));
                }
            }
            if (curColor == C(x, y + 2)) {
                if (curColor == C(x - 1, y + 1)) {
                    m_availableMoves.append(QPoint(x - 1, y + 1));
                }
                if (curColor == C(x + 1, y + 1)) {
                    m_availableMoves.append(QPoint(x + 1, y + 1));
                }
            }
        }
    }
#undef C
    Q_EMIT numberMoves(m_availableMoves.size());
    if (m_availableMoves.isEmpty()) {
        m_board->clearSelection();
        m_gameState->setState(KDiamond::Finished);
    }
}

void Game::updateGraphics()
{
    //calculate new metrics
    const QSize sceneSize = sceneRect().size().toSize();
    const int gridSize = m_board->gridSize();
    const int diamondSize = (int) floor(qMin(
                                            sceneSize.width() / (gridSize + 1.0), //the "+1" and "+0.5" make sure that some space is left on the window for the board border
                                            sceneSize.height() / (gridSize + 0.5)
                                        ));
    const int boardSize = gridSize * diamondSize;
    const int leftOffset = (sceneSize.width() - boardSize) / 2.0;
    QTransform t;
    t.translate(leftOffset, 0).scale(diamondSize, diamondSize);
    m_board->setTransform(t);
    //render background
    QPixmap pix = m_board->renderer()->spritePixmap(QStringLiteral("kdiamond-background"), sceneSize);
    const KGameTheme *theme = m_board->renderer()->theme();
    const bool hasBorder = theme->customData(QStringLiteral("HasBorder")).toInt() > 0;
    if (hasBorder) {
        const qreal borderPercentage = theme->customData(QStringLiteral("BorderPercentage")).toFloat();
        const int padding = borderPercentage * boardSize;
        const int boardBorderSize = 2 * padding + boardSize;
        const QPixmap boardPix = m_board->renderer()->spritePixmap(QStringLiteral("kdiamond-border"), QSize(boardBorderSize, boardBorderSize));
        QPainter painter(&pix);
        painter.drawPixmap(QPoint(leftOffset - padding, -padding), boardPix);
    }
    m_backgroundPixmap = pix;
    update();
}

void Game::drawBackground(QPainter *painter, const QRectF &/*rect*/)
{
    painter->drawPixmap(0, 0, m_backgroundPixmap);
}

void Game::clickDiamond(const QPoint &point)
{
    if (m_gameState->state() != KDiamond::Playing) {
        return;
    }
    //do not allow more than two selections
    const bool isSelected = m_board->hasSelection(point);
    if (!isSelected && m_board->selections().count() == 2) {
        return;
    }
    //select only adjacent diamonds (i.e. if a distant diamond is selected, deselect the first one)
    const auto selections = m_board->selections();
    for (const QPoint &point2 : selections) {
        const int diff = qAbs(point2.x() - point.x()) + qAbs(point2.y() - point.y());
        if (diff > 1) {
            m_board->setSelection(point2, false);
        }
    }
    //toggle selection state
    m_board->setSelection(point, !isSelected);
    if (m_board->selections().count() == 2) {
        m_jobQueue << KDiamond::SwapDiamondsJob;
    }
}

void Game::dragDiamond(const QPoint &point, const QPoint &direction)
{
    //direction must not be null, and must point along one axis
    if ((direction.x() == 0) ^ (direction.y() == 0)) {
        //find target indices
        const QPoint point2 = point + direction;
        if (!m_board->hasDiamond(point2)) {
            return;
        }
        //simulate the clicks involved in this operation
        m_board->clearSelection();
        m_board->setSelection(point, true);
        m_board->setSelection(point2, true);
        m_jobQueue << KDiamond::SwapDiamondsJob;
    }
}

void Game::timerEvent(QTimerEvent *event)
{
    //propagate event to superclass if necessary
    if (event->timerId() != m_timerId) {
        QGraphicsScene::timerEvent(event);
        return;
    }
    //do not handle any jobs while animations are running
    if (m_gameState->state() == KDiamond::Paused || m_board->hasRunningAnimations()) {
        killTimer(m_timerId);
        m_timerId = -1;
        return;
    }
    //anything to do in this update?
    if (m_jobQueue.isEmpty()) {
        return;
    }
    //execute next job in queue
    const KDiamond::Job job = m_jobQueue.takeFirst();
    switch (job) {
    case KDiamond::SwapDiamondsJob: {
        if (m_board->selections().count() != 2) {
            break;    //this can be the case if, during a cascade, two diamonds are selected (inserts SwapDiamondsJob) and then deselected
        }
        //ensure that the selected diamonds are neighbors (this is not necessarily the case as diamonds can move to fill gaps)
        const QList<QPoint> points = m_board->selections();
        m_board->clearSelection();
        const int dx = qAbs(points[0].x() - points[1].x());
        const int dy = qAbs(points[0].y() - points[1].y());
        if (dx + dy != 1) {
            break;
        }
        //start a new cascade
        m_gameState->resetCascadeCounter();
        //copy selection info into another storage (to allow the user to select the next two diamonds while the cascade runs)
        m_swappingDiamonds = points;
        m_jobQueue << KDiamond::RemoveRowsJob; //We already insert this here to avoid another conditional statement.
        [[fallthrough]];
    }
    case KDiamond::RevokeSwapDiamondsJob:
        //invoke movement
        KNotification::event(QStringLiteral("move"));
        m_board->swapDiamonds(m_swappingDiamonds[0], m_swappingDiamonds[1]);
        break;
    case KDiamond::RemoveRowsJob: {
        //find diamond rows and delete these diamonds
        const QList<QPoint> diamondsToRemove = findCompletedRows();
        if (diamondsToRemove.isEmpty()) {
            //no diamond rows were formed by the last move -> revoke movement (unless we are in a cascade)
            if (!m_swappingDiamonds.isEmpty()) {
                m_jobQueue.prepend(KDiamond::RevokeSwapDiamondsJob);
            } else {
                m_jobQueue << KDiamond::UpdateAvailableMovesJob;
            }
        } else {
            //all moves may now be out-dated - flush the moves list
            if (!m_availableMoves.isEmpty()) {
                m_availableMoves.clear();
                Q_EMIT numberMoves(-1);
            }
            //it is now safe to delete the position of the swapping diamonds
            m_swappingDiamonds.clear();
            //report to Game
            m_gameState->addPoints(diamondsToRemove.count());
            //invoke remove animation, then fill gaps immediately after the animation
            KNotification::event(QStringLiteral("remove"));
            for (const QPoint &diamondPos : std::as_const(diamondsToRemove)) {
                m_board->removeDiamond(diamondPos);
            }
            m_jobQueue.prepend(KDiamond::FillGapsJob);
        }
        break;
    }
    case KDiamond::FillGapsJob:
        //fill gaps
        m_board->fillGaps();
        m_jobQueue.prepend(KDiamond::RemoveRowsJob); //allow cascades (i.e. clear rows that have been formed by falling diamonds)
        break;
    case KDiamond::UpdateAvailableMovesJob:
        if (m_gameState->state() != KDiamond::Finished) {
            getMoves();
        }
        break;
    case KDiamond::EndGameJob:
        Q_EMIT pendingAnimationsFinished();
        killTimer(m_timerId);
        m_timerId = -1;
        break;
    }
}

QList<QPoint> Game::findCompletedRows()
{
    //The tactic of this function is brute-force. For now, I do not have a better idea: A friend of mine advised me to look in the environment of moved diamonds, but this is not easy since the re-filling after a deletion can lead to rows that are far away from the original movement. Therefore, we simply search through all diamonds looking for combinations in the horizonal and vertical direction.
    KDiamond::Color currentColor;
    QList<QPoint> diamonds;
    int x, y, xh, yh; //counters
    const int gridSize = m_board->gridSize();
#define C(X, Y) m_board->diamond(QPoint(X, Y))->color()
    //searching in horizontal direction
    for (y = 0; y < gridSize; ++y) {
        for (x = 0; x < gridSize - 2; ++x) { //counter stops at gridSize - 2 to ensure availability of indices x + 1, x + 2
            currentColor = C(x, y);
            if (currentColor != C(x + 1, y)) {
                continue;
            }
            if (currentColor != C(x + 2, y)) {
                continue;
            }
            //If the execution is here, we have found a row of three diamonds starting at (x,y).
            diamonds << QPoint(x, y);
            diamonds << QPoint(x + 1, y);
            diamonds << QPoint(x + 2, y);
            //Does the row have even more elements?
            if (x + 3 >= gridSize) {
                //impossible to locate more diamonds - do not go through the following loop
                x += 2;
                continue;
            }
            for (xh = x + 3; xh <= gridSize - 1; ++xh) {
                if (currentColor == C(xh, y)) {
                    diamonds << QPoint(xh, y);
                } else {
                    break;    //row has stopped before this diamond - no need to continue searching
                }
            }
            x = xh - 1; //do not search at this position in the row anymore (add -1 because x is incremented before the next loop)
        }
    }
    //searching in vertical direction (essentially the same algorithm, just with swapped indices -> no comments here, read the comments above)
    for (x = 0; x < gridSize; ++x) {
        for (y = 0; y < gridSize - 2; ++y) {
            currentColor = C(x, y);
            if (currentColor != C(x, y + 1)) {
                continue;
            }
            if (currentColor != C(x, y + 2)) {
                continue;
            }
            diamonds << QPoint(x, y);
            diamonds << QPoint(x, y + 1);
            diamonds << QPoint(x, y + 2);
            if (y + 3 >= gridSize) {
                y += 2;
                continue;
            }
            for (yh = y + 3; yh <= gridSize - 1; ++yh) {
                if (currentColor == C(x, yh)) {
                    diamonds << QPoint(x, yh);
                } else {
                    break;
                }
            }
            y = yh - 1;
        }
    }
#undef C
    return diamonds;
}

void Game::showHint()
{
    if (m_availableMoves.isEmpty() || !m_board->selections().isEmpty()) {
        return;
    }
    const QPoint location = m_availableMoves.value( QRandomGenerator::global()->bounded(m_availableMoves.size()));
    m_board->setSelection(location, true);
    m_gameState->removePoints(3);
}

void Game::animationFinished()
{
    if (m_timerId == -1) {
        m_timerId = startTimer(UpdateInterval);
    }
}

void Game::stateChange(KDiamond::State state)
{
    m_board->setPaused(state == KDiamond::Paused);
    switch ((int) state) {
    case KDiamond::Finished:
        m_board->clearSelection();
        m_jobQueue << KDiamond::EndGameJob;
        break;
    case KDiamond::Playing:
        if (m_timerId == -1) {
            m_timerId = startTimer(UpdateInterval);
        }
        break;
    }
}

void Game::message(const QString &message)
{
    if (message.isEmpty()) {
        m_messenger->forceHide();
    } else {
        m_messenger->showMessage(message, KGamePopupItem::TopLeft);
    }
}

#include "moc_game.cpp"
