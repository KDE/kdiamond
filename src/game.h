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

#ifndef KDIAMOND_GAME_H
#define KDIAMOND_GAME_H

#ifndef KDIAMOND_BOARD_H
    class Board;
#endif
#ifndef KDIAMOND_MAINWINDOW_H
    class MainWindow;
#endif

#include <QGraphicsView>
class QTime;
#include <KGameDifficulty>

namespace KDiamond
{
    //base duration of a game in seconds
    const int GameDuration = 120;
};

class Game : public QGraphicsView
{
    Q_OBJECT
    public:
        Game(KGameDifficulty::standardLevel difficulty, MainWindow *mainWindow);
        ~Game();

        Board *board();
    public slots:
        void pause(bool paused);
        void update(int milliseconds);
    signals:
        void pointsChanged(int points);
        void remainingTimeChanged(int remainingTime);
        void gameFinished(int points);
    protected:
        virtual void drawBackground(QPainter *painter, const QRectF &rect);
        virtual void mouseReleaseEvent(QMouseEvent *);
        virtual void resizeEvent(QResizeEvent *);
        virtual void wheelEvent(QWheelEvent *event);
    protected slots:
        void diamondsRemoved(int count, int cascade);
    private:
        Board *m_board;
        MainWindow *m_mainWindow;
        QTime *m_gameTime, *m_pauseTime;

        int m_points;
        int m_secondsEarned, m_secondsPaused, m_secondsRemaining;
        bool m_paused, m_finished;
};

#endif //KDIAMOND_GAME_H
