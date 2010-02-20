/***************************************************************************
 *   Copyright (C) 2009 by Kai Dombrowe <just89@gmx.de>                    *
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
#include "devicehelper.h"
#include "device.h"

// KDE
#include <kdebug.h>

// Qt
#include <QtCore/QTimer>
#include <QtCore/QStringList>

// c
#include <unistd.h>


ActionReply DeviceHelper::watch(QVariantMap args)
{

    kDebug() << "watch...";
    QTimer timer(this);
    connect(&timer, SIGNAL(timeout()), this, SLOT(timeout()));
    timer.start(500);

    QList<KeyMon::Device*> devs;
    foreach (const QString &dev, args.value("Devs").toStringList()) {
        KeyMon::Device *device = new KeyMon::Device(this, dev);
        if (device->error()) {
            kDebug() << "error";
            delete device;
            return ActionReply::HelperError;
        }
        connect(device, SIGNAL(buttonPressed(KeyMon::Event)), this, SLOT(key(KeyMon::Event)));
        connect(device, SIGNAL(keyPressed(KeyMon::Event)), this, SLOT(key(KeyMon::Event)));

        devs.append(device);
    }

    kDebug() << "start...";

    m_loop.exec();

    qDeleteAll(devs);

    kDebug() << "done...";
    return ActionReply::SuccessReply;

}


ActionReply DeviceHelper::inputdevicelist(QVariantMap args)
{

    QVariantMap result;

    QVariant var;
    var.setValue(KeyMon::DeviceInfo::toArray(KeyMon::Manager::getInputDeviceList()));

    result["List"] = var;

    ActionReply reply = ActionReply::SuccessReply;
    reply.setData(result);

    return reply;

}


void DeviceHelper::key(const KeyMon::Event &event)
{

    QVariantMap data;
    data["Key"] = event.key;
    data["KeyCode"] = event.keyCode;
    data["Pressed"] = event.pressed;
    data["MouseEvent"] = event.mouseEvent;
    HelperSupport::progressStep(data);

}


void DeviceHelper::timeout()
{

    if (HelperSupport::isStopped()) {
        kDebug() << "isStopped = true";
        m_loop.quit();
    }

}


KDE4_AUTH_HELPER_MAIN("org.kde.recorditnow.helper", DeviceHelper)
