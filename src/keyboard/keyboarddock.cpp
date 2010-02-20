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


KeyboardDock::KeyboardDock(QWidget *parent)
    : QDockWidget(parent), m_grab(false)
{

    setObjectName("Keyboard");
    setAllowedAreas(Qt::AllDockWidgetAreas);
    setFeatures(QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetFloatable);

    setupUi(this);

    connect(KeyMonManager::self(), SIGNAL(keyEvent(KeyMon::Event)), this,
            SLOT(keyPressed(KeyMon::Event)));

}


KeyboardDock::~KeyboardDock()
{

    stop();

}


void KeyboardDock::init(const QHash<int, QString> &map)
{

    QHashIterator<int, KeyWidget*> i(m_keys);
    while (i.hasNext()) {
        i.next();
        keyLayout->removeWidget(i.value());
        delete i.value();
    }
    m_keys.clear();

    QHashIterator<int, QString> it(map);
    while (it.hasNext()) {
        it.next();
        KeyWidget *widget = new KeyWidget(it.value(), this);
        m_keys[it.key()] = widget;
        keyLayout->addWidget(widget);
    }

}


void KeyboardDock::start(const QString &file)
{

    kDebug() << "start:" << file;

  /*  KAuth::Action action("org.kde.recorditnow.helper.watch");
    action.setHelperID("org.kde.recorditnow.helper");
    connect(action.watcher(), SIGNAL(progressStep(QVariantMap)), this,
            SLOT(progressStep(QVariantMap)));

    QVariantMap args;
    args["Device"] = file;
    args["Keyboard"] = true;

    action.setArguments(args);
    action.setExecutesAsync(true);

    KAuth::ActionReply reply = action.execute("org.kde.recorditnow.helper");
    if (reply.errorCode() != KAuth::ActionReply::NoError) {
        kDebug() << "Grab failed!";
    } else {
        kDebug() << "Grab started...";
    }

    m_grab = true;
*/


}


void KeyboardDock::stop()
{
/*
    if (m_grab) {
        kDebug() << "stopping...";

        KAuth::Action action("org.kde.recorditnow.helper.watch");
        action.setHelperID("org.kde.recorditnow.helper");
        action.watcher()->disconnect(this);
        action.stop();

        m_grab = false;
    }
*/
}


void KeyboardDock::progressStep(const QVariantMap &data)
{

    KeyMon::Event event;
    event.keyCode = data.value("KeyCode").toInt();
    event.pressed = data.value("Pressed").toBool();

    keyPressed(event);

}


void KeyboardDock::keyPressed(const KeyMon::Event &event)
{

    kDebug() << "key pressed:" << event.keyCode << event.pressed;
    if (!m_keys.contains(event.keyCode) || event.mouseEvent) {
        return;
    }
    m_keys.value(event.keyCode)->setPressed(event.pressed);

}



#include "keyboarddock.moc"
