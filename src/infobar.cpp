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

#include "infobar.h"
#include "settings.h"

#include <KLocalizedString>
#include <KMainWindow>
#include <KStatusBar>

KDiamond::InfoBar::InfoBar(KMainWindow* parent)
	: m_untimed(Settings::untimed())
{
	//initialize items
	insertPermanentItem(i18n("Points: %1", 0), 1, 1);
	if (m_untimed)
		insertPermanentItem(i18n("Untimed game"), 2, 1);
	else
		insertPermanentItem(i18np("Time left: 1 second", "Time left: %1 seconds", 0), 2, 1);
	insertPermanentItem(i18n("Possible moves: %1", 0), 3, 1);
	//insert into parent
	if (parent != 0)
		parent->setStatusBar(this);
}

void KDiamond::InfoBar::setShowMinutes(bool showMinutes)
{
	Settings::setShowMinutes(showMinutes);
	updateRemainingTime(-1);
}

void KDiamond::InfoBar::setUntimed(bool untimed)
{
	if (untimed)
		changeItem(i18n("Untimed game"), 2);
	m_untimed = untimed;
}

void KDiamond::InfoBar::updatePoints(int points)
{
	changeItem(i18n("Points: %1", points), 1);
}

void KDiamond::InfoBar::updateMoves(int moves)
{
	if (moves == -1)
		changeItem(i18n("Possible moves: %1").arg("..."), 3);
	else
		changeItem(i18n("Possible moves: %1", moves), 3);
}

void KDiamond::InfoBar::updateRemainingTime(int remainingSeconds)
{
	if (m_untimed)
		return;
	//store the time: if remainingSeconds == -1, the old time is just re-rendered (used to apply configuration actions)
	static int storeRemainingSeconds = 0;
	if (remainingSeconds == -1)
		remainingSeconds = storeRemainingSeconds;
	else
		storeRemainingSeconds = remainingSeconds;
	//split time in seconds and minutes if wanted
	int seconds, minutes;
	if (Settings::showMinutes())
	{
		seconds = remainingSeconds % 60;
		minutes = remainingSeconds / 60;
	}
	else
	{
		seconds = remainingSeconds;
		minutes = 0; //the minutes do not appear in the output when minutes == 0
	}
	//compose new string
	QString sOutput;
	if (minutes == 0)
		sOutput = i18n("Time left: %1", i18np("1 second", "%1 seconds", seconds));
	else if (seconds == 0)
		sOutput = i18n("Time left: %1", i18np("1 minute", "%1 minutes", minutes));
	else
		sOutput = i18nc("The two parameters are strings like '2 minutes' or '1 second'.", "Time left: %1, %2", i18np("1 minute", "%1 minutes", minutes), i18np("1 second", "%1 seconds", seconds));
	changeItem(sOutput, 2);
}

#include "infobar.moc"