/*
    SPDX-FileCopyrightText: 2008-2009 Stefan Majewsky <majewsky@gmx.net>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KDIAMOND_INFOBAR_H
#define KDIAMOND_INFOBAR_H

#include <QObject>
class QStatusBar;
class QLabel;
namespace KDiamond
{

class InfoBar : public QObject
{
    Q_OBJECT
public:
    explicit InfoBar(QStatusBar *bar);
public Q_SLOTS:
    void setUntimed(bool untimed);
    void updatePoints(int points);
    void updateMoves(int moves);
    void updateRemainingTime(int remainingSeconds);
private:
    bool m_untimed;
    QStatusBar *m_bar;
    QLabel *mMovement;
    QLabel *mPoints;
    QLabel *mTime;

};

}

#endif // KDIAMOND_INFOBAR_H
