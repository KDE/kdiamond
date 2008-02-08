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
#include "mainwindow.h"

#include <QTime>

Game::Game(KGameDifficulty::standardLevel difficulty, MainWindow *mainWindow = 0)
{
    //init timers
    m_gameTime = new QTime;
    m_gameTime->start();
    m_pauseTime = new QTime;
    m_pauseTime->start(); //now we can always call restart()
    connect(mainWindow, SIGNAL(updateScheduled(int)), this, SLOT(update(int)), Qt::DirectConnection);
    //init board
    m_board = new Board(difficulty, this, mainWindow);
    connect(mainWindow, SIGNAL(updateScheduled(int)), m_board, SLOT(update(int)), Qt::DirectConnection);
    connect(m_board, SIGNAL(diamondsRemoved(int, int)), this, SLOT(diamondsRemoved(int, int)));
    //internal values
    m_points = 0;
    m_secondsEarned = 0;
    m_secondsRemaining = 0;
    m_paused = false;
}

Game::~Game()
{
    delete m_board;
    delete m_gameTime;
    delete m_pauseTime;
}

Board *Game::board()
{
    return m_board;
}

void Game::update(int /*milliseconds*/)
{
    if (m_paused)
        return;
    //calculate new time
    int secondsRemaining = KDiamond::GameDuration + m_secondsPaused + m_secondsEarned - (m_gameTime->elapsed() / 1000);
    if (secondsRemaining <= 0)
        emit gameFinished(m_points);
    else if (m_secondsRemaining != secondsRemaining)
        emit remainingTimeChanged(secondsRemaining);
    m_secondsRemaining = secondsRemaining;
}

void Game::diamondsRemoved(int count, int cascade)
{
    m_points += cascade;
    emit pointsChanged(m_points);
    //If more than three diamonds were removed, give an extra second.
    if (count > 3)
        ++m_secondsEarned;
    update(0); //calculate new remaining time
}

void Game::pause(bool paused)
{
    if (!m_paused && paused)
        m_pauseTime->restart();
    else if (m_paused && !paused)
        m_secondsPaused += m_pauseTime->elapsed() / 1000; //add pause time to calculate time correctly
    m_paused = paused;
}

#include "game.moc"
