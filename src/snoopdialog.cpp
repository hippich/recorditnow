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
#include "snoopdialog.h"
#include "snoop/device.h"

// KDE
#include <kdebug.h>

// Qt
#include <QtGui/QTreeWidget>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusConnectionInterface>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusInterface>


SNoopDialog::SNoopDialog(QWidget *parent)
    : KDialog(parent)
{

    setWindowTitle(i18n("Sound Device"));
    setAttribute(Qt::WA_DeleteOnClose);
    QWidget *widget = new QWidget(this);
    setupUi(widget);
    setMainWidget(widget);

    connect(this, SIGNAL(finished(int)), this, SLOT(dialogFinished(int)));

    resize(600, 300);

    loadDeviceList();

}


SNoopDialog::~SNoopDialog()
{



}




void SNoopDialog::dialogFinished(const int &ret)
{

    if (ret == KDialog::Accepted) {
        QList<QTreeWidgetItem*> items = treeWidget->selectedItems();
        if (!items.isEmpty()) {
            emit deviceSelected(items[0]->text(1));
        }
    }

}


void SNoopDialog::loadDeviceList()
{

    treeWidget->clear();
    QDBusInterface interface("org.freedesktop.Hal",
                             "/org/freedesktop/Hal/Manager",
                             "org.freedesktop.Hal.Manager",
                             QDBusConnection::systemBus());


    QDBusMessage reply = interface.call("FindDeviceByCapability", "input.mouse");
    if (!reply.errorName().isEmpty() || !reply.errorMessage().isEmpty() || reply.arguments().isEmpty()) {
        loadDeviceList2();
    } else {
        QStringList files;
        foreach (const QString &uuid, reply.arguments().first().toStringList()) {
            kDebug() << "uuid:" << uuid;
            QDBusInterface devInterface("org.freedesktop.Hal",
                                           uuid,
                                           "org.freedesktop.Hal.Device",
                                           QDBusConnection::systemBus());
            reply = devInterface.call("GetProperty", "input.device");
            if (reply.errorName().isEmpty() && reply.errorMessage().isEmpty()) {
                const DeviceData data = SNoop::Device::getDevice(reply.arguments().first().toString());
                if (!data.first.isEmpty() || data.second.isEmpty()) {
                    QTreeWidgetItem *item = new QTreeWidgetItem;
                    item->setText(0, data.first);
                    item->setText(1, data.second);
                    treeWidget->addTopLevelItem(item);
                }
            }
        }
    }

    if (treeWidget->topLevelItemCount() == 0) {
        loadDeviceList2();
    }

    treeWidget->header()->setResizeMode(QHeaderView::ResizeToContents);

}


// worst case
void SNoopDialog::loadDeviceList2()
{

    foreach (const DeviceData &dev, SNoop::Device::getDeviceList()) {
        QTreeWidgetItem *item = new QTreeWidgetItem;
        item->setText(0, dev.first);
        item->setText(1, dev.second);
        treeWidget->addTopLevelItem(item);
    }

}


#include "snoopdialog.moc"
