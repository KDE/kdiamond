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

#ifndef KDIAMOND_MAINWINDOW_H
#define KDIAMOND_MAINWINDOW_H

#ifndef KDIAMOND_CONTAINER_H
    class Container;
#endif
#ifndef KDIAMOND_GAME_H
    class Game;
#endif

class QTime;
class QTimer;
#include <KXmlGuiWindow>

namespace KDiamond
{
    //maximum update interval in milliseconds
    const int UpdateInterval = 10;
};

class MainWindow : public KXmlGuiWindow
{
    Q_OBJECT
    public:
        MainWindow(QWidget *parent = 0);
        ~MainWindow();
    public slots:
        void startGame();
        void finishGame(int points);
        void showHighscores();
        void close();
        void configureSettings();
        void loadSettings();
    signals:
        void pause(bool paused);
        void updateScheduled(int milliseconds);
    protected:
        virtual void closeEvent(QCloseEvent *);
    protected slots:
        void updateTime();
        void updatePoints(int points);
        void updateRemainingTime(int remainingSeconds);
    private:
        Game *m_game;
        Container *m_container;

        QTime *m_updateTime;
        QTimer *m_updateTimer;
};

#endif //KDIAMOND_MAINWINDOW_H
