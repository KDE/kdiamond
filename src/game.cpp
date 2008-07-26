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

#include "game.h"
#include "board.h"
#include "game-state.h"
#include "mainwindow.h"
#include "settings.h"

#include <QMouseEvent>
#include <QWheelEvent>
#include <KLocalizedString>

Game::Game(KGameDifficulty::standardLevel difficulty, MainWindow *mainWindow = 0)
	: QGraphicsView(mainWindow)
	, m_board(new Board(this, difficulty))
	, m_mainWindow(mainWindow)
	, m_state(new KDiamond::GameState)
{
	//init board
	connect(m_mainWindow, SIGNAL(updateScheduled(int)), m_board, SLOT(update()));
	connect(m_state, SIGNAL(stateChanged(KDiamond::State)), m_board, SLOT(stateChange(KDiamond::State)));
	connect(m_state, SIGNAL(message(const QString&)), m_board, SLOT(message(const QString&)));
	//init view
	setScene(m_board);
	setFrameStyle(QFrame::NoFrame);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	//"What's this?" context help
	setWhatsThis(i18n("<h3>Rules of Game</h3><p>Your goal is to assemble lines of at least three similar diamonds. Click on two adjacent diamonds to swap them.</p><p>Earn extra points by building cascades, and extra seconds by assembling big lines or multiple lines at one time.</p>"));
}

Game::~Game()
{
	delete m_board;
	delete m_state;
}

Board *Game::board() const
{
	return m_board;
}

KDiamond::GameState *Game::state() const
{
	return m_state;
}

void Game::mouseReleaseEvent(QMouseEvent *event)
{
	if (m_state->state() != KDiamond::Playing)
		event->ignore(); //block input after the end of the game
	else
		QGraphicsView::mouseReleaseEvent(event);
}

void Game::resizeEvent(QResizeEvent *)
{
	int newWidth = width(), newHeight = height();
	m_board->resizeScene(newWidth, newHeight);
	fitInView(QRectF(0.0, 0.0, newWidth, newHeight));
}

void Game::updateTheme()
{
	//this makes the board reload all pixmaps
	m_board->resizeScene(width(), height(), true);
}

void Game::wheelEvent(QWheelEvent *event)
{
	event->ignore(); //prevent user-triggered scrolling
}

#include "game.moc"
