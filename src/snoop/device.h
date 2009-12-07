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

#ifndef DEVICE_H
#define DEVICE_H


// own
#include "event.h"

// KDE
#include <kdemacros.h>

// Qt
#include <QtCore/QObject>


class QSocketNotifier;
namespace SNoop {

class Thread;
class KDE_EXPORT Device : public QObject
{
    Q_OBJECT


public:
    explicit Device(QObject *parent, const QString &file);
    ~Device();

    static QStringList getDeviceList();
    static QString getDeviceName(const QString &file);
    static QString fileForDevice(const QString &device);


private:
    QSocketNotifier *m_socketNotifier;


private slots:
    void readEvents();


signals:
    void buttonPressed(const SNoop::Event &event);
    void finished();


};


}; // Namespace SNoop


#endif // DEVICE_H
