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

#ifndef KDIAMOND_MAINWINDOW_H
#define KDIAMOND_MAINWINDOW_H

class Game;
#include "game-state.h"

class QTime;
class KAction;
class KActionMenu;
#include <KXmlGuiWindow>

namespace KDiamond
{
	class InfoBar;
	class View;
}

class MainWindow : public KXmlGuiWindow
{
	Q_OBJECT
	public:
		MainWindow(QWidget *parent = 0);
		~MainWindow();
	public Q_SLOTS:
		void startGame(KDiamond::Mode mode);
		void startGameDispatcher();
		void stateChange(KDiamond::State state);
		void gameIsOver();
		void showHighscores();

		void configureNotifications();
		void configureSettings();
		void loadSettings();
	Q_SIGNALS:
		void pause(bool paused);
	protected Q_SLOTS:
		void pausedAction(bool paused);
	private:
		KDiamond::GameState* m_gameState;
		Game* m_game;
		KDiamond::View* m_view;

		KDiamond::InfoBar* m_infoBar;

		KActionMenu *m_newAct;
		KAction *m_newTimedAct;
		KAction *m_newUntimedAct;
		KAction *m_pauseAct;
		KAction *m_hintAct;
};

#endif //KDIAMOND_MAINWINDOW_H
