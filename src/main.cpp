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

#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>
#include <KLocale>
#include <KGlobal>

#include "settings.h"
#include "mainwindow.h"
#include "renderer.h"

static const char description[] = I18N_NOOP("KDiamond, a three-in-a-row game.");
static const char version[] = "0.1.1";

int main(int argc, char ** argv)
{
    KAboutData about("kdiamond", 0, ki18nc("The application's name", "KDiamond"), version, ki18n(description),
        KAboutData::License_GPL, ki18n("(C) 2008 Stefan Majewsky"));
    about.addAuthor(ki18n("Stefan Majewsky"), ki18n("Original author and current maintainer"), "majewsky@gmx.net");
    about.addCredit(ki18n("Felix Lemke"), ki18n("First theme"), "lemke.felix@ages-skripte.org");
    about.addCredit(ki18n("Jeffrey Kelling"), ki18n("Technical consultant"), "kelling.jeffrey@ages-skripte.org");
    KCmdLineArgs::init(argc, argv, &about);

    KCmdLineOptions options;
    options.add("VeryEasy", ki18n("Start with Very Easy difficulty level"));
    options.add("Easy", ki18n("Start with Easy difficulty level"));
    options.add("Medium", ki18n("Start with Medium difficulty level"));
    options.add("Hard", ki18n("Start with Hard difficulty level"));
    options.add("VeryHard", ki18n("Start with Very Hard difficulty level"));
    KCmdLineArgs::addCmdLineOptions(options);

    KApplication app;
    KGlobal::locale()->insertCatalog("libkdegames");

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    if (args->isSet("VeryEasy"))
        Settings::setSkill(Settings::EnumSkill::VeryEasy);
    if (args->isSet("Easy"))
        Settings::setSkill(Settings::EnumSkill::Easy);
    if (args->isSet("Medium"))
        Settings::setSkill(Settings::EnumSkill::Medium);
    if (args->isSet("Hard"))
        Settings::setSkill(Settings::EnumSkill::Hard);
    if (args->isSet("VeryHard"))
        Settings::setSkill(Settings::EnumSkill::VeryHard);
    args->clear();

    Renderer::init();
    MainWindow* window = new MainWindow;
    window->show();

    return app.exec();
}
