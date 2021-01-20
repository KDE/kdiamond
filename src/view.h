/*
    SPDX-FileCopyrightText: 2008-2010 Stefan Majewsky <majewsky@gmx.net>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KDIAMOND_VIEW_H
#define KDIAMOND_VIEW_H

#include <QGraphicsView>

namespace KDiamond
{

class View : public QGraphicsView
{
public:
    explicit View(QWidget *parent = nullptr);

    void setScene(QGraphicsScene *scene);
protected:
    void resizeEvent(QResizeEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
};

}

#endif // KDIAMOND_VIEW_H
