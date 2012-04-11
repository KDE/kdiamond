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

#include "mainwindow.h"
#include "game.h"
#include "game-state.h"
#include "infobar.h"
#include "settings.h"
#include "view.h"

#include <QCloseEvent>
#include <QPointer>
#include <QTime>
#include <QTimer>
#include <KAction>
#include <KActionMenu>
#include <KActionCollection>
#include <KApplication>
#include <kglobal.h>
#include <KgDifficulty>
#include <KGameRenderer>
#include <KLocalizedString>
#include <KMessageBox>
#include <KNotifyConfigWidget>
#include <KScoreDialog>
#include <KStandardAction>
#include <KStandardGameAction>
#include <KToggleAction>

MainWindow::MainWindow(QWidget *parent)
	: KXmlGuiWindow(parent)
	, m_gameState(new KDiamond::GameState)
	, m_game(0)
	, m_view(new KDiamond::View)
	, m_infoBar(0)
	, m_newAct(new KActionMenu(KIcon( QLatin1String( "document-new") ), i18nc("new game", "&New" ), this))
	, m_newTimedAct(new KAction(i18n("Timed game"), this))
	, m_newUntimedAct(new KAction(i18n("Untimed game"), this))
	, m_selector(KDiamond::renderer()->themeProvider(), KgThemeSelector::EnableNewStuffDownload)
{
	KDiamond::renderer()->setDefaultPrimaryView(m_view);
	//init GUI - "New Action"
	m_newAct->setShortcut(KStandardShortcut::openNew());
	m_newAct->setToolTip(i18n("Start a new game"));
	m_newAct->setWhatsThis(i18n("Start a new game."));
	actionCollection()->addAction( QLatin1String( "game_new" ), m_newAct);
	connect(m_newAct, SIGNAL(triggered()), this, SLOT(startGameDispatcher()));
	m_newAct->addAction(m_newTimedAct);
	connect(m_newTimedAct, SIGNAL(triggered()), this, SLOT(startGameDispatcher()));
	m_newAct->addAction(m_newUntimedAct);
	connect(m_newUntimedAct, SIGNAL(triggered()), this, SLOT(startGameDispatcher()));
	//init GUI - the other actions
	KStandardGameAction::highscores(this, SLOT(showHighscores()), actionCollection());
	m_pauseAct = KStandardGameAction::pause(this, SLOT(pausedAction(bool)), actionCollection());
	KStandardGameAction::quit(kapp, SLOT(quit()), actionCollection());
	m_hintAct = KStandardGameAction::hint(0, 0, actionCollection());
	KStandardAction::preferences(&m_selector, SLOT(showAsDialog()), actionCollection());
	KStandardAction::configureNotifications(this, SLOT(configureNotifications()), actionCollection());
	//difficulty
	KgDifficultyGUI::init(this);
	connect(Kg::difficulty(), SIGNAL(currentLevelChanged(const KgDifficultyLevel*)), SLOT(startGameDispatcher()));
	//late GUI initialisation
	setupGUI(QSize(300, 400)); //TODO: find better solution for a minimum size
	setCaption(i18nc("The application's name", "KDiamond"));
	setCentralWidget(m_view);
	//init statusbar
	m_infoBar = new KDiamond::InfoBar(statusBar());
	connect(m_gameState, SIGNAL(stateChanged(KDiamond::State)), this, SLOT(stateChange(KDiamond::State)));
	connect(m_gameState, SIGNAL(pointsChanged(int)), m_infoBar, SLOT(updatePoints(int)));
	connect(m_gameState, SIGNAL(leftTimeChanged(int)), m_infoBar, SLOT(updateRemainingTime(int)));
	//init game
	startGameDispatcher();
}

MainWindow::~MainWindow()
{
	Settings::self()->writeConfig();
	delete m_game;
	delete m_gameState;
}

void MainWindow::startGameDispatcher()
{
	if (sender() == m_newUntimedAct)
		startGame(KDiamond::UntimedGame);
	else if (sender() == m_newTimedAct)
		startGame(KDiamond::NormalGame);
	else
		//attention: this may also be used by KgDifficulty and the ctor
		startGame(Settings::untimed() ? KDiamond::UntimedGame : KDiamond::NormalGame);
}

void MainWindow::startGame(KDiamond::Mode mode)
{
	//delete old board
	delete m_game;
	//start new game
	m_gameState->startNewGame();
	m_gameState->setMode(mode);
	m_game = new Game(m_gameState);
	connect(m_gameState, SIGNAL(stateChanged(KDiamond::State)), m_game, SLOT(stateChange(KDiamond::State)));
	connect(m_gameState, SIGNAL(message(QString)), m_game, SLOT(message(QString)));
	connect(m_game, SIGNAL(numberMoves(int)), m_infoBar, SLOT(updateMoves(int)));
	connect(m_game, SIGNAL(pendingAnimationsFinished()), this, SLOT(gameIsOver()));
	connect(m_hintAct, SIGNAL(triggered()), m_game, SLOT(showHint()));
	m_view->setScene(m_game);
	//reset status bar
	m_infoBar->setUntimed(mode == KDiamond::UntimedGame);
	m_infoBar->updatePoints(0);
	m_infoBar->updateRemainingTime(KDiamond::GameDuration);
}

void MainWindow::stateChange(KDiamond::State state)
{
	m_pauseAct->setEnabled(state != KDiamond::Finished);
	m_pauseAct->setChecked(state == KDiamond::Paused);
	m_hintAct->setEnabled(state == KDiamond::Playing);
}

void MainWindow::gameIsOver()
{
	//create score info
	KScoreDialog::FieldInfo scoreInfo;
	scoreInfo[KScoreDialog::Score].setNum(m_gameState->points());
	scoreInfo[KScoreDialog::Custom1] = m_gameState->mode() == KDiamond::UntimedGame ? i18n("Untimed") : i18n("Timed");
	//report score
	QPointer<KScoreDialog> dialog = new KScoreDialog(KScoreDialog::Name | KScoreDialog::Score, this);
	dialog->addField(KScoreDialog::Custom1, i18n("Mode"), "mode");
	dialog->initFromDifficulty(Kg::difficulty());
	dialog->addScore(scoreInfo);
	delete dialog;
}

void MainWindow::showHighscores()
{
	//pause game if necessary
	m_gameState->setState(KDiamond::Paused);
	if (m_gameState->state() != KDiamond::Finished)
		actionCollection()->action("game_pause")->setChecked(true);
	//show dialog
	QPointer<KScoreDialog> dialog = new KScoreDialog(KScoreDialog::Name | KScoreDialog::Score, this);
	dialog->addField(KScoreDialog::Custom1, i18n("Mode"), "mode");
	dialog->initFromDifficulty(Kg::difficulty());
	dialog->exec();
	delete dialog;
}

void MainWindow::pausedAction(bool paused)
{
	m_gameState->setState(paused ? KDiamond::Paused : KDiamond::Playing);
}

void MainWindow::configureNotifications()
{
	KNotifyConfigWidget::configure(this);
}

#include "mainwindow.moc"
