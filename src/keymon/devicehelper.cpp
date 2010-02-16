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

// c
#include <unistd.h>


ActionReply DeviceHelper::watch(QVariantMap args)
{

    KeyMon::Device device(this, args["Device"].toString());
    if (device.error()) {
        return ActionReply::HelperError;
    }
    connect(&device, SIGNAL(buttonPressed(KeyMon::Event)), this, SLOT(key(KeyMon::Event)));
    connect(&device, SIGNAL(keyPressed(KeyMon::Event)), this, SLOT(key(KeyMon::Event)));

    while (!HelperSupport::isStopped()) {
        usleep(10000);
    }
    return ActionReply::SuccessReply;

}


ActionReply DeviceHelper::name(QVariantMap args)
{

    DeviceData data = KeyMon::Device::getDevice(args["Device"].toString());

    QVariantMap map;
    map["Name"] = data.first;
    map["File"] = data.second;

    ActionReply reply = ActionReply::SuccessReply;
    reply.setData(map);

    return reply;

}


void DeviceHelper::key(const KeyMon::Event &event)
{

    QVariantMap data;
    data["Key"] = event.key;
    data["KeyCode"] = event.keyCode;
    data["Pressed"] = event.pressed;
    HelperSupport::progressStep(data);

}


KDE4_AUTH_HELPER_MAIN("org.kde.recorditnow.helper", DeviceHelper)
