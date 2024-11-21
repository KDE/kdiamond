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
#include <KGameDifficulty>
#include <KDBusService>
#define HAVE_KICONTHEME __has_include(<KIconTheme>)
#if HAVE_KICONTHEME
#include <KIconTheme>
#endif

#define HAVE_STYLE_MANAGER __has_include(<KStyleManager>)
#if HAVE_STYLE_MANAGER
#include <KStyleManager>
#endif
#include <QApplication>
#include <QCommandLineParser>
#include <QStandardPaths>

int main(int argc, char **argv)
{
#if HAVE_KICONTHEME
    KIconTheme::initTheme();
#endif
    QApplication app(argc, argv);
#if HAVE_STYLE_MANAGER
    KStyleManager::initStyle();
#else // !HAVE_STYLE_MANAGER
#if defined(Q_OS_MACOS) || defined(Q_OS_WIN)
    QApplication::setStyle(QStringLiteral("breeze"));
#endif // defined(Q_OS_MACOS) || defined(Q_OS_WIN)
#endif // HAVE_STYLE_MANAGER
    KLocalizedString::setApplicationDomain(QByteArrayLiteral("kdiamond"));

    KAboutData about(QStringLiteral("kdiamond"), i18nc("The application's name", "KDiamond"),
                     QStringLiteral(KDIAMOND_VERSION_STRING),
                     i18n("KDiamond, a three-in-a-row game."),
                     KAboutLicense::GPL, i18n("(C) 2008-2010 Stefan Majewsky and others"),
                     QString(),
                     QStringLiteral("https://apps.kde.org/kdiamond"));
    about.addAuthor(i18n("Stefan Majewsky"), i18n("Original author and current maintainer"), QStringLiteral("majewsky@gmx.net"));
    about.addAuthor(i18n("Paul Bunbury"), i18n("Gameplay refinement"), QStringLiteral("happysmileman@googlemail.com"));
    about.addCredit(i18n("Eugene Trounev"), i18n("Default theme"), QStringLiteral("eugene.trounev@gmail.com"));
    about.addCredit(i18n("Felix Lemke"), i18n("Classic theme"), QStringLiteral("lemke.felix@ages-skripte.org"));
    about.addCredit(i18n("Jeffrey Kelling"), i18n("Technical consultant"), QStringLiteral("kelling.jeffrey@ages-skripte.org"));

    KAboutData::setApplicationData(about);
    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("kdiamond")));

    KCrash::initialize();

    QCommandLineParser parser;
    about.setupCommandLine(&parser);
    parser.process(app);
    about.processCommandLine(&parser);

    //resource directory for KNewStuff2 (this call causes the directory to be created; its existence is necessary for the downloader)
    QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QLatin1String("/themes/");

    KGameDifficulty::global()->addStandardLevelRange(
        KGameDifficultyLevel::VeryEasy,
        KGameDifficultyLevel::VeryHard
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
