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



namespace KeyMon {


Device::Device(QObject *parent, const QString &file, const bool &mouse)
    : QObject(parent), m_watchMouse(mouse)
{

    qRegisterMetaType<KeyMon::Event>("KeyMon::Event");

    m_socketNotifier = 0;
    int fd = open(file.toLatin1(), O_RDONLY|O_NONBLOCK);
    if (fd == -1) {
        kWarning() << "open failed!";
        m_error = true;
        return;
    }
    m_error = false;

    m_socketNotifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);
    connect(m_socketNotifier, SIGNAL(activated(int)), this, SLOT(readEvents()));

}


Device::~Device()
{

    if (m_socketNotifier) {
        delete m_socketNotifier;
    }

}


bool Device::error() const
{

    return m_error;

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
        if (bytesRead <= 0) {
            break;
        }
        if (bytesRead != sizeof(ev)) {
            kWarning() << "Internal error!";
            return;
        }

        if (m_watchMouse) {
            const bool pressed = (ev.value == 1);
            KeyMon::Event::Key key;
            switch(ev.code)
            {
            case BTN_LEFT:
                key = KeyMon::Event::LeftButton;
                break;
            case BTN_RIGHT:
                key = KeyMon::Event::RightButton;
                break;
            case BTN_MIDDLE:
                key = KeyMon::Event::MiddleButton;
                break;
            case BTN_EXTRA:
                key = KeyMon::Event::SpecialButton1;
                break;
            case BTN_SIDE:
                key = KeyMon::Event::SpecialButton2;
                break;
            case REL_WHEEL:
                if (pressed) {
                    key = KeyMon::Event::WheelUp;
                } else {
                    key = KeyMon::Event::WheelDown;
                }
                break;
            default:
                key = KeyMon::Event::NoButton;
                break;
            };

            if (key != KeyMon::Event::NoButton) {
                KeyMon::Event sEvent;
                sEvent.key = key;
                sEvent.pressed = pressed;
                emit buttonPressed(sEvent);
            }
        } else { // keyboard
            if (ev.type != EV_KEY) {
                continue;
            }

            const bool pressed = (ev.value == 1 || ev.value == 2);

            KeyMon::Event event;
            event.keyCode = ev.code;
            event.pressed = pressed;

            emit keyPressed(event);
        }
    }

}


}; // Namespace KeyMon


#include "device.moc"
