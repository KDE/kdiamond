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

#include "renderer.h"
#include "diamond.h"
#include "settings.h"
#include <KGameRenderer>

#include <QPainter>
#include <QPixmap>
#include <KGameTheme>

//TODO: port to KGameRenderer temporarily sacrificed prerendering of animation frames

class RendererPrivate
{
	public:
		RendererPrivate();

		KGameRenderer m_renderer;

		bool m_hasBorder;
		qreal m_borderPercentage;
};

RendererPrivate::RendererPrivate()
	: m_renderer(Settings::theme(), "themes/default.desktop")
{
	m_renderer.setFrameSuffix(QString::fromLatin1("-%1"));
}

Renderer::Renderer()
	: p(new RendererPrivate)
{
	loadTheme(p->m_renderer.theme()); //HACK: This is not needed, but this call sets m_hasBorder and m_borderPercentage.
}

Renderer::~Renderer()
{
	delete p;
}

Renderer *Renderer::self()
{
	static Renderer r;
	return &r;
}

KGameRenderer* Renderer::renderer()
{
	return &p->m_renderer;
}

void Renderer::loadTheme(const QString &name)
{
	p->m_renderer.setTheme(name);
	const KGameTheme* gameTheme = p->m_renderer.gameTheme();
	p->m_hasBorder = gameTheme->property("HasBorder").toInt() > 0;
	p->m_borderPercentage = gameTheme->property("BorderPercentage").toFloat();
}

QPixmap Renderer::background(const QSize& sceneSize, int leftOffset, int boardSize)
{
	QPixmap pix = p->m_renderer.spritePixmap("kdiamond-background", sceneSize);
	if (p->m_hasBorder)
	{
		const int padding = p->m_borderPercentage * boardSize;
		const int boardEdgeLength = 2 * padding + boardSize;
		QRect boardGeometry(leftOffset - padding, -padding, boardEdgeLength, boardEdgeLength);
		const QPixmap boardPix = p->m_renderer.spritePixmap("kdiamond-border", boardGeometry.size());
		QPainter painter(&pix);
		painter.drawPixmap(boardGeometry, boardPix);
	}
	return pix;
}
