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

#include "mainwindow.h"
#include "renderer.h"
#include "settings.h"

#include <QDir>
#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>
#include <KDebug>
#include <KGameDifficulty>
#include <KLocale>
#include <KGlobal>
#include <KShell>
#include <KStandardDirs>

static const char description[] = I18N_NOOP("KDiamond, a three-in-a-row game.");
static const char version[] = "1.0";

int main(int argc, char ** argv)
{
    KAboutData about("kdiamond", 0, ki18nc("The application's name", "KDiamond"), version, ki18n(description),
        KAboutData::License_GPL, ki18n("(C) 2008 Stefan Majewsky"));
    about.addAuthor(ki18n("Stefan Majewsky"), ki18n("Original author and current maintainer"), "majewsky@gmx.net");
    about.addCredit(ki18n("Eugene Trounev"), ki18n("Default theme"), "eugene.trounev@gmail.com");
    about.addCredit(ki18n("Felix Lemke"), ki18n("Classic theme"), "lemke.felix@ages-skripte.org");
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
    //resource directory for KNewStuff2
    KGlobal::dirs()->addResourceDir("data", KShell::tildeExpand(QLatin1String("~/.kdiamond/")));
    //make sure that the themes directory exists (KNewStuff2 downloading won't work if it does not exist)
    const QString themesDirectory = KShell::tildeExpand(QLatin1String("~/.kdiamond/kdiamond/themes/"));
    const QDir themesDir(themesDirectory);
    if (!themesDir.exists())
    {
        if (!themesDir.mkpath(themesDirectory))
            kWarning() << i18n("Could not create the themes directory at: %1", themesDirectory);
    }

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    if (args->isSet("VeryEasy"))
        Settings::setSkill(KGameDifficulty::VeryEasy);
    if (args->isSet("Easy"))
        Settings::setSkill(KGameDifficulty::Easy);
    if (args->isSet("Medium"))
        Settings::setSkill(KGameDifficulty::Medium);
    if (args->isSet("Hard"))
        Settings::setSkill(KGameDifficulty::Hard);
    if (args->isSet("VeryHard"))
        Settings::setSkill(KGameDifficulty::VeryHard);
    args->clear();

    Renderer::self(); //causes the static Renderer object to be created and initiated
    MainWindow* window = new MainWindow;
    window->show();

    return app.exec();
}
