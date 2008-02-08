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

#include "greeter.h"

#include <QGridLayout>
#include <QLabel>
#include <KLocalizedString>
#include <KPushButton>

Greeter::Greeter(QWidget *parent)
    : QWidget(parent)
{
    m_button = new KPushButton(KIcon("document-new"), i18n("Start new game"), this);
    connect(m_button, SIGNAL(pressed()), this, SIGNAL(startGame()));
    m_label = new QLabel(i18n("You can choose a difficulty level from the box at the right bottom of the window."), this);
    m_label->setAlignment(Qt::AlignCenter);
    m_label->setWordWrap(true);
    //layout
    m_layout = new QGridLayout;
    m_layout->addWidget(m_button, 1, 1);
    m_layout->addWidget(m_label, 2, 0, 1, 3);
    m_layout->setRowStretch(0, 10);
    m_layout->setRowStretch(3, 10);
    setLayout(m_layout);
}

Greeter::~Greeter()
{
    delete m_layout;
    delete m_button;
    delete m_label;
}

#include "greeter.moc"
