/***************************************************************************
 *   Copyright (C) 2010 by Kai Dombrowe <just89@gmx.de>                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 ***************************************************************************/


// own
#include "keyboarddock.h"
#include "keywidget.h"
#include "../keymonmanager.h"

// KDE
#include <kauth.h>
#include <kdebug.h>

// Qt
#include <QtCore/QFile>
#include <QtCore/QTimer>
#include <QtGui/QMainWindow>


KeyboardDock::KeyboardDock(QWidget *parent)
    : QDockWidget(parent)
{

    setObjectName("Keyboard");
    setAllowedAreas(Qt::AllDockWidgetAreas);
    setFeatures(QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetFloatable);

    setupUi(this);

    connect(KeyMonManager::self(), SIGNAL(keyEvent(KeyMon::Event)), this,
            SLOT(keyPressed(KeyMon::Event)));
    connect(this, SIGNAL(dockLocationChanged(Qt::DockWidgetArea)), this,
            SLOT(relayout(Qt::DockWidgetArea)));

}


KeyboardDock::~KeyboardDock()
{


}


void KeyboardDock::init(const QList<KeyboardKey> &map)
{

    foreach (KeyWidget *widget, m_keyList) {
        keyLayout->removeWidget(widget);
        delete widget;
    }
    m_keyList.clear();

    foreach (const KeyboardKey &key, map) {
        KeyWidget *widget = new KeyWidget(key.second, key.first, this);
        m_keyList.append(widget);
    }
    relayout(static_cast<QMainWindow*>(parentWidget())->dockWidgetArea(this));

}


void KeyboardDock::keyPressed(const KeyMon::Event &event)
{

    if (event.mouseEvent) {
        return;
    }

    foreach (KeyWidget * widget, m_keyList) {
        if (widget->keyCode() == event.keyCode) {
            kDebug() << "key pressed:" << event.keyCode << event.pressed;
            widget->setPressed(event.pressed);
            break;
        }
    }

}


void KeyboardDock::relayout(const Qt::DockWidgetArea &area)
{

    foreach (KeyWidget *widget, m_keyList) {
        keyLayout->removeWidget(widget);
    }

    foreach (KeyWidget *widget, m_keyList) {
        switch (area) {
        case Qt::LeftDockWidgetArea:
        case Qt::RightDockWidgetArea: keyLayout->addWidget(widget, keyLayout->rowCount(), 0); break;
        case Qt::TopDockWidgetArea:
        case Qt::BottomDockWidgetArea:
        default: keyLayout->addWidget(widget, 0, keyLayout->columnCount()); break;
        }
    }


}


#include "keyboarddock.moc"
