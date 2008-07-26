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

#include <QTimeLine> //gives a compile error if included after my own headers

#include "animator.h"
#include "diamond.h"
#include "renderer.h"

#include <QTime>

Animator::Animator()
	: m_duration(0)
	, m_frameCount(0)
	, m_started(false)
	, m_playedLastFrame(false)
	, m_time(0)
	, m_timer(0)
{
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

void Animator::addItem(Diamond *item, const QPointF &from, const QPointF &to)
{
	if (m_started)
		return;
	AnimationData data;
	data.diamond = item;
	data.from = from;
	data.to = to;
	m_data << data;
}

void Animator::start()
{
	m_started = true;
	m_timer = new QTimeLine;
	m_timer->setDuration(m_duration);
	m_timer->setFrameRange(1, m_frameCount);
	m_time = new QTime;
	connect(m_timer, SIGNAL(frameChanged(int)), this, SLOT(setFrame(int)));
	connect(m_timer, SIGNAL(finished()), this, SLOT(slotFinished()));
	m_time->start();
	m_timer->start();
}

void Animator::slotFinished()
{
	if (!m_playedLastFrame)
		setFrame(m_frameCount);
	emit finished();
}

MoveAnimator::MoveAnimator()
	: Animator()
{
}

void MoveAnimator::setMoveLength(int moveLength)
{
	m_duration = moveLength * KDiamond::MoveDuration;
	m_frameCount = moveLength * KDiamond::MoveFrameCount;
}

void MoveAnimator::setFrame(int frame)
{
	qreal x, y, difference = (qreal) frame / (qreal) KDiamond::MoveFrameCount;
	foreach (const AnimationData &data, m_data)
	{
		//kDebug() << "Diamond moving from " << data.from << " to " << data.to << " at " << time.elapsed();
		//the absolute value of the actual difference can not be more than the calculated maximum difference
		x = data.from.x() + qBound(-difference, data.to.x() - data.from.x(), difference);
		y = data.from.y() + qBound(-difference, data.to.y() - data.from.y(), difference);
		data.diamond->setPosInBoardCoords(QPointF(x, y));
		//kDebug() << "Diamond moved from " << data.from << " to " << data.to << " at " << time.elapsed();
	}
	if (frame == m_frameCount)
		m_playedLastFrame = true;
}

RemoveAnimator::RemoveAnimator()
	: Animator()
{
	m_duration = KDiamond::RemoveDuration;
	m_frameCount = Renderer::self()->removeAnimFrameCount();
}

void RemoveAnimator::setFrame(int frame)
{
	foreach (const AnimationData &data, m_data)
		data.diamond->setPixmap(Renderer::self()->removeFrame(data.diamond->color(), frame - 1));
	if (frame == m_frameCount)
		m_playedLastFrame = true;
}

#include "animator.moc"
