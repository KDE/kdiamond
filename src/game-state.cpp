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

#include "game-state.h"
#include "settings.h"

#include <QTime>
#include <QTimer>
#include <KLocalizedString>
#include <KNotification>

namespace KDiamond
{

	class GameStatePrivate
	{
		public:
			GameStatePrivate();
			~GameStatePrivate();

			QTime m_gameTime, m_pauseTime;
			QTimer m_updateTimer;

			Mode m_mode;
			State m_state;
			int m_earnedMilliseconds, m_leftMilliseconds, m_pausedMilliseconds;
			int m_points, m_cascade;
	};

}

KDiamond::GameStatePrivate::GameStatePrivate()
	//these should be the same values as in KDiamond::GameState::startNewGame
	: m_mode(Settings::untimed() ? KDiamond::UntimedGame : KDiamond::NormalGame)
	, m_state(KDiamond::Playing)
	, m_earnedMilliseconds(0)
	, m_leftMilliseconds(0)
	, m_pausedMilliseconds(0)
	, m_points(0)
	, m_cascade(0)
{
	m_gameTime.start();
	m_pauseTime.start(); //now we can always call restart() when we need it
}

KDiamond::GameStatePrivate::~GameStatePrivate()
{
}

KDiamond::GameState::GameState()
	: p(new KDiamond::GameStatePrivate)
{
	connect(&p->m_updateTimer, SIGNAL(timeout()), this, SLOT(update()));
	p->m_updateTimer.start(500);
}

KDiamond::GameState::~GameState()
{
	delete p;
}

KDiamond::Mode KDiamond::GameState::mode() const
{
	return p->m_mode;
}

KDiamond::State KDiamond::GameState::state() const
{
	return p->m_state;
}

int KDiamond::GameState::leftTime() const
{
	return p->m_leftMilliseconds;
}

int KDiamond::GameState::points() const
{
	return p->m_points;
}

void KDiamond::GameState::setMode(KDiamond::Mode mode)
{
	p->m_mode = mode;
	Settings::setUntimed(p->m_mode == KDiamond::UntimedGame);
	update(true); //recalculate time
}

void KDiamond::GameState::setState(KDiamond::State state)
{
	if (p->m_state == KDiamond::Finished) //cannot be changed (except with startNewGame slot)
		return;
	//check for important transitions
	if ((p->m_state == KDiamond::PausedUser || p->m_state == KDiamond::PausedAnimation) && state == KDiamond::Playing)
	{
		//resuming from paused state
		p->m_pausedMilliseconds += p->m_pauseTime.elapsed();
		update(true); //recalculate time
		emit message(QString()); //flush message
	}
	else if (p->m_state == KDiamond::Playing && (state == KDiamond::PausedUser || state == KDiamond::PausedAnimation))
	{
		//going to paused state
		p->m_pauseTime.restart();
		emit message(i18n("Click the pause button again to resume the game."));
	}
	//set new state
	p->m_state = state;
	emit stateChanged(state);
	if (state == KDiamond::Finished)
	{
		p->m_updateTimer.stop(); //no updates needed anymore
		KNotification::event("gamefinished");
		emit message(i18nc("Not meant like 'You have lost', more like 'Time is up'.", "Game over."));
	}
}

void KDiamond::GameState::addPoints(int removedDiamonds)
{
	p->m_points += ++p->m_cascade;
	if (removedDiamonds > 3)
		++p->m_earnedMilliseconds;
	emit pointsChanged(p->m_points);
	update(true); //recalculate time
}

void KDiamond::GameState::resetCascadeCounter()
{
	p->m_cascade = 0;
}

void KDiamond::GameState::startNewGame()
{
	p->m_gameTime.restart();
	//p->m_mode does not need to be resetted as it is kept in sync with Settings::untimed()
	//these should be the same values as in KDiamond::GameStatePrivate constructor
	p->m_state = KDiamond::Playing;
	p->m_earnedMilliseconds = 0;
	p->m_leftMilliseconds = 0;
	p->m_pausedMilliseconds = 0;
	p->m_points = 0;
	p->m_cascade = 0;
	update(true); //recalculate time
	emit message(QString()); //flush message
	emit stateChanged(p->m_state);
	emit pointsChanged(p->m_points);
}

void KDiamond::GameState::update(bool forceRecalculation)
{
	//will not recalculate time when not playing a normal game (unless forced)
	if ((p->m_state != KDiamond::Playing || p->m_mode == KDiamond::UntimedGame) && !forceRecalculation)
		return;
	//calculate new time
	const int leftMilliseconds = 1000 * KDiamond::GameDuration + p->m_earnedMilliseconds + p->m_pausedMilliseconds - p->m_gameTime.elapsed();
	const int leftSeconds = leftMilliseconds / 1000;
	if (leftSeconds <= 0)
		setState(KDiamond::Finished);
	if (p->m_leftMilliseconds / 1000 != leftSeconds)
		emit leftTimeChanged(leftSeconds);
	p->m_leftMilliseconds = leftMilliseconds;
}

#include "game-state.moc"
