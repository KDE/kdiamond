/*
    SPDX-FileCopyrightText: 2008-2010 Stefan Majewsky <majewsky@gmx.net>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "mainwindow.h"
#include "game.h"
#include "infobar.h"
#include "settings.h"
#include "view.h"
// KDEGames
#include <KGameDifficulty>
#include <KGameGraphicsViewRenderer>
#include <KGameHighScoreDialog>
#include <KGameStandardAction>
// KF
#include <KActionCollection>
#include <KActionMenu>
#include <KLocalizedString>
#include <KNotifyConfigWidget>
#include <KStandardAction>
// Qt
#include <QAction>
#include <QPointer>

namespace KDiamond
{
class ThemeProvider : public KGameThemeProvider
{
public:
    explicit ThemeProvider(QObject *parent = nullptr)
        : KGameThemeProvider("Theme", parent)
    {
        discoverThemes(QStringLiteral("themes"));
    }
};

class Renderer : public KGameGraphicsViewRenderer
{
public:
    Renderer() : KGameGraphicsViewRenderer(new ThemeProvider, 10)
    {
        setFrameSuffix(QStringLiteral("-%1"));
    }
};
}

MainWindow::MainWindow(QWidget *parent)
    : KXmlGuiWindow(parent)
    , m_gameState(new KDiamond::GameState)
    , m_game(nullptr)
    , m_view(new KDiamond::View)
    , m_infoBar(nullptr)
    , m_newAct(new KActionMenu(QIcon::fromTheme(QStringLiteral("document-new")), i18nc("@title:menu new game", "&New"), this))
    , m_newTimedAct(new QAction(i18nc("@item:inmenu", "Timed Game"), this))
    , m_newUntimedAct(new QAction(i18nc("@item:inmenu", "Untimed Game"), this))
    , m_renderer(new KDiamond::Renderer())
    , m_selector(m_renderer->themeProvider(), KGameThemeSelector::EnableNewStuffDownload)
{
    m_renderer->setDefaultPrimaryView(m_view);
    //init GUI - "New Action"
    m_newAct->setToolTip(i18nc("@info:tooltip", "Start a new game"));
    m_newAct->setWhatsThis(i18nc("@info:whatsthis", "Starts a new game."));
    actionCollection()->addAction(QStringLiteral("game_new"), m_newAct);
    KActionCollection::setDefaultShortcuts(m_newAct, KStandardShortcut::openNew());
    connect(m_newAct, &KActionMenu::triggered, this, &MainWindow::startGameDispatcher);
    m_newAct->addAction(m_newTimedAct);
    connect(m_newTimedAct, &QAction::triggered, this, &MainWindow::startGameDispatcher);
    m_newAct->addAction(m_newUntimedAct);
    connect(m_newUntimedAct, &QAction::triggered, this, &MainWindow::startGameDispatcher);
    //init GUI - the other actions
    KGameStandardAction::highscores(this, &MainWindow::showHighscores, actionCollection());
    m_pauseAct = KGameStandardAction::pause(this, &MainWindow::pausedAction, actionCollection());
    KGameStandardAction::quit(this, &QWidget::close, actionCollection());
    m_hintAct = KGameStandardAction::hint(nullptr, nullptr, actionCollection());
    KStandardAction::preferences(this, &MainWindow::showPreferences, actionCollection());
    KStandardAction::configureNotifications(this, &MainWindow::configureNotifications, actionCollection());
    //difficulty
    KGameDifficultyGUI::init(this);
    connect(KGameDifficulty::global(), &KGameDifficulty::currentLevelChanged, this, &MainWindow::startGameDispatcher);
    //late GUI initialisation
    setupGUI(QSize(300, 400)); //TODO: find better solution for a minimum size
    setCentralWidget(m_view);
    //init statusbar
    m_infoBar = new KDiamond::InfoBar(statusBar());
    connect(m_gameState, &KDiamond::GameState::stateChanged, this, &MainWindow::stateChange);
    connect(m_gameState, &KDiamond::GameState::pointsChanged, m_infoBar, &KDiamond::InfoBar::updatePoints);
    connect(m_gameState, &KDiamond::GameState::leftTimeChanged, m_infoBar, &KDiamond::InfoBar::updateRemainingTime);
    //init game
    startGameDispatcher();
}

MainWindow::~MainWindow()
{
    Settings::self()->save();
    delete m_renderer;
    delete m_game;
    delete m_gameState;
}

void MainWindow::startGameDispatcher()
{
    if (sender() == m_newUntimedAct) {
        startGame(KDiamond::UntimedGame);
    } else if (sender() == m_newTimedAct) {
        startGame(KDiamond::NormalGame);
    } else
        //attention: this may also be used by KGameDifficulty and the ctor
    {
        startGame(Settings::untimed() ? KDiamond::UntimedGame : KDiamond::NormalGame);
    }
}

void MainWindow::startGame(KDiamond::Mode mode)
{
    //delete old board
    delete m_game;
    //start new game
    m_gameState->startNewGame();
    m_gameState->setMode(mode);
    m_game = new Game(m_gameState, m_renderer);
    connect(m_gameState, &KDiamond::GameState::stateChanged, m_game, &Game::stateChange);
    connect(m_gameState, &KDiamond::GameState::message, m_game, &Game::message);
    connect(m_game, &Game::numberMoves, m_infoBar, &KDiamond::InfoBar::updateMoves);
    connect(m_game, &Game::pendingAnimationsFinished, this, &MainWindow::gameIsOver);
    connect(m_hintAct, &QAction::triggered, m_game, &Game::showHint);
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
    KGameHighScoreDialog::FieldInfo scoreInfo;
    scoreInfo[KGameHighScoreDialog::Score].setNum(m_gameState->points());
    scoreInfo[KGameHighScoreDialog::Custom1] = m_gameState->mode() == KDiamond::UntimedGame ? i18n("Untimed") : i18n("Timed");
    //report score
    QPointer<KGameHighScoreDialog> dialog = new KGameHighScoreDialog(KGameHighScoreDialog::Name | KGameHighScoreDialog::Score, this);
    dialog->addField(KGameHighScoreDialog::Custom1, i18n("Mode"), QStringLiteral("mode"));
    dialog->initFromDifficulty(KGameDifficulty::global());
    dialog->addScore(scoreInfo);
    dialog->exec();
    delete dialog;
}

void MainWindow::showHighscores()
{
    //pause game if necessary
    m_gameState->setState(KDiamond::Paused);
    if (m_gameState->state() != KDiamond::Finished) {
        actionCollection()->action(QStringLiteral("game_pause"))->setChecked(true);
    }
    //show dialog
    QPointer<KGameHighScoreDialog> dialog = new KGameHighScoreDialog(KGameHighScoreDialog::Name | KGameHighScoreDialog::Score, this);
    dialog->addField(KGameHighScoreDialog::Custom1, i18n("Mode"), QStringLiteral("mode"));
    dialog->initFromDifficulty(KGameDifficulty::global());
    dialog->exec();
    delete dialog;
}

void MainWindow::showPreferences()
{
    m_selector.showAsDialog();
}


void MainWindow::pausedAction(bool paused)
{
    m_gameState->setState(paused ? KDiamond::Paused : KDiamond::Playing);
}

void MainWindow::configureNotifications()
{
    KNotifyConfigWidget::configure(this);
}

#include "moc_mainwindow.cpp"
