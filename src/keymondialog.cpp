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
#include "keymondialog.h"
#include "keymon/device.h"

// KDE
#include <kdeversion.h>
#if KDE_IS_VERSION(4,3,80)
    #include <kauth.h>
#endif
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


KeyMonDialog::KeyMonDialog(QWidget *parent)
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


KeyMonDialog::~KeyMonDialog()
{



}


QTreeWidgetItem *KeyMonDialog::newDeviceItem(const DeviceData *d)
{

    QTreeWidgetItem *item = new QTreeWidgetItem;
    if (!d->first.isEmpty()) {
        item->setText(0, d->first);
    } else {
        item->setText(0, i18n("Unknown"));
    }
    item->setText(1, d->second);
    item->setIcon(0, KIcon("input-mouse"));

    return item;

}


void KeyMonDialog::dialogFinished(const int &ret)
{

    if (ret == KDialog::Accepted) {
        QList<QTreeWidgetItem*> items = treeWidget->selectedItems();
        if (!items.isEmpty()) {
            emit deviceSelected(items[0]->text(1));
        }
    }

}


void KeyMonDialog::loadDeviceList()
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
#if KDE_IS_VERSION(4,3,80)
                const QString file = reply.arguments().first().toString();
                if (file.isEmpty()) {
                    continue;
                }

                KAuth::Action action("org.kde.recorditnow.helper.name");
                QVariantMap args;
                args["Device"] = file;
                action.setArguments(args);
                KAuth::ActionReply reply = action.execute("org.kde.recorditnow.helper");

                DeviceData d;
                d.first = reply.data().value("Name").toString();
                d.second = file;

                QTreeWidgetItem *item = newDeviceItem(&d);
                treeWidget->addTopLevelItem(item);
#else
                const DeviceData data = KeyMon::Device::getDevice(reply.arguments().first().toString());
                if (!data.second.isEmpty()) {
                    QTreeWidgetItem *item = newDeviceItem(&data);
                    treeWidget->addTopLevelItem(item);
                }
#endif
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
void KeyMonDialog::loadDeviceList2()
{

    foreach (const DeviceData &dev, KeyMon::Device::getDeviceList()) {
        if (!dev.second.isEmpty()) {
            QTreeWidgetItem *item = newDeviceItem(&dev);
            treeWidget->addTopLevelItem(item);
        }
    }

}


void KeyMonDialog::updateStatus()
{

#if KDE_IS_VERSION(4,3,80)
    KAuth::Action action("org.kde.recorditnow.helper.name");
    KAuth::Action::AuthStatus status = action.authorize();

    for (int i = 0; i < treeWidget->topLevelItemCount(); i++) {
        QTreeWidgetItem *item = treeWidget->topLevelItem(i);
        QFile file(item->text(1));
        if (!file.exists()) {
            item->setIcon(2, KIcon("dialog-error"));
            item->setText(2, i18n("File not found"));
            item->setData(0, Qt::UserRole, "ERR_FILENOTFOUND");
        } else if (status != KAuth::Action::Authorized) {
            item->setIcon(2, KIcon("dialog-error"));
            switch (status) {
            case KAuth::Action::Denied: item->setText(2, i18n("The authorization has been denied "
                                                              "by the authorization backend.")); break;
            default: item->setText(2, i18n("The authorization has been denied.")); break;
            }
            item->setData(0, Qt::UserRole, "ERR_OPENFAILED");
        } else {
            item->setIcon(2, KIcon("dialog-ok"));
            item->setText(2, i18n("Ok"));
            item->setData(0, Qt::UserRole, "ERR_NOERROR");
        }
    }
#else
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
#endif

}



#include "keymondialog.moc"
