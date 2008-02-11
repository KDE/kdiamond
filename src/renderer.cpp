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

#include "renderer.h"
#include "diamond.h"

#include <QPainter>
#include <QPixmap>
#include <KGameTheme>
#include <KPixmapCache>
#include <KSvgRenderer>

//TODO: Load theme from settings in Renderer::init().

//global instances (only visible in this file!)
KSvgRenderer *g_renderer;
KPixmapCache *g_cache;
//storage for metrics and more
int g_sceneWidth, g_sceneHeight, g_diamondEdgeLength;
QSize g_diamondSize, g_sceneSize;
QString g_currentTheme;

bool Renderer::init()
{
    g_renderer = new KSvgRenderer();
    g_cache = new KPixmapCache("kdiamond-cache");
    g_cache->setCacheLimit(3 * 1024);
    g_cache->discard();
    //No cleanup is necessary. The functions are needed during the whole running time of the program, after that the memory will be cleaned up automatically.
    return Renderer::loadTheme("themes/classic.desktop");
}

bool Renderer::loadTheme(const QString &name)
{
    bool discardCache = !g_currentTheme.isEmpty();
    if (!discardCache && g_currentTheme == name)
        return true; //requested to load the theme that is already loaded
    KGameTheme theme;
    //try to load theme
    if (!theme.load(name))
    {
        if (!theme.loadDefault())
            return false;
    }
    g_currentTheme = name;
    //load graphics
    if (!g_renderer->load(theme.graphics()))
        return false;
    //flush cache
    if (discardCache)
        g_cache->discard();
    return true;
}

void Renderer::boardResized(int width, int height, int diamondEdgeLength)
{
    //new metrics
    g_sceneWidth = width;
    g_sceneHeight = height;
    g_diamondEdgeLength = diamondEdgeLength;
    //new sizes
    g_diamondSize = QSize(g_diamondEdgeLength, g_diamondEdgeLength);
    g_sceneSize = QSize(g_sceneWidth, g_sceneHeight);
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

QPixmap pixmapFromCache(const QString &svgName, const QSize &size)
{
    if (size.isEmpty())
        return QPixmap();
    QPixmap pix;
    QString pixName = svgName + QString("_%1-%2").arg(size.width()).arg(size.height());

    if (!g_cache->find(pixName, pix))
    {
        pix = QPixmap(size);
        pix.fill(Qt::transparent);
        QPainter p(&pix);
        p.setRenderHint(QPainter::Antialiasing);
        g_renderer->render(&p, svgName);
        p.end();
        g_cache->insert(pixName, pix);
    }
    return pix;
}

QPixmap Renderer::diamond(KDiamond::Color color)
{
    return pixmapFromCache(colorToString(color), g_diamondSize);
}

QPixmap Renderer::background()
{
    return pixmapFromCache("kdiamond-background", g_sceneSize);
}
