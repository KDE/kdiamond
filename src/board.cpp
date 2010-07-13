/***************************************************************************
 *   Copyright 2008-2010 Stefan Majewsky <majewsky@gmx.net>
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

#include <QPropertyAnimation>
#include <cmath>

const int KDiamond::Board::MoveDuration = 240; //duration of a move animation (per coordinate unit) in milliseconds
const int KDiamond::Board::RemoveDuration = 200; //duration of a move animation in milliseconds

//NOTE: The corresponding difficulty values are {20, 30, 40, 50, 60} (see KGameDifficulty::standardLevel).
static int boardSizes[] = { 12, 10, 8, 8, 8 };
static int boardColorCounts[] = { 5, 5, 5, 6, 7 };

KDiamond::Board::Board(KGameRenderer* renderer, KGameDifficulty::standardLevel difficulty)
	: m_size(boardSizes[difficulty / 10 - 2])
	, m_diamondRenderSize(0)
	, m_colorCount(boardColorCounts[difficulty / 10 - 2])
	, m_paused(false)
	, m_renderer(renderer)
	, m_diamonds(m_size * m_size, 0)
{
	for (QPoint point; point.x() < m_size; ++point.rx())
		for (point.ry() = 0; point.y() < m_size; ++point.ry())
		{
			//displacement vectors needed for the following algorithm
			const QPoint dispY1(0, -1), dispY2(0, -2);
			const QPoint dispX1(-1, 0), dispX2(-2, 0);
			//roll the dice to get a color, but ensure that there are not three of a color in a row from the start
			int color;
			while (true)
			{
				color = qrand() % m_colorCount + 1; // +1 because numbering of enum KDiamond::Color starts at 1
				//condition: no triplet in y axis (attention: only the diamonds above us are defined already)
				if (point.y() >= 2) //no triplet possible for i = 0, 1
				{
					const int otherColor1 = diamond(point + dispY1)->color();
					const int otherColor2 = diamond(point + dispY2)->color();
					if (otherColor1 == color && otherColor2 == color)
						continue; //roll the dice again
				}
				//same condition on x axis
				if (point.x() >= 2)
				{
					const int otherColor1 = diamond(point + dispX1)->color();
					const int otherColor2 = diamond(point + dispX2)->color();
					if (otherColor1 == color && otherColor2 == color)
						continue;
				}
				break;
			}
			rDiamond(point) = spawnDiamond(color);
			diamond(point)->setPos(point);
		}
}

Diamond* KDiamond::Board::spawnDiamond(int color)
{
	Diamond* diamond = new Diamond((KDiamond::Color) color, m_renderer, this);
	diamond->setRenderSize(m_diamondRenderSize);
	connect(diamond, SIGNAL(clicked()), SLOT(slotClicked()));
	connect(diamond, SIGNAL(dragged(QPoint)), SLOT(slotDragged(QPoint)));
	return diamond;
}

QPoint KDiamond::Board::findDiamond(Diamond* diamond) const
{
	int index = m_diamonds.indexOf(diamond);
	if (index == -1)
		return QPoint(-1, -1);
	else
		return QPoint(index % m_size, index / m_size);
}

Diamond*& KDiamond::Board::rDiamond(const QPoint& point)
{
	return m_diamonds[point.x() + point.y() * m_size];
}

Diamond* KDiamond::Board::diamond(const QPoint& point) const
{
	return m_diamonds.value(point.x() + point.y() * m_size);
}

int KDiamond::Board::gridSize() const
{
	return m_size;
}

void KDiamond::Board::determineRenderSize(QSize& renderSize)
{
	//reduce own render size to square shape and round to integer diamond size for efficient rendering
	m_diamondRenderSize = (int) floor(qMin(
		renderSize.width() / (m_size + 1.0), //the "+1" and "+0.5" make sure that some space is left on the window for the board border
		renderSize.height() / (m_size + 0.5)
	));
	renderSize = QSize(m_size, m_size) * m_diamondRenderSize;
	//apply render size
	foreach (Diamond* item, m_diamonds)
		if (item)
			item->setRenderSize(m_diamondRenderSize);
	foreach (Diamond* item, m_activeSelectors + m_inactiveSelectors)
		item->setRenderSize(m_diamondRenderSize);
}

bool KDiamond::Board::hasDiamond(const QPoint& point) const
{
	return 0 <= point.x() && point.x() < m_size && 0 <= point.y() && point.y() < m_size;
}

bool KDiamond::Board::hasRunningAnimations() const
{
	return !m_runningAnimations.isEmpty();
}

void KDiamond::Board::slotAnimationFinished()
{
	if (m_runningAnimations.isEmpty())
		return;
	m_runningAnimations.removeAll(reinterpret_cast<QAbstractAnimation*>(sender()));
	if (m_runningAnimations.isEmpty())
		emit animationsFinished();
}

QList<QPoint> KDiamond::Board::selections() const
{
	return m_selections;
}

bool KDiamond::Board::hasSelection(const QPoint& point) const
{
	return m_selections.contains(point);
}

void KDiamond::Board::setSelection(const QPoint& point, bool selected)
{
	const int index = m_selections.indexOf(point);
	if ((index >= 0) == selected)
		//nothing to do
		return;
	if (selected)
	{
		//add selection, possibly by reusing an old item instance
		Diamond* selector;
		if (!m_inactiveSelectors.isEmpty())
			selector = m_inactiveSelectors.takeLast();
		else
		{
			selector = new Diamond(KDiamond::Selection, m_renderer, this);
			selector->setRenderSize(m_diamondRenderSize);
		}
		m_activeSelectors << selector;
		m_selections << point;
		selector->setPos(point);
		selector->show();
	}
	else
	{
		//remove selection, but try to reuse item instance later
		m_selections.removeAt(index);
		Diamond* selector = m_activeSelectors.takeAt(index);
		m_inactiveSelectors << selector;
		selector->hide();
	}
}

void KDiamond::Board::clearSelection()
{
	foreach (Diamond* selector, m_activeSelectors)
	{
		selector->hide();
		m_inactiveSelectors << selector;
	}
	m_selections.clear();
	m_activeSelectors.clear();
}

void KDiamond::Board::setPaused(bool paused)
{
	//During pauses, the board is hidden and any animations are suspended.
	const bool visible = !paused;
	if (isVisible() == visible)
		return;
	setVisible(visible);
	QList<QAbstractAnimation*>::const_iterator it1 = m_runningAnimations.begin(), it2 = m_runningAnimations.end();
	for (; it1 != it2; ++it1)
		(*it1)->setPaused(paused);
}

void KDiamond::Board::removeDiamond(const QPoint& point)
{
	Diamond* diamond = this->diamond(point);
	if (!diamond)
		return; //diamond has already been removed
	rDiamond(point) = 0;
	//play remove animation (TODO: For non-animated sprites, play an opacity animation instead.)
	QPropertyAnimation* animation = new QPropertyAnimation(diamond, "frame", this);
	animation->setStartValue(0);
	animation->setEndValue(diamond->frameCount() - 1);
	animation->setDuration(KDiamond::Board::RemoveDuration);
	animation->start(QAbstractAnimation::DeleteWhenStopped);
	connect(animation, SIGNAL(finished()), this, SLOT(slotAnimationFinished()));
	connect(animation, SIGNAL(finished()), diamond, SLOT(deleteLater()));
	m_runningAnimations << animation;
}

void KDiamond::Board::spawnMoveAnimations(const QList<MoveAnimSpec>& specs)
{
	foreach (const MoveAnimSpec& spec, specs)
	{
		const int duration = KDiamond::Board::MoveDuration * (spec.to - spec.from).manhattanLength();
		QPropertyAnimation* animation = new QPropertyAnimation(spec.diamond, "pos", this);
		animation->setStartValue(spec.from);
		animation->setEndValue(spec.to);
		animation->setDuration(duration);
		animation->start(QAbstractAnimation::DeleteWhenStopped);
		connect(animation, SIGNAL(finished()), this, SLOT(slotAnimationFinished()));
		m_runningAnimations << animation;
	}
}

void KDiamond::Board::swapDiamonds(const QPoint& point1, const QPoint& point2)
{
	//swap diamonds in internal representation
	Diamond* diamond1 = this->diamond(point1);
	Diamond* diamond2 = this->diamond(point2);
	rDiamond(point1) = diamond2;
	rDiamond(point2) = diamond1;
	//play movement animations
	const MoveAnimSpec spec1 = { diamond1, point1, point2 };
	const MoveAnimSpec spec2 = { diamond2, point2, point1 };
	spawnMoveAnimations(QList<MoveAnimSpec>() << spec1 << spec2);
}

void KDiamond::Board::fillGaps()
{
	QList<MoveAnimSpec> specs;
	//fill gaps
	int x, y, yt; //counters - (x, yt) is the target position of diamond (x,y)
	for (x = 0; x < m_size; ++x)
	{
		//We have to search from the bottom of the column. Exclude the lowest element (x = m_size - 1) because it cannot move down.
		for (y = m_size - 2; y >= 0; --y)
		{
			if (!diamond(QPoint(x, y)))
				//no need to move gaps -> these are moved later
				continue;
			if (diamond(QPoint(x, y + 1)))
				//there is something right below this diamond -> Do not move.
				continue;
			//search for the lowest possible position
			for (yt = y; yt < m_size - 1; ++yt)
			{
				if (diamond(QPoint(x, yt + 1)))
					break; //xt now holds the lowest possible position
			}
			rDiamond(QPoint(x, yt)) = diamond(QPoint(x, y));
			rDiamond(QPoint(x, y)) = 0;
			const MoveAnimSpec spec = { diamond(QPoint(x, yt)), QPoint(x, y), QPoint(x, yt) };
			specs << spec;
			//if this element is selected, move the selection, too
			const int index = m_selections.indexOf(QPoint(x, y));
			if (index != -1)
			{
				m_selections.replace(index, QPoint(x, yt));
				const MoveAnimSpec spec = { m_activeSelectors[index], QPoint(x, y), QPoint(x, yt) };
				specs << spec;
			}
		}
	}
	//fill top rows with new elements
	for (x = 0; x < m_size; ++x)
	{
		yt = 0; //now: holds the position from where the diamond comes (-1 for the lowest new diamond)
		for (y = m_size - 1; y >= 0; --y)
		{
			Diamond*& diamond = this->rDiamond(QPoint(x, y));
			if (diamond)
				continue; //inside of diamond stack - no gaps to fill
			--yt;
			diamond = spawnDiamond(qrand() % m_colorCount + 1);
			diamond->setPos(QPoint(x, yt));
			const MoveAnimSpec spec = { diamond, QPoint(x, yt), QPoint(x, y) };
			specs << spec;
		}
	}
	spawnMoveAnimations(specs);
}

void KDiamond::Board::slotClicked()
{
	const QPoint point = findDiamond(qobject_cast<Diamond*>(sender()));
	if (point.x() + point.y() > 0)
		emit clicked(point);
}

void KDiamond::Board::slotDragged(const QPoint& direction)
{
	const QPoint point = findDiamond(qobject_cast<Diamond*>(sender()));
	if (point.x() + point.y() > 0)
		emit dragged(point, direction);
}

QRectF KDiamond::Board::boundingRect() const
{
	return QRectF();
}

void KDiamond::Board::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	Q_UNUSED(painter) Q_UNUSED(option) Q_UNUSED(widget)
}

#include "board.moc"
