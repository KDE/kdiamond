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

#ifndef KDIAMOND_RENDERER_H
#define KDIAMOND_RENDERER_H

#include "diamond.h"

class QPixmap;
class RendererPrivate;

class Renderer {
	private:
		Renderer();
		Renderer(const Renderer &);
		~Renderer();
	public:
		static Renderer *self();

		bool loadTheme(const QString &name);
		void boardResized(int width, int height, int leftOffset, int diamondEdgeLength, int diamondCountOnEdge);

		int removeAnimFrameCount();
		bool hasBorder();

		QPixmap diamond(KDiamond::Color color);
		QPixmap removeFrame(KDiamond::Color color, int frame);
		QPixmap background();
	private:
		RendererPrivate *p;
};

#endif // KDIAMOND_RENDERER_H
