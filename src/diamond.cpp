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

#include "diamond.h"
#include "board.h"
#include "renderer.h"

#include <QPainter>

KDiamond::Color KDiamond::colorFromNumber(int number)
{
    switch (number)
    {
        case 1: return KDiamond::RedDiamond;
        case 2: return KDiamond::GreenDiamond;
        case 3: return KDiamond::BlueDiamond;
        case 4: return KDiamond::YellowDiamond;
        case 5: return KDiamond::WhiteDiamond;
        case 6: return KDiamond::BlackDiamond;
        case 7: return KDiamond::OrangeDiamond;
        default: return KDiamond::RedDiamond; //will be a sane default in most cases
    }
}

Diamond::Diamond(int xIndex, int yIndex, qreal xPos, qreal yPos, KDiamond::Color color, Board *board)
    : QWidget(board)
{
    m_xIndex = xIndex;
    m_yIndex = yIndex;
    m_xPos = xPos;
    m_yPos = yPos;
    m_color = color;
    m_board = board;
    m_moving = false;
    m_selected = false;
}

KDiamond::Color Diamond::color() const
{
    return m_color;
}

int Diamond::xIndex() const
{
    return m_xIndex;
}

int Diamond::yIndex() const
{
    return m_yIndex;
}

qreal Diamond::xPos() const
{
    return m_xPos;
}

qreal Diamond::yPos() const
{
    return m_yPos;
}

void Diamond::setXIndex(int xIndex)
{
    m_xIndex = xIndex;
}

void Diamond::setYIndex(int yIndex)
{
    m_yIndex = yIndex;
}

void Diamond::move(qreal xPos, qreal yPos)
{
    m_xTarget = xPos;
    m_yTarget = yPos;
    connect(m_board, SIGNAL(updateScheduled(int)), this, SLOT(moveAnimation(int)));
}

void Diamond::select(bool selected)
{
    if (m_selected == selected)
        return;
    m_selected = selected;
    update();
}

void Diamond::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QRectF bounds(0.0, 0.0, width(), height());
    if (m_selected)
        Renderer::drawShadow(&p, bounds);
    Renderer::drawDiamond(&p, bounds, m_color);
}

void Diamond::mouseReleaseEvent(QMouseEvent *)
{
    if (m_selected)
        m_board->unselectDiamond(m_xIndex, m_yIndex);
    else
        m_board->selectDiamond(m_xIndex, m_yIndex);
    m_selected = !m_selected;
    update();
}

void Diamond::moveAnimation(int milliseconds)
{
    //move each coordinate towards the target position
    qreal dx = m_xTarget - m_xPos;
    qreal dy = m_yTarget - m_yPos;
    if (dx == 0.0 && dy == 0.0)
        //advertise end of animation to m_board by disconnecting from update signal
        m_board->disconnect(SIGNAL(updateScheduled(int)), this, SLOT(moveAnimation(int)));
    else
    {
        //maximum absolute value is maxStep
        qreal maxStep = ((qreal) milliseconds) / KDiamond::MoveDuration;
        if (qAbs(dx) > maxStep)
            dx = maxStep * dx / qAbs(dx); // = maxStep * sign of dx
        if (qAbs(dy) > maxStep)
            dy = maxStep * dy / qAbs(dy);
        //apply position change
        m_xPos += dx;
        m_yPos += dy;
        m_board->updateDiamond(m_xIndex, m_yIndex);
    }
}

#include "diamond.moc"
