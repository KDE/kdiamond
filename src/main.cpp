/*
    SPDX-FileCopyrightText: 2008-2009 Stefan Majewsky <majewsky@gmx.net>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "mainwindow.h"
#include "settings.h"
#include "kdiamond_version.h"

#include <ctime>

#include <KAboutData>
#include <KCrash>

#include <KLocalizedString>

#include <KgDifficulty>
#include <kdelibs4configmigrator.h>
#include <QApplication>
#include <QCommandLineParser>
#include <QStandardPaths>
#include <KDBusService>

int main(int argc, char **argv)
{
    // Fixes blurry icons with fractional scaling
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication app(argc, argv);
    Kdelibs4ConfigMigrator migrate(QStringLiteral("kdiamond"));
    migrate.setConfigFiles(QStringList() << QStringLiteral("kdiamondrc") << QStringLiteral("kdiamond.notifyrc"));
    migrate.setUiFiles(QStringList() << QStringLiteral("kdiamondui.rc"));
    migrate.migrate();

    KLocalizedString::setApplicationDomain("kdiamond");
    KAboutData about(QStringLiteral("kdiamond"), i18nc("The application's name", "KDiamond"),
                     QStringLiteral(KDIAMOND_VERSION_STRING),
                     i18n("KDiamond, a three-in-a-row game."),
                     KAboutLicense::GPL, i18n("(C) 2008-2010 Stefan Majewsky and others"),
                     QString(),
                     QStringLiteral("https://kde.org/applications/games/org.kde.kdiamond"));
    about.addAuthor(i18n("Stefan Majewsky"), i18n("Original author and current maintainer"), QStringLiteral("majewsky@gmx.net"));
    about.addAuthor(i18n("Paul Bunbury"), i18n("Gameplay refinement"), QStringLiteral("happysmileman@googlemail.com"));
    about.addCredit(i18n("Eugene Trounev"), i18n("Default theme"), QStringLiteral("eugene.trounev@gmail.com"));
    about.addCredit(i18n("Felix Lemke"), i18n("Classic theme"), QStringLiteral("lemke.felix@ages-skripte.org"));
    about.addCredit(i18n("Jeffrey Kelling"), i18n("Technical consultant"), QStringLiteral("kelling.jeffrey@ages-skripte.org"));
    QCommandLineParser parser;
    KAboutData::setApplicationData(about);
    KCrash::initialize();
    about.setupCommandLine(&parser);
    parser.process(app);
    about.processCommandLine(&parser);

    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("kdiamond")));

    //resource directory for KNewStuff2 (this call causes the directory to be created; its existence is necessary for the downloader)
    QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QLatin1String("/themes/");

    Kg::difficulty()->addStandardLevelRange(
        KgDifficultyLevel::VeryEasy,
        KgDifficultyLevel::VeryHard
    );
    KDBusService service;
    // see if we are starting with session management
    if (app.isSessionRestored()) {
        kRestoreMainWindows<MainWindow>();
    } else {
        MainWindow *window = new MainWindow;
        window->show();
    }
    return app.exec();
}
