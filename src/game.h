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

#ifndef KDIAMOND_GAME_H
#define KDIAMOND_GAME_H

class Diamond;
#include "game-state.h"

class QAbstractAnimation;
#include <QGraphicsScene>
#include <KgDifficulty>
class KGamePopupItem;
class KGameRenderer;

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

	KGameRenderer* renderer();
}

class Game : public QGraphicsScene
{
	Q_OBJECT
	public:
		Game(KDiamond::GameState* state, KgDifficultyLevel::StandardLevel difficulty);
	public Q_SLOTS:
		void updateGraphics();

		void clickDiamond(const QPoint& point);
		void dragDiamond(const QPoint& point, const QPoint& direction);

		void animationFinished();
		void message(const QString &message);
		void stateChange(KDiamond::State state);
		void showHint();
	Q_SIGNALS:
		void boardResized();
		void numberMoves(int moves);
		void pendingAnimationsFinished();
	protected:
		virtual void timerEvent(QTimerEvent* event);
	private:
		QList<QPoint> findCompletedRows();
		void getMoves();
	private:
		QList<KDiamond::Job> m_jobQueue;
		QList<QPoint> m_availableMoves, m_swappingDiamonds;
		int m_timerId;

		KDiamond::Board* m_board;
		KDiamond::GameState *m_gameState;

		KGamePopupItem *m_messenger;
};

#endif //KDIAMOND_GAME_H
