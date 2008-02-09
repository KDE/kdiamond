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

#include <KStandardDirs>

//TODO: This is just an intermediate solution which needs to see further performance improvements.

QSvgRenderer *g_background;
QSvgRenderer **g_diamonds;

void Renderer::init()
{
    g_background = new QSvgRenderer(KStandardDirs::locate("data", "kdiamond/kdiamond-background.svg"));
    g_diamonds = new QSvgRenderer*[8]; //index 0 = shadow, 1 - 7 = diamonds
    g_diamonds[0] = new QSvgRenderer(KStandardDirs::locate("data", "kdiamond/kdiamond-selected.svg"));
    g_diamonds[1] = new QSvgRenderer(KStandardDirs::locate("data", "kdiamond/kdiamond-red.svg"));
    g_diamonds[2] = new QSvgRenderer(KStandardDirs::locate("data", "kdiamond/kdiamond-green.svg"));
    g_diamonds[3] = new QSvgRenderer(KStandardDirs::locate("data", "kdiamond/kdiamond-blue.svg"));
    g_diamonds[4] = new QSvgRenderer(KStandardDirs::locate("data", "kdiamond/kdiamond-yellow.svg"));
    g_diamonds[5] = new QSvgRenderer(KStandardDirs::locate("data", "kdiamond/kdiamond-white.svg"));
    g_diamonds[6] = new QSvgRenderer(KStandardDirs::locate("data", "kdiamond/kdiamond-black.svg"));
    g_diamonds[7] = new QSvgRenderer(KStandardDirs::locate("data", "kdiamond/kdiamond-orange.svg"));
    //No cleanup is performed because these objects life until the end of the application. Their memory is then freed up by the kernel.
}

QSvgRenderer *Renderer::diamond(KDiamond::Color color)
{
    return g_diamonds[(int) color];
}

QSvgRenderer *Renderer::background()
{
    return g_background;
}

QSvgRenderer *Renderer::shadow()
{
    return g_diamonds[0];
}
