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
#include "board.h"
#include "diamond.h"
#include "settings.h"

#include <QPainter>
#include <QPixmap>
#include <KGameTheme>
#include <KPixmapCache>
#include <KSvgRenderer>

//global instances (only visible in this file!)
KSvgRenderer *g_renderer;
KPixmapCache *g_cache;
//storage for metrics
int g_sceneWidth, g_sceneHeight, g_diamondEdgeLength;
QSize g_diamondSize, g_sceneSize;
//storage for theme properties
QString g_currentTheme;
int g_removeAnimFrameCount;
bool g_hasBorder;

bool Renderer::init()
{
    g_renderer = new KSvgRenderer();
    g_cache = new KPixmapCache("kdiamond-cache");
    g_cache->setCacheLimit(3 * 1024);
    g_cache->discard();
    //No cleanup is necessary. The functions are needed during the whole running time of the program, after that the memory will be cleaned up automatically.
    return Renderer::loadTheme(Settings::theme());
}

bool Renderer::loadTheme(const QString &name)
{
    bool discardCache = !g_currentTheme.isEmpty();
    if (!g_currentTheme.isEmpty() && g_currentTheme == name)
        return true; //requested to load the theme that is already loaded
    KGameTheme theme;
    //try to load theme
    if (!theme.load(name))
    {
        if (!theme.loadDefault())
            return false;
    }
    g_currentTheme = name;
    g_removeAnimFrameCount = theme.property("RemoveAnimFrames").toInt();
    g_hasBorder = theme.property("HasBorder").toInt() > 0;
    //load graphics
    if (!g_renderer->load(theme.graphics()))
        return false;
    //flush cache
    if (discardCache)
        g_cache->discard();
    return true;
}

void Renderer::boardResized(int width, int height, int leftOffset, int topOffset, int diamondEdgeLength, int diamondCountOnEdge)
{
    //new metrics
    g_sceneWidth = width;
    g_sceneHeight = height;
    g_diamondEdgeLength = diamondEdgeLength;
    int borderEdgeLength = (diamondCountOnEdge + 2.0 * KDiamond::BorderPadding) * diamondEdgeLength;
    //new sizes
    g_diamondSize = QSize(g_diamondEdgeLength, g_diamondEdgeLength);
    g_sceneSize = QSize(g_sceneWidth, g_sceneHeight);
    //pre-render the background (it is more complex than the other pixmaps because it may include the border)
    QString pixName = QString("kdiamond-background_%1-%2").arg(width).arg(height);
    QPixmap pix;
    if (!g_cache->find(pixName, pix))
    {
        pix = QPixmap(g_sceneSize);
        pix.fill(Qt::transparent);
        QPainter p(&pix);
        g_renderer->render(&p, "kdiamond-background");
        if (g_hasBorder)
            g_renderer->render(&p, "kdiamond-border", QRectF(leftOffset - KDiamond::BorderPadding * diamondEdgeLength, topOffset - KDiamond::BorderPadding * diamondEdgeLength, borderEdgeLength, borderEdgeLength));
        p.end();
        g_cache->insert(pixName, pix);
    }
}

int Renderer::removeAnimFrameCount()
{
    return g_removeAnimFrameCount;
}

bool Renderer::hasBorder()
{
    return g_hasBorder;
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
        g_renderer->render(&p, svgName);
        p.end();
        g_cache->insert(pixName, pix);
    }
    return pix;
}

QPixmap Renderer::diamond(KDiamond::Color color)
{
    QString svgName = colorToString(color);
    //cache the move animation for this diamond
    if (color != KDiamond::Selection)
    {
        for (int i = 0; i < g_removeAnimFrameCount; ++i)
        {
            QString frameSvgName = svgName + QString("-%1").arg(i);
            QString framePixName = frameSvgName + QString("_%1-%1").arg(g_diamondEdgeLength);
            QPixmap pix;
            if (!g_cache->find(framePixName, pix))
            {
                pix = QPixmap(g_diamondSize);
                pix.fill(Qt::transparent);
                QPainter p(&pix);
                g_renderer->render(&p, frameSvgName);
                p.end();
                g_cache->insert(framePixName, pix);
            }
        }
    }
    //return the static pixmap
    return pixmapFromCache(svgName, g_diamondSize);
}

QPixmap Renderer::removeFrame(KDiamond::Color color, int frame)
{
    if (frame >= g_removeAnimFrameCount || color == KDiamond::Selection)
        return QPixmap();
    return pixmapFromCache(colorToString(color) + QString("-%1").arg(frame), g_diamondSize);
}

QPixmap Renderer::background()
{
    return pixmapFromCache("kdiamond-background", g_sceneSize);
}
