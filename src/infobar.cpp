/***************************************************************************
 *   Copyright 2008-2009 Stefan Majewsky <majewsky@gmx.net>
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

#include "infobar.h"
#include "settings.h"

#include <KLocalizedString>
#include <QStatusBar>
#include <QLabel>

KDiamond::InfoBar::InfoBar(QStatusBar* bar)
	: m_untimed(Settings::untimed())
	, m_bar(bar)
{
    mMovement = new QLabel(i18n("Possible moves: %1", 0));
    mPoints = new QLabel(i18n("Points: %1", 0));
    mTime = new QLabel;
    if (m_untimed)
        mTime->setText(i18n("Untimed game"));
    else
        mTime->setText(i18n("Time left: %1", QLatin1String("0:00")));
    m_bar->addPermanentWidget(mPoints);
    m_bar->addPermanentWidget(mTime);
    m_bar->addPermanentWidget(mMovement);
	m_bar->show();
}

void KDiamond::InfoBar::setUntimed(bool untimed)
{
	if (untimed)
        mTime->setText(i18n("Untimed game"));
	m_untimed = untimed;
}

void KDiamond::InfoBar::updatePoints(int points)
{
    mPoints->setText(i18n("Points: %1", points));
}

void KDiamond::InfoBar::updateMoves(int moves)
{
	if (moves == -1)
        mMovement->setText(i18nc("Shown when the board is in motion.", "Possible moves: ..."));
	else
        mMovement->setText(i18n("Possible moves: %1", moves));
}

void KDiamond::InfoBar::updateRemainingTime(int remainingSeconds)
{
	if (m_untimed)
		return;
	//split time in seconds and minutes
    const int seconds = remainingSeconds % 60;
    const int minutes = remainingSeconds / 60;
	//compose new string
    QString secondString = QString::number(seconds);
    const QString minuteString = QString::number(minutes);
	if (seconds < 10)
        secondString.prepend(QLatin1Char('0'));
    mTime->setText(i18n("Time left: %1", QString::fromLatin1("%1:%2").arg(minuteString).arg(secondString)));
	//special treatment if game is finished
	if (remainingSeconds == 0)
		updateMoves(0);
}


