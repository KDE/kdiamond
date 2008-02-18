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

#include <KGamePopupItem>
#include <KLocalizedString>
#include <KNotification>

Board::Board(KGameDifficulty::standardLevel difficulty)
    : QGraphicsScene()
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
    //init scene (with some default scene size that makes board coordinates equal scene coordinates)
    setSceneRect(0.0, 0.0, m_size, m_size);
    m_diamondEdgeLength = 1.0;
    m_topOffset = m_leftOffset = 0.0;
    //fill diamond field and scene
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
        }
    }
    //init selection markers
    m_selection1 = new Diamond(0, 0, 0, 0, KDiamond::Selection, this);
    m_selection1->hide();
    m_selection2 = new Diamond(0, 0, 0, 0, KDiamond::Selection, this);
    m_selection2->hide();
    //init background
    m_background = new QGraphicsPixmapItem(0, this);
    m_background->setZValue(1);
    m_background->setAcceptedMouseButtons(0);
    //init messengers
    m_messenger = new KGamePopupItem;
    m_messenger->setMessageOpacity(0.8);
    addItem(m_messenger);
    QRectF messengerRect = m_messenger->sceneBoundingRect();
    //init GUI and internal values (any metrical values are calc'ed in the first resizeEvent())
    m_selected1x = m_selected1y = m_selected2x = m_selected2y = -1;
    m_swapping1x = m_swapping1y = m_swapping2x = m_swapping2y = -1;
    m_paused = m_timeIsUp = false;
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
    delete m_selection1;
    delete m_selection2;
    delete m_background;
    delete m_messenger;
}

int Board::diamondCountOnEdge() const
{
    return m_size;
}

//Converts board coordinates (i.e. (0,0) is the top left point of the board, 1 unit = 1 diamond) to scene coordinates.
QPointF Board::boardToScene(const QPointF &boardCoords) const
{
    return QPointF(
        boardCoords.x() * m_diamondEdgeLength + m_leftOffset,
        boardCoords.y() * m_diamondEdgeLength + m_topOffset
    );
}

QPointF Board::sceneToBoard(const QPointF &sceneCoords) const
{
    return QPointF(
        (sceneCoords.x() - m_leftOffset) / m_diamondEdgeLength,
        (sceneCoords.y() - m_topOffset) / m_diamondEdgeLength
    );
}

qreal Board::diamondEdgeLength() const
{
    return m_diamondEdgeLength;
}

//Adapt scene coordinates to size of view. (This congruence is required by KGamePopupItem.)
void Board::resizeScene(qreal newWidth, qreal newHeight, bool force)
{
    //do not resize if nothing would change
    if (!force && width() == newWidth && height() == newHeight)
        return;
    setSceneRect(0.0, 0.0, newWidth, newHeight);
    //calculate new metrics - A board margin of half a diamond's size is hard-coded.
    m_diamondEdgeLength = qMin(newWidth, newHeight) / (m_size + 1);
    qreal boardSize = m_size * m_diamondEdgeLength;
    m_leftOffset = (newWidth - boardSize) / 2.0;
    m_topOffset = (newHeight - boardSize) / 2.0;
    //renderer
    Renderer::boardResized(newWidth, newHeight, m_leftOffset, m_topOffset, m_diamondEdgeLength, m_size);
    //diamonds
    emit boardResized(); //give diamonds the chance to change their metrics
    //background
    m_background->setPixmap(Renderer::background());
    QRectF bgRect = m_background->sceneBoundingRect();
    //The 10.0 and -5.0 are part of a nasty hack to avoid white borders around the background.
    m_background->scale((newWidth + 10.0) / bgRect.width(), (newHeight + 10.0) / bgRect.height());
    m_background->setPos(-5.0, -5.0);
    m_background->setVisible(true);
}

void Board::mouseOnDiamond(int xIndex, int yIndex)
{
    if (m_selected1x == xIndex && m_selected1y == yIndex)
    {
        //clicked again on first selected diamond - remove selection
        //Attention: This code is re-used in Board::update for the RemoveRowsJob. If you modify it here, please do also apply your changes over there.
        m_selected1x = m_selected2x;
        m_selected1y = m_selected2y;
        if (m_selected1x == -1 || m_selected1y == -1)
            m_selection1->hide();
        else
        {
            m_selection1->setPosInBoardCoords(QPointF(m_selected1x, m_selected1y));
            m_selection1->show();
        }
        m_selected2x = -1;
        m_selected2y = -1;
        m_selection2->hide();
    }
    else if (m_selected2x == xIndex && m_selected2y == yIndex)
    {
        //clicked again on second selected diamond - remove selection
        m_selected2x = -1;
        m_selected2y = -1;
        m_selection2->hide();
    }
    else if (m_selected1x == -1 || m_selected1y == -1)
    {
        //nothing selected - this is the first selected diamond
        m_selected1x = xIndex;
        m_selected1y = yIndex;
        m_selection1->setPosInBoardCoords(QPointF(xIndex, yIndex));
        m_selection1->show();
    }
    else if (m_selected2x == -1 || m_selected2y == -1)
    {
        //this could be the second selected diamond - ensure that the second one is a neighbor of the other one
        int dx = qAbs(m_selected1x - xIndex), dy = qAbs(m_selected1y - yIndex);
        if (dx + dy == 1)
        {
            m_selected2x = xIndex;
            m_selected2y = yIndex;
            m_selection2->setPosInBoardCoords(QPointF(xIndex, yIndex));
            m_selection2->show();
            m_jobQueue << KDiamond::SwapDiamondsJob;
        }
        else
        {
            //selected a diamond that it is not a neighbor - move the first selection to this diamond
            m_selected1x = xIndex;
            m_selected1y = yIndex;
            m_selection1->setPosInBoardCoords(QPointF(xIndex, yIndex));
            m_selection1->show();
        }
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
    if (paused)
    {
        m_selection1->hide();
        m_selection2->hide();
    }
    else
    {
        m_selection1->setVisible(m_selected1x != -1 && m_selected1y != -1);
        m_selection2->setVisible(m_selected2x != -1 && m_selected2y != -1);
    }
}

void Board::update()
{
    //see Diamond::move(const QPointF &) for explanation
    if (m_paused || Diamond::animationsInProgress() > 0)
        return;
    if(m_jobQueue.count() == 0) //nothing to do in this update
    {
        //finish game if possible
        if (m_timeIsUp)
            emit gameOver();
        return;
    }
    //execute first job in queue
    KDiamond::Job job = m_jobQueue.takeFirst();
    int dx, dy;
    Diamond *temp;
    switch (job)
    {
        case KDiamond::SwapDiamondsJob:
            if (m_selected1x == -1 || m_selected1y == -1 || m_selected2x == -1 || m_selected2y == -1)
                break; //this can be the case if, during a cascade, two diamonds are selected (inserts SwapDiamondsJob) and then deselected
            //ensure that the selected diamonds are neighbors (this is not necessarily the case as diamonds can move to fill gaps)
            dx = qAbs(m_selected1x - m_selected2x);
            dy = qAbs(m_selected1y - m_selected2y);
            if (dx + dy != 1)
            {
                m_selected1x = m_selected1y = m_selected2x = m_selected2y = -1;
                m_selection1->hide();
                m_selection2->hide();
                break;
            }
            //start a new cascade
            m_cascade = 0;
            //copy selection info into another storage (to allow the user to select the next two diamonds while the cascade runs)
            m_swapping1x = m_selected1x;
            m_swapping1y = m_selected1y;
            m_swapping2x = m_selected2x;
            m_swapping2y = m_selected2y;
            m_selected1x = m_selected1y = m_selected2x = m_selected2y = -1;
            m_selection1->hide();
            m_selection2->hide();
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
            KNotification::event("move");
            m_diamonds[m_swapping1x][m_swapping1y]->move(QPointF(m_swapping1x, m_swapping1y));
            m_diamonds[m_swapping2x][m_swapping2y]->move(QPointF(m_swapping2x, m_swapping2y));
            break;
        case KDiamond::RemoveRowsJob:
            m_cascade++;
            //find diamond rows and delete these diamonds
            m_diamondsToRemove = findCompletedRows();
            if (m_diamondsToRemove.count() == 0)
            {
                //no diamond rows were formed by the last move -> revoke movement (unless we are in a cascade)
                if (m_swapping1x != -1 && m_swapping1y != -1 && m_swapping2x != -1 && m_swapping2y != -1)
                    m_jobQueue << KDiamond::RevokeSwapDiamondsJob;
            }
            else
            {
                //it is now safe to delete the position of the swapping diamonds
                m_swapping1x = m_swapping1y = m_swapping2x = m_swapping2y = -1;
                //report to Game
                emit diamondsRemoved(m_diamondsToRemove.count(), m_cascade);
                //prepare to fill gaps
                m_jobQueue.prepend(KDiamond::FillGapsJob); //prepend this job as it has to be executed immediately after the animations (before handling any further user input)
                //invoke remove animation
                KNotification::event("remove");
                foreach (QPoint *diamondPos, m_diamondsToRemove)
                {
                    m_diamonds[diamondPos->x()][diamondPos->y()]->remove();
                    //remove selection if necessary
                    if (m_selected1x == diamondPos->x() && m_selected1y == diamondPos->y())
                    {
                        m_selected1x = m_selected2x;
                        m_selected1y = m_selected2y;
                        if (m_selected1x == -1 || m_selected1y == -1)
                            m_selection1->hide();
                        else
                        {
                            m_selection1->setPosInBoardCoords(QPointF(m_selected1x, m_selected1y));
                            m_selection1->show();
                        }
                        m_selected2x = -1;
                        m_selected2y = -1;
                        m_selection2->hide();
                    }
                    else if (m_selected2x == diamondPos->x() && m_selected2y == diamondPos->y())
                    {
                        m_selected2x = -1;
                        m_selected2y = -1;
                        m_selection2->hide();
                    }
                }
            }
            break;
        case KDiamond::FillGapsJob:
            //delete diamonds after their remove animation has played
            foreach (QPoint *diamondPos, m_diamondsToRemove)
            {
                delete m_diamonds[diamondPos->x()][diamondPos->y()];
                m_diamonds[diamondPos->x()][diamondPos->y()] = 0;
                //cleanup pointer
                delete diamondPos;
            }
            m_diamondsToRemove.clear();
            //fill gaps
            fillGaps();
            m_jobQueue.prepend(KDiamond::RemoveRowsJob); //allow cascades (i.e. clear rows that have been formed by falling diamonds)
            break;
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
            m_diamonds[x][yt] = m_diamonds[x][y];
            m_diamonds[x][y] = 0;
            m_diamonds[x][yt]->setYIndex(yt);
            m_diamonds[x][yt]->move(QPointF(x, yt));
            //if this element is selected, move the selection, too
            if (m_selected1x == x && m_selected1y == y)
            {
                m_selected1y = yt;
                m_selection1->move(QPointF(x, yt));
            }
            if (m_selected2x == x && m_selected2y == y)
            {
                m_selected2y = yt;
                m_selection2->move(QPointF(x, yt));
            }
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
            m_diamonds[x][y]->setPosInBoardCoords(QPointF(x, yt));
            m_diamonds[x][y]->updateGeometry();
            m_diamonds[x][y]->move(QPointF(x, y));
        }
    }
}

void Board::timeIsUp()
{
    m_selection1->hide();
    m_selection2->hide();
    m_timeIsUp = true;
    showMessage(i18nc("Not meant like 'You have lost', more like 'Time is up'.", "Game over."), 0);
}

void Board::showMessage(const QString &message, int timeout)
{
    m_messenger->setMessageTimeout(timeout);
    m_messenger->showMessage(message, KGamePopupItem::TopLeft);
}

void Board::hideMessage()
{
    m_messenger->forceHide();
}

#include "board.moc"
