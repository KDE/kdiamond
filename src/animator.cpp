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

#include "animator.h"
#include "diamond.h"
#include "renderer.h"

#include <QTimeLine>

Animator::Animator()
{
    m_duration = m_frameCount = 0;
    m_started = m_playedLastFrame = false;
    m_timer = 0;
}

Animator::~Animator()
{
    delete m_timer;
}

int Animator::duration() const
{
    return m_duration;
}

int Animator::frameCount() const
{
    return m_frameCount;
}

void Animator::addItem(Diamond *item, const QPointF &position, int maxPositionDiff)
{
    if (!(m_started || m_items.contains(item)))
    {
        m_items << item;
        m_positions << position;
        m_maxPositionDiffs << maxPositionDiff;
    }
}

void Animator::start()
{
    m_started = true;
    m_timer = new QTimeLine;
    m_timer->setDuration(m_duration);
    m_timer->setFrameRange(1, m_frameCount);
    connect(m_timer, SIGNAL(frameChanged(int)), this, SLOT(setFrame(int)));
    connect(m_timer, SIGNAL(finished()), this, SLOT(slotFinished()));
    m_timer->start();
}

void Animator::slotFinished()
{
    if (!m_playedLastFrame)
        setFrame(m_frameCount);
    emit finished();
}

XMoveAnimator::XMoveAnimator(int dx)
    : Animator()
{
    dx = qAbs(m_dx = dx); //same like m_dx = dx; dx = qAbs(dx);
    m_duration = dx * KDiamond::MoveDuration;
    m_frameCount = dx * KDiamond::MoveFrameCount;
}

#include <KDebug>

void XMoveAnimator::setFrame(int frame)
{
    //calculate difference
    qreal diff = (m_dx * (qreal) frame) / (qreal) m_frameCount;
    //move diamonds
    QList<Diamond *>::const_iterator iterItem = m_items.constBegin();
    QList<QPointF>::const_iterator iterPos = m_positions.constBegin();
    QList<int>::const_iterator iterDiff = m_maxPositionDiffs.constBegin();
    int debug = 0;
    while (iterItem != m_items.constEnd()) {
        if (*iterDiff == 0 || qAbs(diff) <= qAbs(*iterDiff))
            (*iterItem)->setPosInBoardCoords(QPointF((*iterPos).x() + diff, (*iterPos).y()));
        ++iterDiff; ++iterItem; ++iterPos; ++debug;
    }
    if (frame == m_frameCount)
        m_playedLastFrame = true;
    kDebug() << frame << debug << m_playedLastFrame;
}

YMoveAnimator::YMoveAnimator(int dy)
    : Animator()
{
    dy = qAbs(m_dy = dy); //same like m_dy = dy; dy = qAbs(dy);
    m_duration = dy * KDiamond::MoveDuration;
    m_frameCount = dy * KDiamond::MoveFrameCount;
}

void YMoveAnimator::setFrame(int frame)
{
    //calculate difference
    qreal diff = (m_dy * (qreal) frame) / (qreal) m_frameCount;
    //move diamonds
    QList<Diamond *>::const_iterator iterItem = m_items.constBegin();
    QList<QPointF>::const_iterator iterPos = m_positions.constBegin();
    QList<int>::const_iterator iterDiff = m_maxPositionDiffs.constBegin();
    int debug = 0;
    while (iterItem != m_items.constEnd()) {
        if (*iterDiff == 0 || qAbs(diff) <= qAbs(*iterDiff))
            (*iterItem)->setPosInBoardCoords(QPointF((*iterPos).x(), (*iterPos).y() + diff));
        ++iterDiff; ++iterItem; ++iterPos; ++debug;
    }
    if (frame == m_frameCount)
        m_playedLastFrame = true;
    kDebug() << frame << debug << m_playedLastFrame;
}

RemoveAnimator::RemoveAnimator()
    : Animator()
{
    m_duration = KDiamond::RemoveDuration;
    m_frameCount = Renderer::removeAnimFrameCount();
}

void RemoveAnimator::setFrame(int frame)
{
    foreach (Diamond *item, m_items)
        item->setPixmap(Renderer::removeFrame(item->color(), frame - 1));
}

#include "animator.moc"
