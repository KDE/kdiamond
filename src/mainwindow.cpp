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
#include "renderer.h"
#include "settings.h"

#include <time.h>
#include <QCloseEvent>
#include <QTime>
#include <QTimer>
#include <KActionCollection>
#include <KApplication>
#include <KConfigDialog>
#include <KGameDifficulty>
#include <KGameThemeSelector>
#include <KLocalizedString>
#include <KMessageBox>
#include <KNotifyConfigWidget>
#include <KScoreDialog>
#include <KStandardAction>
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
    KStandardGameAction::quit(kapp, SLOT(quit()), actionCollection());
    KStandardAction::preferences(this, SLOT(configureSettings()), actionCollection());
    KStandardAction::configureNotifications(this, SLOT(configureNotifications()), actionCollection());
    KToggleAction *showMinutes = actionCollection()->add<KToggleAction>("show_minutes");
    showMinutes->setText(i18n("Show minutes on timer"));
    showMinutes->setChecked(Settings::showMinutes());
    connect(showMinutes, SIGNAL(triggered(bool)), this, SLOT(showMinutesOnTimer(bool)));
    //init GUI - statusbar etc.
    statusBar()->insertPermanentItem(i18n("Points: %1", 0), 1, 1);
    statusBar()->insertPermanentItem(i18np("Time left: 1 second", "Time left: %1 seconds", 0), 2, 1);
    setAutoSaveSettings();
    //init GUI - center area
    m_game = 0;
    m_container = new Container(this);
    setCentralWidget(m_container);
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
        KGameDifficulty::setLevel((KGameDifficulty::standardLevel) (skill));
    //late GUI initiation
    setupGUI(QSize(300, 400)); //TODO: find better solution for a minimum size
    setCaption(i18nc("The application's name", "KDiamond"));
}

MainWindow::~MainWindow()
{
    m_updateTimer->stop();
    delete m_updateTime;
    delete m_updateTimer;
    if (m_game != 0)
        delete m_game;
}

void MainWindow::startGame()
{
    //save (eventually changed) difficulty level
    KGameDifficulty::standardLevel level = KGameDifficulty::level();
    Settings::setSkill((int) level);
    //delete old game
    if (m_game != 0)
    {
        delete m_game;
        m_game = 0;
    }
    //start new game
    m_game = new Game(KGameDifficulty::level(), this);
    connect(this, SIGNAL(pause(bool)), m_game, SLOT(pause(bool)));
    connect(this, SIGNAL(pause(bool)), m_game->board(), SLOT(pause(bool)));
    connect(m_game, SIGNAL(pointsChanged(int)), this, SLOT(updatePoints(int)));
    connect(m_game, SIGNAL(remainingTimeChanged(int)), this, SLOT(updateRemainingTime(int)));
    connect(m_game, SIGNAL(timeIsUp(int)), this, SLOT(timeIsUp()));
    connect(m_game->board(), SIGNAL(gameOver()), this, SLOT(gameOver()));
    m_container->setWidget(m_game);
    //reset the Pause button's state
    QAction *pauseAction = actionCollection()->action("game_pause");
    pauseAction->setChecked(false);
    pauseAction->setEnabled(true);
    //reset status bar
    updatePoints(0);
    updateRemainingTime(KDiamond::GameDuration);
}

void MainWindow::timeIsUp()
{
    updateRemainingTime(0);
    //reset the Pause button's state
    QAction *pauseAction = actionCollection()->action("game_pause");
    pauseAction->setChecked(false);
    pauseAction->setEnabled(false);
}

void MainWindow::gameOver()
{
    //report score
    KScoreDialog dialog(KScoreDialog::Name | KScoreDialog::Score, this);
    dialog.setConfigGroup(KGameDifficulty::levelString());
    dialog.addScore(m_game->points());
    dialog.exec();
}

void MainWindow::showHighscores()
{
    m_game->pause(true);
    m_game->board()->pause(true);
    actionCollection()->action("game_pause")->setChecked(true);
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
    //store the time: if remainingSeconds == -1, the old time is just re-rendered (used by the configuration action MainWindow::showMinutesOnTimer)
    static int storeRemainingSeconds = 0;
    if (remainingSeconds == -1)
        remainingSeconds = storeRemainingSeconds;
    else
        storeRemainingSeconds = remainingSeconds;
    //split time in seconds and minutes if wanted
    int seconds, minutes;
    if (Settings::showMinutes())
    {
        seconds = remainingSeconds % 60;
        minutes = remainingSeconds / 60;
    }
    else
    {
        seconds = remainingSeconds;
        minutes = 0; //the minutes do not appear in the output when minutes == 0
    }
    //compose new string
    QString sOutput;
    if (minutes == 0)
        sOutput = i18n("Time left: %1", i18np("1 second", "%1 seconds", seconds));
    else if (seconds == 0)
        sOutput = i18n("Time left: %1", i18np("1 minute", "%1 minutes", minutes));
    else
        sOutput = i18nc("The two parameters are strings like '2 minutes' or '1 second'.", "Time left: %1, %2", i18np("1 minute", "%1 minutes", minutes), i18np("1 second", "%1 seconds", seconds));
    statusBar()->changeItem(sOutput, 2);
}

void MainWindow::showMinutesOnTimer(bool showMinutes)
{
    Settings::setShowMinutes(showMinutes);
    updateRemainingTime(-1);
}

void MainWindow::configureNotifications()
{
    KNotifyConfigWidget::configure(this);
}

void MainWindow::configureSettings()
{
    if (KConfigDialog::showDialog("settings"))
        return;
    KConfigDialog *dialog = new KConfigDialog(this, "settings", Settings::self());
    dialog->addPage(new KGameThemeSelector(dialog, Settings::self(), KGameThemeSelector::NewStuffDisableDownload), i18n("Theme"), "games-config-theme");
    connect(dialog, SIGNAL(settingsChanged(const QString&)), this, SLOT(loadSettings()));
    dialog->setHelp(QString(), "kdiamond");
    dialog->show();
}

void MainWindow::loadSettings()
{
    if (!Renderer::self()->loadTheme(Settings::theme()))
    {
        KMessageBox::error(this, i18n("Failed to load \"%1\" theme. Please check your installation.", Settings::theme()));
        return;
    }
    //redraw game scene if necessary
    if (m_game != 0)
    {
        m_game->updateTheme(); //resets all pixmaps
        m_game->scene()->invalidate(m_game->scene()->sceneRect(), QGraphicsScene::BackgroundLayer);
    }
}

#include "mainwindow.moc"
