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
#include "device.h"

// KDE
#include <kdebug.h>

// Qt
#include <QtCore/QSocketNotifier>
#include <QtCore/QMetaType>
#include <QtCore/QDir>


// C
#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>



namespace SNoop {


Device::Device(QObject *parent, const QString &file)
    : QObject(parent)
{

    qRegisterMetaType<SNoop::Event>("SNoop::Event");

    int fd = open(file.toLatin1(), O_RDONLY|O_NONBLOCK);
    m_socketNotifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);
    connect(m_socketNotifier, SIGNAL(activated(int)), this, SLOT(readEvents()));

}


Device::~Device()
{

    if (m_socketNotifier) {
        delete m_socketNotifier;
    }

}


QList<DeviceData> Device::getDeviceList()
{

    QList<DeviceData> deviceList;
    QDir input("/dev/input");
    if (!input.exists()) {
        kWarning() << "/dev/input: no such directory!";
        return deviceList;
    }

    QStringList files = input.entryList(QDir::System);
    const QRegExp rx("^event[0-9]+$");

    foreach (const QString &file, files) {
        if (rx.exactMatch(file)) {                        
            const DeviceData device = getDevice(file);
            if (!device.first.isEmpty() && !device.second.isEmpty()) {
                deviceList.append(device);
            }
        }
    }
    return deviceList;

}


DeviceData Device::getDevice(const QString &file)
{

    QString path = file;
    DeviceData device;
    int fd;

    if (!path.startsWith(QLatin1Char('/'))) {
        path.prepend("/dev/input/");
    }

    if ((fd = open (path.toLatin1(), O_RDONLY)) == -1) {
        kWarning() << path << ": open failed!";
    } else {
        char buff[32];
        ioctl(fd, EVIOCGNAME(sizeof(buff)), buff);

        device.first = QString(buff);
    }
    device.second = path;

    return device;

}


void Device::readEvents()
{

    int fd = m_socketNotifier->socket();
    for (;;) {
        struct input_event ev;
        int bytesRead = read(fd, &ev, sizeof(ev));
        if (bytesRead <= 0)
            break;
        if (bytesRead != sizeof(ev)) {
            kWarning("Internal error!");
            return;
        }

        int size = sizeof (struct input_event);
        const bool pressed = (ev.value == 1);
        SNoop::Event::Key key;
        switch(ev.code)
        {
        case BTN_LEFT:
            key = SNoop::Event::LeftButton;
            break;
        case BTN_RIGHT:
            key = SNoop::Event::RightButton;
            break;
        case BTN_MIDDLE:
            key = SNoop::Event::MiddleButton;
            break;
        case BTN_EXTRA:
            key = SNoop::Event::SpecialButton1;
            break;
        case BTN_SIDE:
            key = SNoop::Event::SpecialButton2;
            break;
        case REL_WHEEL:
            if (pressed) {
                key = SNoop::Event::WheelUp;
            } else {
                key = SNoop::Event::WheelDown;
            }
            break;
            default:
            key = SNoop::Event::NoButton;
            break;
        };

        if (key != SNoop::Event::NoButton) {
            SNoop::Event sEvent;
            sEvent.key = key;
            sEvent.pressed = pressed;
            emit buttonPressed(sEvent);
        }
    }

}


}; // Namespace SNoop


#include "device.moc"
