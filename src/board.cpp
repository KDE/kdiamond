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

#include "board.h"
#include "diamond.h"
#include "game.h"
#include "renderer.h"

Board::Board(KGameDifficulty::standardLevel difficulty, Game */*game*/, QWidget *parent)
    : QWidget(parent)
{
    switch (difficulty)
    {
        case KGameDifficulty::VeryEasy:
            m_size = KDiamond::VeryEasySize;
            m_colorCount = KDiamond::VeryEasyColors;
            break;
        case KGameDifficulty::Easy:
            m_size = KDiamond::EasySize;
            m_colorCount = KDiamond::EasyColors;
            break;
        case KGameDifficulty::Medium:
            m_size = KDiamond::MediumSize;
            m_colorCount = KDiamond::MediumColors;
            break;
        case KGameDifficulty::Hard:
            m_size = KDiamond::HardSize;
            m_colorCount = KDiamond::HardColors;
            break;
        case KGameDifficulty::VeryHard:
        default: //punish bugs with "Very hard" mode ;-)
            m_size = KDiamond::VeryHardSize;
            m_colorCount = KDiamond::VeryHardColors;
            break;
    }
    //fill diamond field
    m_diamonds = new Diamond**[m_size];
    for (int x = 0; x < m_size; ++x)
    {
        m_diamonds[x] = new Diamond*[m_size];
        for (int y = 0; y < m_size; ++y)
        {
            //roll the dice to get a color, but ensure that there are not three of a color in a row from the start
            int color, otherColor1, otherColor2;
            while (true)
            {
                color = qrand() % m_colorCount + 1; // +1 because numbering of enum KDiamond::Color starts at 1
                //condition: no triplet in y axis (attention: only the diamonds above us are defined already)
                if (y >= 2) //no triplet possible for i = 0, 1
                {
                    otherColor1 = m_diamonds[x][y - 1]->color();
                    otherColor2 = m_diamonds[x][y - 2]->color();
                    if (otherColor1 == color && otherColor2 == color)
                        continue; //roll the dice again
                }
                //same condition on x axis
                if (x >= 2)
                {
                    otherColor1 = m_diamonds[x - 1][y]->color();
                    otherColor2 = m_diamonds[x - 2][y]->color();
                    if (otherColor1 == color && otherColor2 == color)
                        continue;
                }
                break;
            }
            //set diamond
            m_diamonds[x][y] = new Diamond(x, y, x, y, KDiamond::colorFromNumber(color), this);
            m_diamonds[x][y]->show();
        }
    }
    //init GUI and internal values (any metrical values are calc'ed in the first resizeEvent())
    m_selected1x = m_selected1y = m_selected2x = m_selected2y = -1;
    m_swapping1x = m_swapping1y = m_swapping2x = m_swapping2y = -1;
    m_paused = false;
    show();
}

Board::~Board()
{
    for (int i = 0; i < m_size; ++i)
    {
        for (int j = 0; j < m_size; ++j)
        {
            delete m_diamonds[i][j];
        }
        delete[] m_diamonds[i];
    }
    delete[] m_diamonds;
}

void Board::selectDiamond(int xIndex, int yIndex)
{
    if (m_selected1x == -1 || m_selected1y == -1)
    {
        m_selected1x = xIndex;
        m_selected1y = yIndex;
    }
    else if (m_selected2x == -1 || m_selected2y == -1)
    {
        //ensure that the second one is a neighbor of the other one
        int dx = qAbs(m_selected1x - xIndex), dy = qAbs(m_selected1y - yIndex);
        if ((dx == 1 && dy == 0) || (dx == 0 && dy == 1))
        {
            m_selected2x = xIndex;
            m_selected2y = yIndex;
            m_jobQueue << KDiamond::SwapDiamondsJob;
        }
        else
        {
            //selected a diamond that it is not a neighbor - move the first selection to this diamond
            m_diamonds[m_selected1x][m_selected1y]->select(false);
            m_selected1x = xIndex;
            m_selected1y = yIndex;
        }
    }
}

void Board::unselectDiamond(int xIndex, int yIndex)
{
    if (m_selected1x == xIndex && m_selected1y == yIndex)
    {
        m_selected1x = m_selected2x;
        m_selected1y = m_selected2y;
        m_selected2x = -1;
        m_selected2y = -1;
    }
    else if (m_selected2x == xIndex && m_selected2y == yIndex)
    {
        m_selected2x = -1;
        m_selected2y = -1;
    }
}

void Board::pause(bool paused)
{
    m_paused = paused;
    for (int x = 0; x < m_size; ++x)
    {
        for (int y = 0; y < m_size; ++y)
            m_diamonds[x][y]->setVisible(!paused);
    }
}

void Board::update(int milliseconds)
{
    if (m_paused)
        return;
    if (receivers(SIGNAL(updateScheduled(int))) > 0)
        emit updateScheduled(milliseconds);
    else if (m_jobQueue.count() != 0)
    {
        KDiamond::Job job = m_jobQueue.takeFirst();
        Diamond *temp;
        QSet<QPoint *> removeTheseDiamonds;
        switch (job)
        {
            case KDiamond::SwapDiamondsJob:
                m_cascade = 0; //starting a new cascade
                //copy selection info into another storage (to allow the user to select the next two diamonds while the cascade runs)
                m_swapping1x = m_selected1x;
                m_swapping1y = m_selected1y;
                m_swapping2x = m_selected2x;
                m_swapping2y = m_selected2y;
                m_selected1x = m_selected1y = m_selected2x = m_selected2y = -1;
                m_jobQueue << KDiamond::RemoveRowsJob; //We already insert this here to avoid another conditional statement.
            case KDiamond::RevokeSwapDiamondsJob: //It is not an error that there is no break statement before this case: The swapping code is essentially the same, but the swap job has to do the above variable initializations etc.
                //swap diamonds
                temp = m_diamonds[m_swapping1x][m_swapping1y];
                m_diamonds[m_swapping1x][m_swapping1y] = m_diamonds[m_swapping2x][m_swapping2y];
                m_diamonds[m_swapping2x][m_swapping2y] = temp;
                m_diamonds[m_swapping1x][m_swapping1y]->setXIndex(m_swapping1x);
                m_diamonds[m_swapping1x][m_swapping1y]->setYIndex(m_swapping1y);
                m_diamonds[m_swapping2x][m_swapping2y]->setXIndex(m_swapping2x);
                m_diamonds[m_swapping2x][m_swapping2y]->setYIndex(m_swapping2y);
                //invoke movement and unselect
                m_diamonds[m_swapping1x][m_swapping1y]->move(m_swapping1x, m_swapping1y);
                m_diamonds[m_swapping2x][m_swapping2y]->move(m_swapping2x, m_swapping2y);
                m_diamonds[m_swapping1x][m_swapping1y]->select(false);
                m_diamonds[m_swapping2x][m_swapping2y]->select(false);
                break;
            case KDiamond::RemoveRowsJob:
                m_cascade++;
                //find diamond rows and delete these diamonds
                removeTheseDiamonds = findCompletedRows();
                if (removeTheseDiamonds.count() == 0)
                {
                    //no diamond rows were formed by the last move -> revoke movement (unless we are in a cascade)
                    if (m_swapping1x != -1 && m_swapping1y != -1 && m_swapping2x != -1 && m_swapping2y != -1)
                        m_jobQueue << KDiamond::RevokeSwapDiamondsJob;
                    break;
                }
                else
                    //report to Game
                    emit diamondsRemoved(removeTheseDiamonds.count(), m_cascade);
                //delete diamonds
                foreach (QPoint *diamondPos, removeTheseDiamonds)
                {
                    delete m_diamonds[diamondPos->x()][diamondPos->y()];
                    m_diamonds[diamondPos->x()][diamondPos->y()] = 0;
                }
                //cleanup pointers
                foreach (QPoint *toDelete, removeTheseDiamonds)
                    delete toDelete;
                //fill gaps
                fillGaps();
                //it is now safe to delete the position of the swapping diamonds
                m_swapping1x = m_swapping1y = m_swapping2x = m_swapping2y = -1;
                m_jobQueue.prepend(KDiamond::RemoveRowsJob); //allow cascades (i.e. clear rows that have been formed by falling diamonds); prepend this job as it has to be executed immediately after the animations (before handling any further user input)
                break;
        }
    }
}

QSet<QPoint *> Board::findCompletedRows()
{
    //The tactic of this function is brute-force. For now, I do not have a better idea: A friend of mine advised me to look in the environment of moved diamonds, but this is not easy since the re-filling after a deletion can lead to rows that are far away from the original movement. Therefore, we simply search through all diamonds looking for combinations in the horizonal and vertical direction. The created QList is returned as a QSet to remove duplicates. (It is not necessary where the rows exactly are. Only the number of diamonds and their position is relevant for scoring and removing these diamonds.) (We do not directly build a QSet as QList allows for faster insertions. At the end, the QSet can easily be created with the right capacity.)
    KDiamond::Color currentColor;
    QList<QPoint *> diamonds; //use a QList as storage as it allows for faster insertion than a QSet
    int x, y, xh, yh; //counters
    //searching in horizontal direction
    for (y = 0; y < m_size; ++y)
    {
        for (x = 0; x < m_size - 2; ++x) //counter stops at m_size - 2 to ensure availability of indices x + 1, x + 2
        {
            currentColor = m_diamonds[x][y]->color();
            if (currentColor != m_diamonds[x + 1][y]->color())
                continue;
            if (currentColor != m_diamonds[x + 2][y]->color())
                continue;
            //If the execution is here, we have found a row of three diamonds starting at (x,y).
            diamonds << new QPoint(x, y);
            diamonds << new QPoint(x + 1, y);
            diamonds << new QPoint(x + 2, y);
            //Does the row have even more elements?
            if (x + 3 >= m_size)
            {
                //impossible to locate more diamonds - do not go through the following loop
                x += 2;
                continue;
            }
            for (xh = x + 3; xh <= m_size - 1; ++xh)
            {
                if (currentColor == m_diamonds[xh][y]->color())
                    diamonds << new QPoint(xh, y);
                else
                    break; //row has stopped before this diamond - no need to continue searching
            }
            x = xh - 1; //do not search at this position in the row anymore (add -1 because x is incremented before the next loop)
        }
    }
    //searching in vertical direction (essentially the same algorithm, just with swapped indices -> no comments here, read the comments above)
    for (x = 0; x < m_size; ++x)
    {
        for (y = 0; y < m_size - 2; ++y)
        {
            currentColor = m_diamonds[x][y]->color();
            if (currentColor != m_diamonds[x][y + 1]->color())
                continue;
            if (currentColor != m_diamonds[x][y + 2]->color())
                continue;
            diamonds << new QPoint(x, y);
            diamonds << new QPoint(x, y + 1);
            diamonds << new QPoint(x, y + 2);
            if (y + 3 >= m_size)
            {
                y += 2;
                continue;
            }
            for (yh = y + 3; yh <= m_size - 1; ++yh)
            {
                if (currentColor == m_diamonds[x][yh]->color())
                    diamonds << new QPoint(x, yh);
                else
                    break;
            }
        }
    }
    return diamonds.toSet();
}

void Board::fillGaps()
{
    //fill gaps
    int x, y, yt; //counters - (x, yt) is the target position of diamond (x,y)
    for (x = 0; x < m_size; ++x)
    {
        //We have to search from the bottom of the column. Exclude the lowest element (x = m_size - 1) because it cannot move down.
        for (y = m_size - 2; y >= 0; --y)
        {
            if (m_diamonds[x][y] == 0)
                //no need to move gaps -> these are moved later
                continue;
            if (m_diamonds[x][y + 1] != 0)
                //there is something right below this diamond -> Do not move.
                continue;
            //search for the lowest possible position
            for (yt = y; yt < m_size - 1; ++yt)
            {
                if (m_diamonds[x][yt + 1] != 0)
                    break; //xt now holds the lowest possible position
            }
            //continue;
            m_diamonds[x][yt] = m_diamonds[x][y];
            m_diamonds[x][y] = 0;
            m_diamonds[x][yt]->setYIndex(yt);
            m_diamonds[x][yt]->move(x, yt);
        }
    }
    //fill top rows with new elements
    for (x = 0; x < m_size; ++x)
    {
        yt = 0; //now: holds the position from where the diamond comes (-1 for the lowest new diamond)
        for (y = m_size - 1; y >= 0; --y)
        {
            if (m_diamonds[x][y] != 0)
                continue; //inside of diamond stack - no gaps to fill
            --yt;
            m_diamonds[x][y] = new Diamond(x, y, x, yt, KDiamond::colorFromNumber(qrand() % m_colorCount + 1), this);
            updateDiamond(x, y);
            m_diamonds[x][y]->move(x, y);
            m_diamonds[x][y]->show();
        }
    }
}

void Board::paintEvent(QPaintEvent *)
{
    //draw background tiles
    QPainter painter(this);
    for (int x = 0; x < width(); x += KDiamond::BackgroundTileSize)
    {
        for (int y = 0; y < height(); y += KDiamond::BackgroundTileSize)
            Renderer::drawBackground(&painter, QRectF(x, y, KDiamond::BackgroundTileSize, KDiamond::BackgroundTileSize));
    }
}

void Board::resizeEvent(QResizeEvent *)
{
    //calculate new field position
    qreal fieldWidth = width();
    qreal fieldHeight = height();
    qreal quadraticFieldSize = qMin(fieldHeight, fieldWidth);
    m_leftOffset = (fieldWidth - quadraticFieldSize) / 2.0; //factor 2 because we need space on left _and_ right side to center the diamonds
    m_topOffset = (fieldHeight - quadraticFieldSize) / 2.0;
    m_diamondEdgeLength = quadraticFieldSize / (qreal) m_size;
    //adjust geometry of diamonds
    for (int x = 0; x < m_size; ++x)
    {
        for (int y = 0; y < m_size; ++y)
        {
            m_diamonds[x][y]->setGeometry(
                m_leftOffset + m_diamonds[x][y]->xPos() * m_diamondEdgeLength,
                m_topOffset + m_diamonds[x][y]->yPos() * m_diamondEdgeLength,
                m_diamondEdgeLength, m_diamondEdgeLength
            );
        }
    }
}

void Board::updateDiamond(int xIndex, int yIndex)
{
    m_diamonds[xIndex][yIndex]->setGeometry(
        m_leftOffset + m_diamonds[xIndex][yIndex]->xPos() * m_diamondEdgeLength,
        m_topOffset + m_diamonds[xIndex][yIndex]->yPos() * m_diamondEdgeLength,
        m_diamondEdgeLength, m_diamondEdgeLength
    );
}

#include "board.moc"
