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

#ifndef DEVICEHELPER_H
#define DEVICEHELPER_H


// own
#include "device.h"
#include "event.h"

// KDE
#include <kauth.h>

// Qt
#include <QtCore/QObject>


using namespace KAuth;
class DeviceHelper : public QObject
{
    Q_OBJECT


public slots:
    ActionReply watch(QVariantMap args);
    ActionReply name(QVariantMap args);


private:
    SNoop::Device *m_device;


private slots:
    void key(const SNoop::Event &event);


};


#endif // DEVICEHELPER_H
