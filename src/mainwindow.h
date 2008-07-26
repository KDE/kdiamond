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

#ifndef KDIAMOND_MAINWINDOW_H
#define KDIAMOND_MAINWINDOW_H

class Container;
class Game;
#include "game-state.h"

class KToggleAction;
class QTime;
class QTimer;
#include <KXmlGuiWindow>

namespace KDiamond
{
	//maximum update interval in milliseconds
	const int UpdateInterval = 40;
}

class MainWindow : public KXmlGuiWindow
{
	Q_OBJECT
	public:
		MainWindow(QWidget *parent = 0);
		~MainWindow();
	public slots:
		void startGame();
		void stateChange(KDiamond::State state);
		void gameIsOver();
		void showHighscores();
		void close();

		void configureNotifications();
		void configureSettings();
		void loadSettings();
		void showMinutesOnTimer(bool showMinutes);
	signals:
		void showHint();
		void pause(bool paused);
		void updateScheduled(int milliseconds);
	protected:
		virtual void closeEvent(QCloseEvent *);
	protected slots:
		void pausedAction(bool paused);
		void untimedAction(bool untimed);

		void updateTime();
		void updatePoints(int points);
		void updateMoves(int moves);
		void updateRemainingTime(int remainingSeconds);
	private:
		Game *m_game;
		Container *m_container;

		QTime *m_updateTime;
		QTimer *m_updateTimer;
};

#endif //KDIAMOND_MAINWINDOW_H
