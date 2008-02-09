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
    class QGraphicsSvgItem;
#endif
#ifndef KDIAMOND_GAME_H
    class Game;
#endif

#include <QGraphicsScene>
#include <QList>
#include <QSet>
#include <KGameDifficulty>
class KGamePopupItem;

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

class Board : public QGraphicsScene
{
    Q_OBJECT
    public:
        Board(KGameDifficulty::standardLevel difficulty);
        ~Board();
        int diamondCountOnEdge() const;

        void mouseOnDiamond(int xIndex, int yIndex);
    public slots:
        void pause(bool paused);
        void showMessage(const QString &message);
        void update(int milliseconds);
    signals:
        void diamondsRemoved(int count, int cascade);
        void updateScheduled(int milliseconds);
        void animationInProgress(); //see Diamond::move(const QPointF &) for details
    private:
        QSet<QPoint *> findCompletedRows();
        void fillGaps();
    private:
        KDiamond::Size m_size;
        KDiamond::ColorCount m_colorCount;
        QList<KDiamond::Job> m_jobQueue;

        Diamond ***m_diamonds;
        QGraphicsSvgItem *m_selection1, *m_selection2;
        KGamePopupItem *m_messenger;

        int m_selected1x, m_selected1y, m_selected2x, m_selected2y; //coordinates of the selected items (or -1 if they are not selected)
        int m_swapping1x, m_swapping1y, m_swapping2x, m_swapping2y; //coordinates of the swapping/swapped items (stored to revoke the swapping if necessary)
        bool m_paused;
        int m_cascade; //cascade count (necessary for score calculation in Game)
};

#endif //KDIAMOND_BOARD_H
