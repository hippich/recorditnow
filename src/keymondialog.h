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


#ifndef KEYMONDIALOG_H
#define KEYMONDIALOG_H


// own
#include "ui_keymondialog.h"

// KDE
#include <kdialog.h>
#include <kprocess.h>


typedef QPair<QString, QString> DeviceData;
class QTreeWidgetItem;
class KeyMonDialog : public KDialog, public Ui::KeyMonDialog
{
    Q_OBJECT


public:
    explicit KeyMonDialog(QWidget *parent = 0);
    ~KeyMonDialog();


private:
    QTreeWidgetItem *newDeviceItem(const DeviceData *d);


private slots:
    void dialogFinished(const int &ret);
    void loadDeviceList();
    void loadDeviceList2();
    void updateStatus();


signals:
    void deviceSelected(const QString &id);


};


#endif // KEYMONDIALOG_H
