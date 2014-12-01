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

KDiamond::InfoBar::InfoBar(QStatusBar* bar)
	: m_untimed(Settings::untimed())
	, m_bar(bar)
{
//PORT TO QT5
#if 0
	m_bar->insertPermanentItem(i18n("Points: %1", 0), 1, 1);
	if (m_untimed)
		m_bar->insertPermanentItem(i18n("Untimed game"), 2, 1);
	else
		m_bar->insertPermanentItem(i18n("Time left: %1", QLatin1String("0:00")), 2, 1);
	m_bar->insertPermanentItem(i18n("Possible moves: %1", 0), 3, 1);
	m_bar->show();
#endif
}

void KDiamond::InfoBar::setUntimed(bool untimed)
{
#if 0 //PORT QT5
	if (untimed)
		m_bar->changeItem(i18n("Untimed game"), 2);
	m_untimed = untimed;
#endif
}

void KDiamond::InfoBar::updatePoints(int points)
{
#if 0 //PORT QT5
	m_bar->changeItem(i18n("Points: %1", points), 1);
#endif
}

void KDiamond::InfoBar::updateMoves(int moves)
{
#if 0 //PORT QT5
	if (moves == -1)
		m_bar->changeItem(i18nc("Shown when the board is in motion.", "Possible moves: ..."), 3);
	else
		m_bar->changeItem(i18n("Possible moves: %1", moves), 3);
#endif
}

void KDiamond::InfoBar::updateRemainingTime(int remainingSeconds)
{
#if 0 //PORT QT5
	if (m_untimed)
		return;
	//split time in seconds and minutes
	int seconds = remainingSeconds % 60;
	int minutes = remainingSeconds / 60;
	//compose new string
	QString secondString = QString::number(seconds);
	QString minuteString = QString::number(minutes);
	if (seconds < 10)
		secondString.prepend(QChar('0'));
	m_bar->changeItem(i18n("Time left: %1", QString("%1:%2").arg(minuteString).arg(secondString)), 2);
	//special treatment if game is finished
	if (remainingSeconds == 0)
		updateMoves(0);
#endif
}

#include "infobar.moc"
