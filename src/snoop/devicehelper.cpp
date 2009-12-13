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

// KDE
#include <kdebug.h>

// c
#include <unistd.h>


ActionReply DeviceHelper::watch(QVariantMap args)
{

    m_device = new SNoop::Device(this, args["Device"].toString());
    if (!m_device) {
        return ActionReply::HelperError;
    }
    connect(m_device, SIGNAL(buttonPressed(SNoop::Event)), this, SLOT(key(SNoop::Event)));

    while (!HelperSupport::isStopped()) {
        usleep(10000);
    }
    delete m_device;
    return ActionReply::SuccessReply;

}


ActionReply DeviceHelper::name(QVariantMap args)
{

    DeviceData data = SNoop::Device::getDevice(args["Device"].toString());

    QVariantMap map;
    map["Name"] = data.first;
    map["File"] = data.second;

    ActionReply reply = ActionReply::SuccessReply;
    reply.setData(map);

    return reply;

}


void DeviceHelper::key(const SNoop::Event &event)
{

    QVariantMap data;
    data["Key"] = event.key;
    data["Pressed"] = event.pressed;
    HelperSupport::progressStep(data);

}


KDE4_AUTH_HELPER_MAIN("org.kde.recorditnow.helper", DeviceHelper)
