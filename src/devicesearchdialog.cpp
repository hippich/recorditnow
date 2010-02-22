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
#include "keymonmanager.h"

// KDE
#include <klocalizedstring.h>

// Qt
#include <QtGui/QTreeWidgetItem>


DeviceSearchDialog::DeviceSearchDialog(const bool &input, QWidget *parent)
    : KDialog(parent), m_input(input)
{

    QWidget *main = new QWidget(this);
    setupUi(main);
    setMainWidget(main);

    setAttribute(Qt::WA_DeleteOnClose);
    resize(500, 300);

    if (input) {
        setWindowTitle(i18n("Input Device"));
        foreach (const KeyMon::DeviceInfo &info, KeyMonManager::self()->getInputDeviceList()) {
            QTreeWidgetItem *item = new QTreeWidgetItem;
            item->setText(0, info.name);
            item->setText(1, info.file);
            item->setText(2, info.uuid);

            if (info.name.toLower().contains(QRegExp(".*keyboard.*"))) {
                item->setIcon(0, KIcon("input-keyboard"));
            } else if (info.name.toLower().contains(QRegExp(".*mouse.*"))) {
                item->setIcon(0, KIcon("input-mouse"));
            }

            treeWidget->addTopLevelItem(item);
        }
    }

    connect(this, SIGNAL(finished(int)), this, SLOT(dialogFinished(int)));

}


void DeviceSearchDialog::dialogFinished(const int &ret)
{

    if (ret == KDialog::Accepted) {
        QList<QTreeWidgetItem*> items = treeWidget->selectedItems();
        if (items.isEmpty()) {
            return;
        }
        emit deviceSelected(items.first()->text(2));
    }

}


#include "devicesearchdialog.moc"
