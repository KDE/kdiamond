/*
    SPDX-FileCopyrightText: 2008-2009 Stefan Majewsky <majewsky@gmx.net>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "infobar.h"
#include "settings.h"

#include <KLocalizedString>
#include <QStatusBar>
#include <QLabel>

KDiamond::InfoBar::InfoBar(QStatusBar *bar)
    : m_untimed(Settings::untimed())
    , m_bar(bar)
{
    mMovement = new QLabel(i18n("Possible moves: %1", 0));
    mPoints = new QLabel(i18n("Points: %1", 0));
    mTime = new QLabel;
    if (m_untimed) {
        mTime->setText(i18n("Untimed game"));
    } else {
        mTime->setText(i18n("Time left: %1", QLatin1String("0:00")));
    }
    m_bar->addPermanentWidget(mPoints);
    m_bar->addPermanentWidget(mTime);
    m_bar->addPermanentWidget(mMovement);
    m_bar->show();
}

void KDiamond::InfoBar::setUntimed(bool untimed)
{
    if (untimed) {
        mTime->setText(i18n("Untimed game"));
    }
    m_untimed = untimed;
}

void KDiamond::InfoBar::updatePoints(int points)
{
    mPoints->setText(i18n("Points: %1", points));
}

void KDiamond::InfoBar::updateMoves(int moves)
{
    if (moves == -1) {
        mMovement->setText(i18nc("Shown when the board is in motion.", "Possible moves: …"));
    } else {
        mMovement->setText(i18n("Possible moves: %1", moves));
    }
}

void KDiamond::InfoBar::updateRemainingTime(int remainingSeconds)
{
    if (m_untimed) {
        return;
    }
    //split time in seconds and minutes
    const int seconds = remainingSeconds % 60;
    const int minutes = remainingSeconds / 60;
    //compose new string
    QString secondString = QString::number(seconds);
    const QString minuteString = QString::number(minutes);
    if (seconds < 10) {
        secondString.prepend(QLatin1Char('0'));
    }
    mTime->setText(i18n("Time left: %1", QStringLiteral("%1:%2").arg(minuteString).arg(secondString)));
    //special treatment if game is finished
    if (remainingSeconds == 0) {
        updateMoves(0);
    }
}

#include "moc_infobar.cpp"
