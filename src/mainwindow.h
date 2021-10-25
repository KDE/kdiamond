/*
    SPDX-FileCopyrightText: 2008-2010 Stefan Majewsky <majewsky@gmx.net>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KDIAMOND_MAINWINDOW_H
#define KDIAMOND_MAINWINDOW_H

class Game;
#include "game-state.h"

class QAction;
class KActionMenu;
class KGameRenderer;

#include <KXmlGuiWindow>
#include <KgThemeSelector>

namespace KDiamond
{
class InfoBar;
class View;
}

class MainWindow : public KXmlGuiWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
public Q_SLOTS:
    void startGame(KDiamond::Mode mode);
    void startGameDispatcher();
    void stateChange(KDiamond::State state);
    void gameIsOver();
    void showHighscores();
    void showPreferences();

    void configureNotifications();
Q_SIGNALS:
    void pause(bool paused);
protected Q_SLOTS:
    void pausedAction(bool paused);
private:
    KDiamond::GameState *m_gameState;
    Game *m_game;
    KDiamond::View *m_view;

    KDiamond::InfoBar *m_infoBar;

    KActionMenu *m_newAct;
    QAction *m_newTimedAct;
    QAction *m_newUntimedAct;
    QAction *m_pauseAct;
    QAction *m_hintAct;
    KGameRenderer *m_renderer;
    KgThemeSelector m_selector;
};

#endif //KDIAMOND_MAINWINDOW_H
