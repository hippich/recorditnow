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
#include <klocalizedstring.h>
#include <kpushbutton.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>

// Qt
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusInterface>
#include <QtCore/QFile>
#include <QtCore/QTimer>


SNoopDialog::SNoopDialog(QWidget *parent)
    : KDialog(parent)
{

    setWindowTitle(i18n("Mouse"));
    setAttribute(Qt::WA_DeleteOnClose);

    QWidget *widget = new QWidget(this);
    setupUi(widget);
    setMainWidget(widget);

    setButtons(KDialog::Ok|KDialog::Cancel|KDialog::User1);
    setButtonText(KDialog::User1, i18n("Reload"));
    setButtonIcon(KDialog::User1, KIcon("view-refresh"));

    connect(this, SIGNAL(finished(int)), this, SLOT(dialogFinished(int)));
    connect(button(KDialog::User1), SIGNAL(clicked()), this, SLOT(loadDeviceList()));

    resize(600, 300);

    QTimer::singleShot(0, this, SLOT(loadDeviceList()));

}


SNoopDialog::~SNoopDialog()
{



}


QTreeWidgetItem *SNoopDialog::newDeviceItem(const DeviceData *d)
{

    QTreeWidgetItem *item = new QTreeWidgetItem;
    if (!d->first.isEmpty()) {
        item->setText(0, d->first);
    } else {
        item->setText(0, i18n("Unkown"));
    }
    item->setText(1, d->second);
    item->setIcon(0, KIcon("input-mouse"));

    return item;

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
                if (!data.second.isEmpty()) {
                    QTreeWidgetItem *item = newDeviceItem(&data);
                    treeWidget->addTopLevelItem(item);
                }
            }
        }
    }

    if (treeWidget->topLevelItemCount() == 0) {
        loadDeviceList2();
    }

    updateStatus();
    treeWidget->header()->setResizeMode(QHeaderView::ResizeToContents);

}


// worst case
void SNoopDialog::loadDeviceList2()
{

    foreach (const DeviceData &dev, SNoop::Device::getDeviceList()) {
        if (!dev.second.isEmpty()) {
            QTreeWidgetItem *item = newDeviceItem(&dev);
            treeWidget->addTopLevelItem(item);
        }
    }

}


void SNoopDialog::updateStatus()
{

    for (int i = 0; i < treeWidget->topLevelItemCount(); i++) {
        QTreeWidgetItem *item = treeWidget->topLevelItem(i);
        QFile file(item->text(1));
        if (!file.exists()) {
            item->setIcon(2, KIcon("dialog-error"));
            item->setText(2, i18n("File not found"));
            item->setData(0, Qt::UserRole, "ERR_FILENOTFOUND");
        } else if (!file.open(QIODevice::ReadOnly)) {
            file.close();
            item->setIcon(2, KIcon("dialog-error"));
            item->setText(2, i18nc("%1 = error string", "Cannot open file: %1", file.errorString()));
            item->setData(0, Qt::UserRole, "ERR_OPENFAILED");
        } else {
            item->setIcon(2, KIcon("dialog-ok"));
            item->setText(2, i18n("Ok"));
            item->setData(0, Qt::UserRole, "ERR_NOERROR");
        }
    }

}



#include "snoopdialog.moc"
