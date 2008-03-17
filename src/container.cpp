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

#include "container.h"

Container::Container(QWidget *parent)
    : QWidget(parent)
    , m_widget(0)
{
}

void Container::setWidget(QWidget *widget)
{
    m_widget = widget;
    m_widget->setParent(this);
    m_widget->setGeometry(0, 0, width(), height());
    m_widget->show();
}

void Container::resizeEvent(QResizeEvent *)
{
    m_widget->setGeometry(0, 0, width(), height());
}

#include "container.moc"
