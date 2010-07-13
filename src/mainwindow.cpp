/***************************************************************************
 *   Copyright 2008-2009 Stefan Majewsky <majewsky@gmx.net>
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
#include "game-state.h"
#include "infobar.h"
#include "renderer.h"
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
	, m_infoBar(0)
	, m_newAct(new KActionMenu(KIcon("document-new"), i18nc("new game", "&New"), this))
	, m_newTimedAct(new KAction(i18n("Timed game"), this))
	, m_newUntimedAct(new KAction(i18n("Untimed game"), this))
{
	//init GUI - "New Action"
	m_newAct->setShortcut(KStandardShortcut::openNew());
	m_newAct->setToolTip(i18n("Start a new game"));
	m_newAct->setWhatsThis(i18n("Start a new game."));
	actionCollection()->addAction("game_new", m_newAct);
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
	KStandardAction::preferences(this, SLOT(configureSettings()), actionCollection());
	KStandardAction::configureNotifications(this, SLOT(configureNotifications()), actionCollection());
	//difficulty
	KGameDifficulty::init(this, this, SLOT(startGameDispatcher()));
	KGameDifficulty::addStandardLevel(KGameDifficulty::VeryEasy);
	KGameDifficulty::addStandardLevel(KGameDifficulty::Easy);
	KGameDifficulty::addStandardLevel(KGameDifficulty::Medium);
	KGameDifficulty::addStandardLevel(KGameDifficulty::Hard);
	KGameDifficulty::addStandardLevel(KGameDifficulty::VeryHard);
	//late GUI initialisation
	setupGUI(QSize(300, 400)); //TODO: find better solution for a minimum size
	setCaption(i18nc("The application's name", "KDiamond"));
	setCentralWidget(m_view);
	connect(m_view, SIGNAL(resized()), this, SLOT(updateTheme()));
	//init statusbar
	m_infoBar = new KDiamond::InfoBar(statusBar());
	connect(m_game, SIGNAL(stateChanged(KDiamond::State)), this, SLOT(stateChange(KDiamond::State)));
	connect(m_game, SIGNAL(pointsChanged(int)), m_infoBar, SLOT(updatePoints(int)));
	connect(m_game, SIGNAL(leftTimeChanged(int)), m_infoBar, SLOT(updateRemainingTime(int)));
	//selected skill
	int skill = Settings::skill();
	if (skill < KGameDifficulty::VeryEasy) //includes default values like 0 or 1
		KGameDifficulty::setLevel(KGameDifficulty::Easy);
	else
		KGameDifficulty::setLevel((KGameDifficulty::standardLevel) (skill));
}

MainWindow::~MainWindow()
{
	Settings::self()->writeConfig();
	delete m_board;
	delete m_game;
}

void MainWindow::startGameDispatcher()
{
	if (sender() == m_newUntimedAct)
		startGame(KDiamond::UntimedGame);
	else if (sender() == m_newTimedAct)
		startGame(KDiamond::NormalGame);
	else
		//attention: this may also be used by KGameDifficulty
		startGame(Settings::untimed() ? KDiamond::UntimedGame : KDiamond::NormalGame);
}

void MainWindow::startGame(KDiamond::Mode mode)
{
	//save (eventually changed) difficulty level
	KGameDifficulty::standardLevel level = KGameDifficulty::level();
	Settings::setSkill((int) level);
	//delete old board
	delete m_board;
	//start new game
	m_game->startNewGame();
	m_game->setMode(mode);
	m_board = new Board(m_game, KGameDifficulty::level());
	updateTheme(false); //initializes the theme
	connect(m_game, SIGNAL(stateChanged(KDiamond::State)), m_board, SLOT(stateChange(KDiamond::State)));
	connect(m_game, SIGNAL(message(const QString&)), m_board, SLOT(message(const QString&)));
	connect(m_board, SIGNAL(numberMoves(int)), m_infoBar, SLOT(updateMoves(int)));
	connect(m_board, SIGNAL(pendingAnimationsFinished()), this, SLOT(gameIsOver()));
	connect(m_hintAct, SIGNAL(triggered()), m_board, SLOT(showHint()));
	m_view->setScene(m_board);
	m_view->fitInView(0, 0, m_view->width(), m_view->height());
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
	scoreInfo[KScoreDialog::Score].setNum(m_game->points());
	scoreInfo[KScoreDialog::Custom1] = m_game->mode() == KDiamond::UntimedGame ? i18n("Untimed") : i18n("Timed");
	//report score
	QPointer<KScoreDialog> dialog = new KScoreDialog(KScoreDialog::Name | KScoreDialog::Score, this);
	dialog->addLocalizedConfigGroupNames(KGameDifficulty::localizedLevelStrings());
	dialog->setConfigGroupWeights(KGameDifficulty::levelWeights());
	dialog->addField(KScoreDialog::Custom1, i18n("Mode"), "mode");
	dialog->setConfigGroup(KGameDifficulty::localizedLevelString());
	dialog->addScore(scoreInfo);
	dialog->exec();
	delete dialog;
}

void MainWindow::showHighscores()
{
	//pause game if necessary
	m_game->setState(KDiamond::Paused);
	if (m_game->state() != KDiamond::Finished)
		actionCollection()->action("game_pause")->setChecked(true);
	//show dialog
	QPointer<KScoreDialog> dialog = new KScoreDialog(KScoreDialog::Name | KScoreDialog::Score, this);
	dialog->addLocalizedConfigGroupNames(KGameDifficulty::localizedLevelStrings());
	dialog->setConfigGroupWeights(KGameDifficulty::levelWeights());
	dialog->addField(KScoreDialog::Custom1, i18n("Mode"), "mode");
	dialog->exec();
	delete dialog;
}

void MainWindow::pausedAction(bool paused)
{
	m_game->setState(paused ? KDiamond::Paused : KDiamond::Playing);
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
	Renderer::self()->loadTheme(Settings::theme());
	//redraw game scene if necessary
	if (m_game != 0)
	{
		updateTheme(true);
		m_board->invalidate(m_board->sceneRect(), QGraphicsScene::BackgroundLayer);
	}
}

#include "mainwindow.moc"
