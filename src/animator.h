/***************************************************************************
 *   Copyright 2008 Stefan Majewsky <majewsky.stefan@ages-skripte.org>
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

#ifndef KDIAMOND_ANIMATOR_H
#define KDIAMOND_ANIMATOR_H

#ifndef KDIAMOND_DIAMOND_H
	class Diamond;
#endif

#include <QList>
#include <QObject>
#include <QPointF>
class QTimeLine;

namespace KDiamond
{
	//duration of a move animation (per coordinate unit) in milliseconds
	const int MoveDuration = 240;
	//frame count during a move animation (KDiamond::MoveDuration should be divideable by KDiamond::MoveInterval) - 40 milliseconds per frame is a sane default as it equals 25 pictures per second which is the minimum speed required to get a flicker-free movement (in the ideal case)
	const int MoveFrameCount = 6;
	//duration of a move animation (frame count is determined by the theme)
	const int RemoveDuration = 200;
}

struct AnimationData
{
	Diamond *diamond;
	QPointF from, to;
};

class Animator : public QObject
{
	Q_OBJECT
	public:
		Animator();
		~Animator();

		int duration() const;
		int frameCount() const;

		void addItem(Diamond *item, const QPointF &from = QPointF(), const QPointF &to = QPointF());
		void start();
	protected slots:
		virtual void setFrame(int) = 0;
		void slotFinished();
	signals:
		void finished();
	protected:
		int m_duration;
		int m_frameCount;

		bool m_started, m_playedLastFrame; //the latter prevents a race condition between the last setFrame and finished
		QTimeLine *m_timer;

		QList<AnimationData> m_data;
};

class MoveAnimator : public Animator
{
	Q_OBJECT
	public:
		MoveAnimator();
		void setMoveLength(int moveLength);
	protected slots:
		virtual void setFrame(int);
};

class RemoveAnimator : public Animator
{
	Q_OBJECT
	public:
		RemoveAnimator();
	protected slots:
		virtual void setFrame(int);
};

#endif
