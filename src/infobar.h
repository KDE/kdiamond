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

#ifndef KDIAMOND_INFOBAR_H
#define KDIAMOND_INFOBAR_H

#include <QObject>
class KStatusBar;

namespace KDiamond
{

	class InfoBar : public QObject
	{
		Q_OBJECT
		public:
			InfoBar(KStatusBar* bar);
		public Q_SLOTS:
			void setShowMinutes(bool showMinutes);
			void setUntimed(bool untimed);
			void updatePoints(int points);
			void updateMoves(int moves);
			void updateRemainingTime(int remainingSeconds);
		private:
			bool m_untimed;
			KStatusBar* m_bar;
	};

}

#endif // KDIAMOND_INFOBAR_H