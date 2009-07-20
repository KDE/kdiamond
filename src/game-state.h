/***************************************************************************
 *   Copyright 2008-2009 Stefan Majewsky <majewsky.stefan@ages-skripte.org>
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

#ifndef KDIAMOND_GAMESTATE_H
#define KDIAMOND_GAMESTATE_H

#include <QObject>

namespace KDiamond
{

	class GameStatePrivate;

	//base duration of a game in seconds
	const int GameDuration = 200;

	enum Mode
	{
		NormalGame,
		UntimedGame
	};
	enum State
	{
		Playing,
		Paused,
		Finished
	};

	class GameState : public QObject
	{
		Q_OBJECT
		public:
			GameState();
			~GameState();

			Mode mode() const;
			State state() const;
			int leftTime() const;
			int points() const;

			void setMode(Mode mode);
			void setState(State state);
		public Q_SLOTS:
			void addPoints(int removedDiamonds);
			void removePoints(int points);
			void resetCascadeCounter();
			void startNewGame();
			void update(bool forceRecalculation = false);
		Q_SIGNALS:
			void message(const QString& text); //text == QString() means: hide the message popup
			void stateChanged(KDiamond::State state); //warning: moc needs the full identifier of KDiamond::State
			void pointsChanged(int points);
			void leftTimeChanged(int seconds);
		protected:
			virtual void timerEvent(QTimerEvent* event);
		private:
			GameStatePrivate *p;
	};

}

#endif // KDIAMOND_GAMESTATE_H
