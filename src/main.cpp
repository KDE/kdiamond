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

#include "mainwindow.h"
#include "settings.h"

#include <ctime>

#include <KAboutData>

#include <KLocalizedString>

#include <KgDifficulty>
#include <kdelibs4configmigrator.h>
#include <QApplication>
#include <QCommandLineParser>
#include <QStandardPaths>
#include <KDBusService>

static const char description[] = I18N_NOOP("KDiamond, a three-in-a-row game.");
static const char version[] = "1.5";

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    Kdelibs4ConfigMigrator migrate(QStringLiteral("kdiamond"));
    migrate.setConfigFiles(QStringList() << QStringLiteral("kdiamondrc") << QStringLiteral("kdiamond.notifyrc"));
    migrate.setUiFiles(QStringList() << QStringLiteral("kdiamondui.rc"));
    migrate.migrate();

    qsrand(time(0));
    KLocalizedString::setApplicationDomain("kdiamond");
    KAboutData about(QStringLiteral("kdiamond"), i18nc("The application's name", "KDiamond"), QLatin1Literal(version), i18n(description),
                     KAboutLicense::GPL, i18n("(C) 2008-2010 Stefan Majewsky and others"), QStringLiteral("http://games.kde.org/kdiamond"));
    about.addAuthor(i18n("Stefan Majewsky"), i18n("Original author and current maintainer"), QStringLiteral("majewsky@gmx.net"));
    about.addAuthor(i18n("Paul Bunbury"), i18n("Gameplay refinement"), QStringLiteral("happysmileman@googlemail.com"));
    about.addCredit(i18n("Eugene Trounev"), i18n("Default theme"), QStringLiteral("eugene.trounev@gmail.com"));
    about.addCredit(i18n("Felix Lemke"), i18n("Classic theme"), QStringLiteral("lemke.felix@ages-skripte.org"));
    about.addCredit(i18n("Jeffrey Kelling"), i18n("Technical consultant"), QStringLiteral("kelling.jeffrey@ages-skripte.org"));
    QCommandLineParser parser;
    KAboutData::setApplicationData(about);
    parser.addVersionOption();
    parser.addHelpOption();
    about.setupCommandLine(&parser);
    parser.process(app);
    about.processCommandLine(&parser);

    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("kdiamond")));

    //resource directory for KNewStuff2 (this call causes the directory to be created; its existence is necessary for the downloader)
    QStandardPaths::writableLocation(QStandardPaths::DataLocation) + QLatin1String("/themes/");

    Kg::difficulty()->addStandardLevelRange(
        KgDifficultyLevel::VeryEasy,
        KgDifficultyLevel::VeryHard
    );
    KDBusService service;
    // see if we are starting with session management
    if (app.isSessionRestored()) {
        RESTORE(MainWindow);
    } else {
        MainWindow *window = new MainWindow;
        window->show();
    }
    return app.exec();
}
