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

#include <QPainter>
#include <QPixmap>
#include <KPixmapCache>
#include <KStandardDirs>

KPixmapCache *g_cache;

void Renderer::init()
{
    g_cache = new KPixmapCache("kdiamond-pixmaps");
    g_cache->setCacheLimit(2048); //2 megabytes should be enough and not too big
}

void Renderer::drawDiamond(QPainter *painter, const QRectF &bounds, KDiamond::Color color)
{
    QPixmap pixmap;
    QSize size = bounds.size().toSize();
    switch (color)
    {
        case KDiamond::RedDiamond:
            pixmap = g_cache->loadFromSvg(KStandardDirs::locate("data", "kdiamond/kdiamond-red.svg"), size);
            break;
        case KDiamond::GreenDiamond:
            pixmap = g_cache->loadFromSvg(KStandardDirs::locate("data", "kdiamond/kdiamond-green.svg"), size);
            break;
        case KDiamond::BlueDiamond:
            pixmap = g_cache->loadFromSvg(KStandardDirs::locate("data", "kdiamond/kdiamond-blue.svg"), size);
            break;
        case KDiamond::YellowDiamond:
            pixmap = g_cache->loadFromSvg(KStandardDirs::locate("data", "kdiamond/kdiamond-yellow.svg"), size);
            break;
        case KDiamond::WhiteDiamond:
            pixmap = g_cache->loadFromSvg(KStandardDirs::locate("data", "kdiamond/kdiamond-white.svg"), size);
            break;
        case KDiamond::BlackDiamond:
            pixmap = g_cache->loadFromSvg(KStandardDirs::locate("data", "kdiamond/kdiamond-black.svg"), size);
            break;
        case KDiamond::OrangeDiamond:
            pixmap = g_cache->loadFromSvg(KStandardDirs::locate("data", "kdiamond/kdiamond-orange.svg"), size);
            break;
    }
    painter->drawPixmap(bounds, pixmap, QRectF(0.0, 0.0, bounds.width(), bounds.height()));
}

void Renderer::drawBackground(QPainter *painter, const QRectF &bounds)
{
    QPixmap pixmap = g_cache->loadFromSvg(KStandardDirs::locate("data", "kdiamond/kdiamond-background.svg"), bounds.size().toSize());
    painter->drawPixmap(bounds, pixmap, QRectF(0.0, 0.0, bounds.width(), bounds.height()));
}

void Renderer::drawShadow(QPainter *painter, const QRectF &bounds)
{
    QPixmap pixmap = g_cache->loadFromSvg(KStandardDirs::locate("data", "kdiamond/kdiamond-selected.svg"), bounds.size().toSize());
    painter->drawPixmap(bounds, pixmap, QRectF(0.0, 0.0, bounds.width(), bounds.height()));
}
