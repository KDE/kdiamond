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

#ifndef KDIAMOND_RENDERER_H
#define KDIAMOND_RENDERER_H

#include "diamond.h"

class KGameRenderer;
class QPixmap;
class RendererPrivate;

class Renderer {
	private:
		Renderer();
		~Renderer();
		Q_DISABLE_COPY(Renderer)
	public:
		static Renderer *self();
		KGameRenderer* renderer();

		void loadTheme(const QString &name);
		QPixmap background(const QSize& sceneSize, int leftOffset, int boardSize);
	private:
		RendererPrivate *p;
};

#endif // KDIAMOND_RENDERER_H
