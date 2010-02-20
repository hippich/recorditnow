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
#include "devicemanager.h"

// KDE
#include <kdebug.h>

// Qt
#include <QtCore/QFile>
#include <QtCore/QDir>

// C
#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>


QList<DeviceManager::DeviceData> DeviceManager::getInputDeviceList()
{

    const QDir dir("/dev/input/by-id");
    QList<DeviceData> list;

    foreach (const QFileInfo &info, dir.entryInfoList(QStringList(), QDir::Files)) {
        if (!info.isSymLink()) {
            continue;
        }

        int fd;
        if ((fd = open(info.symLinkTarget().toLatin1(), O_RDONLY)) == -1) {
            kWarning() << info.symLinkTarget() << ": open failed!";
        } else {
            char buff[32];
            ioctl(fd, EVIOCGNAME(sizeof(buff)), buff);

            DeviceData device;
            device.file = info.symLinkTarget();
            device.name = QString(buff);
            device.uuid = info.absoluteFilePath();

            list.append(device);
        }
    }

    return list;

}



