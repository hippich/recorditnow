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

    setButtons(KDialog::Ok|KDialog::Cancel|KDialog::User1|KDialog::User2);
    setButtonText(KDialog::User1, i18n("Reload"));
    setButtonIcon(KDialog::User1, KIcon("view-refresh"));
    setButtonText(KDialog::User2, i18n("Fix Permissions"));
    setButtonIcon(KDialog::User2, KIcon("tools-wizard"));

    connect(this, SIGNAL(finished(int)), this, SLOT(dialogFinished(int)));
    connect(button(KDialog::User1), SIGNAL(clicked()), this, SLOT(loadDeviceList()));
    connect(button(KDialog::User2), SIGNAL(clicked()), this, SLOT(fixPermissions()));
    connect(treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(itemSelectionChanged()));

    resize(600, 300);

    QTimer::singleShot(0, this, SLOT(loadDeviceList()));

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
                if (!data.second.isEmpty()) {
                    QTreeWidgetItem *item = new QTreeWidgetItem;
                    if (!data.first.isEmpty()) {
                        item->setText(0, data.first);
                    } else {
                        item->setText(0, i18n("Unkown"));
                    }
                    item->setText(1, data.second);
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
    itemSelectionChanged();

}


// worst case
void SNoopDialog::loadDeviceList2()
{

    foreach (const DeviceData &dev, SNoop::Device::getDeviceList()) {
        if (!dev.second.isEmpty()) {
            QTreeWidgetItem *item = new QTreeWidgetItem;
            if (!dev.first.isEmpty()) {
                item->setText(0, dev.first);
            } else {
                item->setText(0, i18n("Unkown"));
            }
            item->setText(1, dev.second);
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


void SNoopDialog::fixPermissions()
{

    QList<QTreeWidgetItem*> items = treeWidget->selectedItems();
    if (items.isEmpty()) {
        return;
    }

    const QString exe = KGlobal::dirs()->findExe("recorditnow_fix_permissions");
    if (exe.isEmpty()) {
        KMessageBox::error(this, i18n("Cannot find \"%1\"", QString("recorditnow_fix_permissions")));
        return;
    }

    const QString su = KGlobal::dirs()->findExe("kdesu");
    if (exe.isEmpty()) {
        KMessageBox::error(this, i18n("Cannot find \"%1\"", QString("kdesu")));
        return;
    }

    const QString device = items[0]->text(1);
    const QString cmd = exe+' '+"--device "+device;
    const QStringList args = QStringList() << "-i" << "recorditnow"  << "--attach"
                             << QString::number(winId()) << "--noignorebutton" << "-c" << cmd;

    kDebug() << "args:" << args;

    setEnabled(false);

    KProcess *process = new KProcess(this);
    connect(process, SIGNAL(finished(int,QProcess::ExitStatus)), this,
            SLOT(fixFinished(int,QProcess::ExitStatus)));

    process->setProgram(su, args);
    process->setOutputChannelMode(KProcess::MergedChannels);
    process->start();

}


void SNoopDialog::fixFinished(const int &exitCode, const QProcess::ExitStatus &exitStatus)
{

    // TODO
    switch (exitCode) {
    case 0: break; // no error
    case 1:        // no such file
    case 2:        // perm denied
    case 3:        // groupadd not found
    case 4:        // internal error
    case 5:        // gpasswd not found
    case 6:        // no user
    case 7:        // open failed
    case 8:        // write failed
    case 9:        // chmod not found
    case 10:       // chmod failed
    default: break;
    }

    KProcess *process = static_cast<KProcess*>(sender());

    kDebug() << "fix finished:" << "exitCode:" << exitCode << "exitStatus:" << exitStatus <<
            "output:" << process->readAllStandardOutput();

    delete process;

    loadDeviceList();
    setEnabled(true);

}


void SNoopDialog::itemSelectionChanged()
{

    QList<QTreeWidgetItem*> items = treeWidget->selectedItems();
    if (items.isEmpty() || items.size() > 1) {
        button(KDialog::User2)->setEnabled(false);
        return;
    }

    if (items[0]->data(0, Qt::UserRole).toString() == "ERR_OPENFAILED") {
        button(KDialog::User2)->setEnabled(true);
    } else {
        button(KDialog::User2)->setEnabled(false);
    }

}



#include "snoopdialog.moc"
