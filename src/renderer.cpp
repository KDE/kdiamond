/***************************************************************************
 *   Copyright 2008-2009 Stefan Majewsky <majewsky.stefan@ages-skripte.org>
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
#include "board.h"
#include "diamond.h"
#include "settings.h"

#include <QPainter>
#include <QPixmap>
#include <KGameTheme>
#include <KPixmapCache>
#include <QSvgRenderer>

typedef QPair<KDiamond::Color, int> RendererFrame;

class RendererPrivate
{
	public:
		RendererPrivate();

		QSvgRenderer m_renderer;
		KPixmapCache m_cache;

		QSize m_diamondSize;
		QSize m_sceneSize;

		QString m_currentTheme;
		int m_removeAnimFrameCount;
		bool m_hasBorder;
		qreal m_borderPercentage;

		QList<RendererFrame> m_framesToRender;
};

const QString sizeSuffix("_%1-%2");
const QString frameSuffix("-%1");

RendererPrivate::RendererPrivate()
	: m_renderer()
	, m_cache("kdiamond-cache")
{
	m_cache.setCacheLimit(5 * 1024);
}

Renderer::Renderer()
	: p(new RendererPrivate)
{
	loadTheme(Settings::theme());
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

bool Renderer::loadTheme(const QString &name)
{
	bool discardCache = !p->m_currentTheme.isEmpty();
	if (!p->m_currentTheme.isEmpty() && p->m_currentTheme == name)
		return true; //requested to load the theme that is already loaded
	KGameTheme theme;
	//try to load theme
	if (!theme.load(name))
	{
		if (!theme.loadDefault())
			return false;
	}
	p->m_currentTheme = name;
	p->m_removeAnimFrameCount = theme.property("RemoveAnimFrames").toInt();
	p->m_hasBorder = theme.property("HasBorder").toInt() > 0;
	p->m_borderPercentage = theme.property("BorderPercentage").toFloat();
	//load graphics
	if (!p->m_renderer.load(theme.graphics()))
		return false;
	//flush cache
	if (discardCache)
		p->m_cache.discard();
	//issue new frame render requests
	for (int color = 1; color <= 7; ++color) //color == 0 is for selections, do not pre-render animation frames for this one
	{
		const KDiamond::Color color2 = (KDiamond::Color) color;
		for (int frame = 0; frame < p->m_removeAnimFrameCount; ++frame)
			p->m_framesToRender << RendererFrame(color2, frame);
	}
	return true;
}

void Renderer::boardResized(int width, int height, int leftOffset, int diamondEdgeLength, int diamondCountOnEdge)
{
	//new metrics
	p->m_sceneSize = QSize(width, height);
	p->m_diamondSize = QSize(diamondEdgeLength, diamondEdgeLength);
	//pre-render the background (it is more complex than the other pixmaps because it may include the border)
	const QString svgName("kdiamond-background");
	const QString boardSvgName("kdiamond-border");
	QString pixName = p->m_currentTheme + svgName + sizeSuffix.arg(width).arg(height);
	QPixmap pix(QSize(width, height));
	if (!p->m_cache.find(pixName, pix))
	{
		pix.fill(Qt::transparent);
		QPainter painter(&pix);
		p->m_renderer.render(&painter, svgName);
		if (p->m_hasBorder)
		{
			const qreal innerBoardEdgeLength = diamondCountOnEdge * diamondEdgeLength;
			const qreal padding = p->m_borderPercentage * innerBoardEdgeLength;
			const int boardEdgeLength = 2.0 * padding + innerBoardEdgeLength;
			p->m_renderer.render(&painter, boardSvgName, QRect(leftOffset - padding, -padding, boardEdgeLength, boardEdgeLength));
		}
		painter.end();
		p->m_cache.insert(pixName, pix);
	}
}

int Renderer::removeAnimFrameCount()
{
	return p->m_removeAnimFrameCount;
}

bool Renderer::hasBorder()
{
	return p->m_hasBorder;
}

QString colorToString(KDiamond::Color color)
{
	switch (color)
	{
		case KDiamond::RedDiamond:
			return "kdiamond-red";
		case KDiamond::GreenDiamond:
			return "kdiamond-green";
		case KDiamond::BlueDiamond:
			return "kdiamond-blue";
		case KDiamond::YellowDiamond:
			return "kdiamond-yellow";
		case KDiamond::WhiteDiamond:
			return "kdiamond-white";
		case KDiamond::BlackDiamond:
			return "kdiamond-black";
		case KDiamond::OrangeDiamond:
			return "kdiamond-orange";
		default:
			return "kdiamond-selection";
	}
}

QPixmap pixmapFromCache(RendererPrivate *p, const QString &svgName, const QSize &size)
{
	if (size.isEmpty())
		return QPixmap();
	QPixmap pix(size);
	QString pixName = p->m_currentTheme + svgName + sizeSuffix.arg(size.width()).arg(size.height());

	if (!p->m_cache.find(pixName, pix))
	{
		pix.fill(Qt::transparent);
		QPainter painter(&pix);
		p->m_renderer.render(&painter, svgName);
		painter.end();
		p->m_cache.insert(pixName, pix);
	}
	return pix;
}

void Renderer::prerenderNextAnimationFrame()
{
	if (p->m_framesToRender.isEmpty())
		return;
	RendererFrame frameRequest = p->m_framesToRender.takeFirst();
	removeFrame(frameRequest.first, frameRequest.second);
}

QPixmap Renderer::diamond(KDiamond::Color color)
{
	return pixmapFromCache(p, colorToString(color), p->m_diamondSize);
}

QPixmap Renderer::removeFrame(KDiamond::Color color, int frame)
{
	if (frame < 0 || frame >= p->m_removeAnimFrameCount || color == KDiamond::Selection)
		return QPixmap();
	p->m_framesToRender.removeAll(RendererFrame(color, frame));
	return pixmapFromCache(p, colorToString(color) + frameSuffix.arg(frame), p->m_diamondSize);
}

QPixmap Renderer::background()
{
	return pixmapFromCache(p, "kdiamond-background", p->m_sceneSize);
}
