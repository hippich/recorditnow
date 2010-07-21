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
#include "devicesearchdialog.h"
#include "devicesearchwidget.h"

// KDE
#include <klocalizedstring.h>
#include <kmessagebox.h>

// Qt
#include <QtGui/QTreeWidgetItem>


DeviceSearchDialog::DeviceSearchDialog(const KeyMon::DeviceInfo::DeviceType &type, QWidget *parent)
    : KDialog(parent)
{

    RecordItNow::DeviceSearchWidget *main = new RecordItNow::DeviceSearchWidget(this);
    setMainWidget(main);

    setAttribute(Qt::WA_DeleteOnClose);
    resize(500, 300);

    main->search(type);

    switch (type) {
    case KeyMon::DeviceInfo::MouseType: setWindowTitle(i18n("Mouse"));  break;
    case KeyMon::DeviceInfo::KeyboardType: setWindowTitle(i18n("Keyboard")); break;
    default: break;
    }

    connect(this, SIGNAL(finished(int)), this, SLOT(dialogFinished(int)));


    if (main->deviceCount() == 0) {
        KMessageBox::information(this, i18n("No devices found."));
        reject();
    }

}


void DeviceSearchDialog::dialogFinished(const int &ret)
{

    if (ret == KDialog::Accepted) {
        QString device = static_cast<RecordItNow::DeviceSearchWidget*>(mainWidget())->selectedDevice();
        if (device.isEmpty()) {
            return;
        }
        emit deviceSelected(device);
    }

}


#include "devicesearchdialog.moc"
