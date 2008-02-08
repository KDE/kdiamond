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

#ifndef KDIAMOND_BOARD_H
#define KDIAMOND_BOARD_H

#ifndef KDIAMOND_DIAMOND_H
    class Diamond;
#endif
#ifndef KDIAMOND_GAME_H
    class Game;
#endif

#include <QList>
#include <QSet>
#include <QWidget>
#include <KGameDifficulty>

namespace KDiamond
{
    //the width and height in pixels after which the background of the board is repeated
    const int BackgroundTileSize = 200;
    //specification of the difficulties
    enum Size
    {
        VeryEasySize = 12,
        EasySize = 10,
        MediumSize = 8,
        HardSize = 8,
        VeryHardSize = 8
    };
    enum ColorCount
    {
        VeryEasyColors = 5,
        EasyColors = 5,
        MediumColors = 5,
        HardColors = 6,
        VeryHardColors = 7
    };
    //jobs to be done during the board update
    enum Job {
        SwapDiamondsJob = 1, //swap selected diamonds
        RemoveRowsJob, //remove complete rows of diamonds and add points
        RevokeSwapDiamondsJob //revoke swapping of diamonds (will be requested by the RemoveRowsJob if no rows have been formed)
    };
};

class Board : public QWidget
{
    Q_OBJECT
    public:
        Board(KGameDifficulty::standardLevel difficulty, Game *game, QWidget *parent = 0);
        ~Board();

        void selectDiamond(int xIndex, int yIndex);
        void unselectDiamond(int xIndex, int yIndex);
        void updateDiamond(int xIndex, int yIndex);
    public slots:
        void pause(bool paused);
        void update(int milliseconds);
    signals:
        void diamondsRemoved(int count, int cascade);
        void updateScheduled(int milliseconds);
    protected:
        virtual void paintEvent(QPaintEvent *);
        virtual void resizeEvent(QResizeEvent *);
    private:
        QSet<QPoint *> findCompletedRows();
        void fillGaps();
    private:
        KDiamond::Size m_size;
        KDiamond::ColorCount m_colorCount;

        Diamond ***m_diamonds;
        QList<KDiamond::Job> m_jobQueue;
        qreal m_topOffset, m_leftOffset, m_diamondEdgeLength;

        int m_selected1x, m_selected1y, m_selected2x, m_selected2y;
        int m_swapping1x, m_swapping1y, m_swapping2x, m_swapping2y;
        bool m_paused;
        int m_cascade;
};

#endif //KDIAMOND_BOARD_H
