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

#include "mainwindow.h"
#include "board.h"
#include "container.h"
#include "game.h"
#include "greeter.h"
#include "settings.h"

#include <time.h>
#include <QCloseEvent>
#include <QTime>
#include <QTimer>
#include <KActionCollection>
#include <KGameDifficulty>
#include <KLocalizedString>
#include <KScoreDialog>
#include <KStandardGameAction>
#include <KStatusBar>
#include <KToggleAction>

MainWindow::MainWindow(QWidget *parent)
    : KXmlGuiWindow(parent)
{
    //init timers and randomizer (necessary for the board)
    m_updateTimer = new QTimer;
    connect(m_updateTimer, SIGNAL(timeout()), this, SLOT(updateTime()), Qt::DirectConnection);
    m_updateTimer->start(KDiamond::UpdateInterval);
    m_updateTime = new QTime;
    m_updateTime->start();
    qsrand(time(0));
    //init GUI - actions
    KStandardGameAction::gameNew(this, SLOT(startGame()), actionCollection());
    KStandardGameAction::highscores(this, SLOT(showHighscores()), actionCollection());
    KStandardGameAction::pause(this, SIGNAL(pause(bool)), actionCollection());
    KStandardGameAction::quit(this, SLOT(close()), actionCollection());
    statusBar()->insertPermanentItem(i18n("Points: %1", 0), 1, 1);
    statusBar()->insertPermanentItem(i18np("Time left: 1 second", "Time left: %1 seconds", 0), 2, 1);
    setAutoSaveSettings();
    //init GUI - center area
    m_game = 0;
    m_container = new Container(this);
    setCentralWidget(m_container);
    m_greeter = new Greeter(this);
    connect(m_greeter, SIGNAL(startGame()), this, SLOT(startGame()));
    m_container->setWidget(m_greeter);
    //difficulty
    KGameDifficulty::init(this, this, SLOT(startGame()));
    KGameDifficulty::addStandardLevel(KGameDifficulty::VeryEasy);
    KGameDifficulty::addStandardLevel(KGameDifficulty::Easy);
    KGameDifficulty::addStandardLevel(KGameDifficulty::Medium);
    KGameDifficulty::addStandardLevel(KGameDifficulty::Hard);
    KGameDifficulty::addStandardLevel(KGameDifficulty::VeryHard);
    //selected skill
    int skill = Settings::skill();
    if (skill < KGameDifficulty::VeryEasy) //includes default values like 0 or 1
        KGameDifficulty::setLevel(KGameDifficulty::Easy);
    else
        KGameDifficulty::setLevel((KGameDifficulty::standardLevel) (Settings::skill()));
    //late GUI initiation
    setupGUI(QSize(550, 400));
    setCaption(i18nc("The application's name", "KDiamond"));
}

MainWindow::~MainWindow()
{
    m_updateTimer->stop();
    delete m_updateTime;
    delete m_updateTimer;
    if (m_game != 0)
        delete m_game;
    if (m_greeter != 0)
        delete m_greeter;
}

void MainWindow::startGame()
{
    //remove what was on the screen
    if (m_game != 0)
        stopGame(false);
    if (m_greeter != 0)
    {
        delete m_greeter;
        m_greeter = 0;
    }
    //reset the Pause button's toggle state
    ((KToggleAction *) actionCollection()->action("game_pause"))->setChecked(false);
    //start new game
    m_game = new Game(KGameDifficulty::level(), this);
    connect(this, SIGNAL(pause(bool)), m_game, SLOT(pause(bool)));
    connect(this, SIGNAL(pause(bool)), m_game->board(), SLOT(pause(bool)));
    connect(m_game, SIGNAL(pointsChanged(int)), this, SLOT(updatePoints(int)));
    connect(m_game, SIGNAL(remainingTimeChanged(int)), this, SLOT(updateRemainingTime(int)));
    connect(m_game, SIGNAL(gameFinished(int)), this, SLOT(finishGame(int)));
    m_container->setWidget(m_game);
    //reset status bar
    updatePoints(0);
    updateRemainingTime(KDiamond::GameDuration);
}

void MainWindow::stopGame(bool showGreeter)
{
    //save (eventually changed) difficulty level
    KGameDifficulty::standardLevel level = KGameDifficulty::level();
    Settings::setSkill((int) level);
    //stop game
    if (m_game != 0)
    {
        delete m_game;
        m_game = 0;
    }
    //show greeter if necessary
    if (showGreeter)
    {
        if (m_greeter == 0)
        {
            m_greeter = new Greeter();
            connect(m_greeter, SIGNAL(startGame()), this, SLOT(startGame()));
        }
        m_container->setWidget(m_greeter);
    }
}

void MainWindow::finishGame(int points)
{
    //stop game
    stopGame(true);
    updateRemainingTime(0);
    //report score
    KScoreDialog dialog(KScoreDialog::Name | KScoreDialog::Score, this);
    dialog.setConfigGroup(KGameDifficulty::levelString());
    dialog.addScore(points);
    dialog.exec();
}

void MainWindow::showHighscores()
{
    KScoreDialog dialog(KScoreDialog::Name | KScoreDialog::Score, this);
    dialog.exec();
}

void MainWindow::close()
{
    Settings::self()->writeConfig();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    close();
    event->accept();
}

void MainWindow::updateTime()
{
    int milliseconds = m_updateTime->elapsed();
    m_updateTime->restart();
    emit updateScheduled(milliseconds);
}

void MainWindow::updatePoints(int points)
{
    statusBar()->changeItem(i18n("Points: %1", points), 1);
}

void MainWindow::updateRemainingTime(int remainingSeconds)
{
    statusBar()->changeItem(i18np("Time left: 1 second", "Time left: %1 seconds", remainingSeconds), 2);
}

#include "mainwindow.moc"
