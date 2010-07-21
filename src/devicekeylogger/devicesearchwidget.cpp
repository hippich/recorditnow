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
#include "devicesearchwidget.h"
#include "manager.h"

// Qt
#include <QtGui/QTreeWidgetItem>


namespace RecordItNow {


DeviceSearchWidget::DeviceSearchWidget(QWidget *parent)
    : QWidget(parent)
{

    setupUi(this);

    connect(treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(itemSelectionChanged()));

}


DeviceSearchWidget::~DeviceSearchWidget()
{


}


int DeviceSearchWidget::deviceCount() const
{

    return treeWidget->topLevelItemCount();

}


QString DeviceSearchWidget::selectedDevice() const
{

    QList<QTreeWidgetItem*> items = treeWidget->selectedItems();
    if (!items.isEmpty()) {
        return items.first()->text(2);
    } else {
        return QString();
    }

}


void DeviceSearchWidget::search(const KeyMon::DeviceInfo::DeviceType &type)
{

    treeWidget->clear();

    switch (type) {
    case KeyMon::DeviceInfo::MouseType:
    case KeyMon::DeviceInfo::KeyboardType: {
            foreach (const KeyMon::DeviceInfo &info, KeyMon::Manager::getInputDeviceList()) {
                if (info.type != type) {
                    continue;
                }
                QTreeWidgetItem *item = new QTreeWidgetItem;
                if (!info.icon.isEmpty()) {
                    item->setIcon(0, KIcon(info.icon));
                }
                item->setText(0, info.name);
                item->setText(1, info.file);
                item->setText(2, info.uuid);
                treeWidget->addTopLevelItem(item);
            }
        }
    default: break;
    }

    treeWidget->header()->setResizeMode(QHeaderView::ResizeToContents);

    setDisabled(deviceCount() == 0);

}


void DeviceSearchWidget::itemSelectionChanged()
{

    emit deviceSelectionChanged(selectedDevice());

}


} // namespace RecordItNow


#include "devicesearchwidget.moc"
