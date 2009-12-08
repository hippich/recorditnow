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


// Qt
#include <QtCore/QObject>


class Device : public QObject
{
    Q_OBJECT


public:
    enum Error {
        ERR_NOERROR = 0,
        ERR_NOSUCHFILE = 1,
        ERR_PERMDENIED = 2,
        ERR_GROUPADDNOTFOUND = 3,
        ERR_INTERNALERROR = 4,
        ERR_GPASSWDNOTFOUND = 5,
        ERR_NOUSER = 6,
        ERR_OPENFAILED = 7,
        ERR_WRITEFAILED = 8,
        ERR_CHMODNOTFOUND = 9,
        ERR_CHMODFIALED = 10
    };

    explicit Device(const QString &device, QObject *parent = 0);
    ~Device();


private:
    QString m_device;


private slots:
    void fix();


protected:
    void exit(const Device::Error &code);



};


#endif // DEVICE_H
