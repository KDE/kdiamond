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
#include "game-state.h"
#include "infobar.h"
#include "renderer.h"
#include "settings.h"
#include "view.h"

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
#include <KToggleAction>

MainWindow::MainWindow(QWidget *parent)
	: KXmlGuiWindow(parent)
	, m_game(new KDiamond::GameState)
	, m_board(0)
	, m_view(new KDiamond::View)
	, m_infoBar(new KDiamond::InfoBar(this))
	, m_updateTime(new QTime)
	, m_updateTimer(new QTimer)
{
	//init timers and randomizer (necessary for the board)
	connect(m_updateTimer, SIGNAL(timeout()), this, SLOT(updateTime()), Qt::DirectConnection);
	m_updateTime->start();
	qsrand(time(0));
	//init GUI - actions
	KStandardGameAction::gameNew(this, SLOT(startGame()), actionCollection());
	KStandardGameAction::highscores(this, SLOT(showHighscores()), actionCollection());
	KStandardGameAction::pause(this, SLOT(pausedAction(bool)), actionCollection());
	KStandardGameAction::quit(kapp, SLOT(quit()), actionCollection());
	KStandardGameAction::hint(0, 0, actionCollection());
	KStandardAction::preferences(this, SLOT(configureSettings()), actionCollection());
	KStandardAction::configureNotifications(this, SLOT(configureNotifications()), actionCollection());
	KToggleAction *showMinutes = actionCollection()->add<KToggleAction>("show_minutes");
	showMinutes->setText(i18n("Show minutes on timer"));
	showMinutes->setChecked(Settings::showMinutes());
	connect(showMinutes, SIGNAL(triggered(bool)), m_infoBar, SLOT(setShowMinutes(bool)));
	KToggleAction *untimed = actionCollection()->add<KToggleAction>("untimed");
	untimed->setText(i18n("Untimed Game"));
	untimed->setChecked(Settings::untimed());
	connect(untimed, SIGNAL(triggered(bool)), this, SLOT(untimedAction(bool)));
	connect(untimed, SIGNAL(triggered(bool)), m_infoBar, SLOT(setUntimed(bool)));
	//init GUI - statusbar etc.
	setAutoSaveSettings();
	//init GUI - center area
	setCentralWidget(m_view);
	connect(m_view, SIGNAL(resized()), this, SLOT(updateTheme()));
	//init game state
	connect(m_game, SIGNAL(stateChanged(KDiamond::State)), this, SLOT(stateChange(KDiamond::State)));
	connect(m_game, SIGNAL(pointsChanged(int)), m_infoBar, SLOT(updatePoints(int)));
	connect(m_game, SIGNAL(leftTimeChanged(int)), m_infoBar, SLOT(updateRemainingTime(int)));
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
	Settings::self()->writeConfig();
	m_updateTimer->stop();
	delete m_updateTime;
	delete m_updateTimer;
	delete m_game;
}

void MainWindow::startGame()
{
	//save (eventually changed) difficulty level
	KGameDifficulty::standardLevel level = KGameDifficulty::level();
	Settings::setSkill((int) level);
	//delete old board
	if (m_board != 0)
		delete m_board;
	//start new game
	m_game->startNewGame();
	m_board = new Board(m_game, KGameDifficulty::level());
	updateTheme(false); //initalizes the theme
	connect(this, SIGNAL(updateScheduled(int)), m_board, SLOT(update()));
	connect(m_game, SIGNAL(stateChanged(KDiamond::State)), m_board, SLOT(stateChange(KDiamond::State)));
	connect(m_game, SIGNAL(message(const QString&)), m_board, SLOT(message(const QString&)));
	connect(m_board, SIGNAL(numberMoves(int)), m_infoBar, SLOT(updateMoves(int)));
	connect(m_board, SIGNAL(pendingAnimationsFinished()), this, SLOT(gameIsOver()));
	m_view->setScene(m_board);
	m_view->fitInView(0, 0, m_view->width(), m_view->height());
	//reset the Pause button's state
	//TODO: move this to the general changeState slot
	QAction *pauseAction = actionCollection()->action(KStandardGameAction::name(KStandardGameAction::Pause));
	pauseAction->setChecked(false);
	pauseAction->setEnabled(true);
	QAction *hintAction = actionCollection()->action(KStandardGameAction::name(KStandardGameAction::Hint));
	hintAction->setEnabled(true);
	connect(hintAction, SIGNAL(triggered()), m_board, SLOT(showHint()));
	//reset status bar
	//TODO: is this necessary?
	m_infoBar->updatePoints(0);
	m_infoBar->updateRemainingTime(KDiamond::GameDuration);
	//reset timers
	m_updateTimer->start(KDiamond::UpdateInterval);
	m_updateTime->restart();
}

void MainWindow::stateChange(KDiamond::State state)
{
	//disable pause button once game is over
	if (state == KDiamond::Finished)
	{
		QAction *pauseAction = actionCollection()->action(KStandardGameAction::name(KStandardGameAction::Pause));
		pauseAction->setChecked(false);
		pauseAction->setEnabled(false);
	}
	//hint is only possible while playing
	QAction *hintAction = actionCollection()->action(KStandardGameAction::name(KStandardGameAction::Hint));
	hintAction->setEnabled(state == KDiamond::Playing);
}

void MainWindow::gameIsOver()
{
	m_updateTimer->stop();
	//report score
	KScoreDialog dialog(KScoreDialog::Name | KScoreDialog::Score, this);
	dialog.setConfigGroup(KGameDifficulty::levelString()); //TODO: react to localization
	dialog.addScore(m_game->points());
	dialog.exec();
}

void MainWindow::showHighscores()
{
	m_game->setState(KDiamond::Paused);
	if (m_game->state() != KDiamond::Finished)
		actionCollection()->action("game_pause")->setChecked(true);
	KScoreDialog dialog(KScoreDialog::Name | KScoreDialog::Score, this);
	dialog.exec();
}

void MainWindow::pausedAction(bool paused)
{
	m_game->setState(paused ? KDiamond::Paused : KDiamond::Playing);
}

void MainWindow::untimedAction(bool untimed)
{
	m_game->setMode(untimed ? KDiamond::UntimedGame : KDiamond::NormalGame);
}

void MainWindow::updateTime()
{
	int milliseconds = m_updateTime->elapsed();
	m_updateTime->restart();
	emit updateScheduled(milliseconds); //TODO: are the milliseconds needed?
}

void MainWindow::updateTheme(bool force)
{
	if (m_board)
		m_board->resizeScene(m_view->width(), m_view->height(), force);
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
	dialog->addPage(new KGameThemeSelector(dialog, Settings::self(), KGameThemeSelector::NewStuffEnableDownload), i18n("Theme"), "games-config-theme");
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
		updateTheme(true);
		m_board->invalidate(m_board->sceneRect(), QGraphicsScene::BackgroundLayer);
	}
}

#include "mainwindow.moc"
