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

#include "view.h"

#include <QWheelEvent>
#include <KLocalizedString>

KDiamond::View::View(QWidget* parent)
	: QGraphicsView(parent)
{
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	//optimize rendering
	setOptimizationFlags(QGraphicsView::DontSavePainterState | QGraphicsView::DontAdjustForAntialiasing);
	//"What's this?" context help
	setWhatsThis(i18n("<h3>Rules of Game</h3><p>Your goal is to assemble lines of at least three similar diamonds. Click on two adjacent diamonds to swap them.</p><p>Earn extra points by building cascades, and extra seconds by assembling big lines or multiple lines at one time.</p>"));
}

void KDiamond::View::resizeEvent(QResizeEvent* event)
{
	Q_UNUSED(event)
	emit resized();
	setTransform(QTransform());
}

void KDiamond::View::wheelEvent(QWheelEvent* event)
{
	//do not allow wheel events
	event->ignore();
}

#include "view.moc"
