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

#ifndef DEVICESEARCHWIDGET_H
#define DEVICESEARCHWIDGET_H


// own
#include "ui_devicesearchwidget.h"
#include "keymon/deviceinfo.h"


namespace RecordItNow {


class DeviceSearchWidget : public QWidget, Ui::DeviceSearchWidget
{
    Q_OBJECT


public:
    explicit DeviceSearchWidget(QWidget *parent = 0);
    ~DeviceSearchWidget();

    int deviceCount() const;
    QString selectedDevice() const;

    void search(const KeyMon::DeviceInfo::DeviceType &type);


private slots:
    void itemSelectionChanged();


signals:
    void deviceSelectionChanged(const QString &device);


};


} // namespace RecordItNow


#endif // DEVICESEARCHWIDGET_H
