/*
    SPDX-FileCopyrightText: 2008-2009 Stefan Majewsky <majewsky@gmx.net>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KDIAMOND_GAMESTATE_H
#define KDIAMOND_GAMESTATE_H

#include <QObject>

namespace KDiamond
{

class GameStatePrivate;

//base duration of a game in seconds
const int GameDuration = 200;

enum Mode {
    NormalGame,
    UntimedGame
};
enum State {
    Playing,
    Paused,
    Finished
};

class GameState : public QObject
{
    Q_OBJECT
public:
    GameState();
    ~GameState() override;

    Mode mode() const;
    State state() const;
    int leftTime() const;
    int points() const;

    void setMode(Mode mode);
    void setState(State state);
public Q_SLOTS:
    void addPoints(int removedDiamonds);
    void removePoints(int points);
    void resetCascadeCounter();
    void startNewGame();
    void update(bool forceRecalculation = false);
Q_SIGNALS:
    void message(const QString &text); //text == QString() means: hide the message popup
    void stateChanged(KDiamond::State state); //warning: moc needs the full identifier of KDiamond::State
    void pointsChanged(int points);
    void leftTimeChanged(int seconds);
protected:
    void timerEvent(QTimerEvent *event) override;
private:
    GameStatePrivate *p;
};

}

#endif // KDIAMOND_GAMESTATE_H
