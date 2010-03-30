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
#include "frameconfig.h"
#include "helper.h"

// KDE
#include <kdebug.h>
#include <knuminput.h>
#include <kconfiggroup.h>

// Qt
#include <QtGui/QTreeWidgetItem>


FrameConfig::FrameConfig(KConfig *cfg, QWidget *parent)
    : RecordItNow::ConfigPage(cfg, parent)
{

    setupUi(this);

    addButton->setIcon(KIcon("list-add"));
    removeButton->setIcon(KIcon("list-remove"));
    upButton->setIcon(KIcon("go-up"));
    downButton->setIcon(KIcon("go-down"));

    connect(addButton, SIGNAL(clicked()), this, SLOT(add()));
    connect(removeButton, SIGNAL(clicked()), this, SLOT(remove()));
    connect(upButton, SIGNAL(clicked()), this, SLOT(up()));
    connect(downButton, SIGNAL(clicked()), this, SLOT(down()));
    connect(sizeTree, SIGNAL(itemSelectionChanged()), this, SLOT(itemSelectionChanged()));
    connect(sizeEdit, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));
    connect(sizeTree, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this,
            SLOT(itemChanged(QTreeWidgetItem*,int)));

    sizeTree->header()->setResizeMode(QHeaderView::ResizeToContents);
    itemSelectionChanged();
    textChanged(QString());

}


QList<FrameSize> FrameConfig::sizes() const
{

    QList<FrameSize> sizes;
    for (int i = 0; i < sizeTree->topLevelItemCount(); i++) {
        QTreeWidgetItem *item = sizeTree->topLevelItem(i);

        const KIntNumInput *width = static_cast<KIntNumInput*>(sizeTree->itemWidget(item, 1));
        const KIntNumInput *height = static_cast<KIntNumInput*>(sizeTree->itemWidget(item, 2));

        sizes.append(FrameSize(QSize(width->value(), height->value()), item->text(0)));
    }
    return sizes;

}


QList<FrameSize> FrameConfig::defaultSizes()
{

    QList<FrameSize> defaults;

    defaults.append(FrameSize(QSize(640, 480), QString("640 x 480 (4:3 SD)")));
    defaults.append(FrameSize(QSize(800, 600), QString("800 x 600")));
    defaults.append(FrameSize(QSize(1024, 768), QString("1024 x 768")));
    defaults.append(FrameSize(QSize(1280, 720), QString("1280 x 720 (16x9 HD)")));

    return defaults;

}


QList<FrameSize> FrameConfig::readSizes(KConfig *config)
{

    KConfigGroup cfg(config, "Frame");
    return RecordItNow::Helper::listFromArray<FrameSize>(cfg.readEntry("Frames", QByteArray()));

}


void FrameConfig::writeSizes(const QList<FrameSize> &sizes, KConfig *config)
{

    QStringList list;

    KConfigGroup cfg(config, "Frame");
    /*foreach (const Size &s, sizes) {
        cfg.writeEntry(QString("Size %1").arg(s.first), s.second);
        list.append(s.first);
    }
    cfg.writeEntry("Names", list);
    */
    cfg.writeEntry("Frames", RecordItNow::Helper::listToArray(sizes));
    cfg.sync();

}


void FrameConfig::saveConfig()
{

    writeSizes(sizes(), config());

}


void FrameConfig::setDefaults()
{

    sizeTree->clear();

    foreach (const FrameSize &size, defaultSizes()) {
        newTreeWidgetItem(size.text(), size.size(), sizeTree->topLevelItemCount());
    }

}


void FrameConfig::loadConfig()
{

    foreach (const FrameSize &size, readSizes(config())) {
        newTreeWidgetItem(size.text(), size.size(), sizeTree->topLevelItemCount());
    }

}


void FrameConfig::add()
{

    if (sizeEdit->text().isEmpty()) {
        return;
    }

    newTreeWidgetItem(sizeEdit->text(), QSize(100, 100), sizeTree->topLevelItemCount());
    sizeEdit->clear();

    emit configChanged();

}


void FrameConfig::remove()
{

    QList<QTreeWidgetItem*> items = sizeTree->selectedItems();
    if (items.isEmpty()) {
        return;
    }

    QTreeWidgetItem *item = items.first();

    sizeTree->takeTopLevelItem(sizeTree->indexOfTopLevelItem(item));
    delete item;

    emit configChanged();

}


void FrameConfig::up()
{

    QList<QTreeWidgetItem*> items = sizeTree->selectedItems();
    if (items.isEmpty()) {
        return;
    }

    const int from = sizeTree->indexOfTopLevelItem(items.first());
    const int to = from-1;

    move(from, to);


}


void FrameConfig::down()
{

    QList<QTreeWidgetItem*> items = sizeTree->selectedItems();
    if (items.isEmpty()) {
        return;
    }

    const int from = sizeTree->indexOfTopLevelItem(items.first());
    const int to = from+1;

    move(from, to);

}


void FrameConfig::move(const int &from, const int &to)
{

    if (to > sizeTree->topLevelItemCount() || to < 0) {
        return;
    }

    QTreeWidgetItem *oldItem = sizeTree->topLevelItem(from);

    const int width = static_cast<KIntNumInput*>(sizeTree->itemWidget(oldItem, 1))->value();
    const int height = static_cast<KIntNumInput*>(sizeTree->itemWidget(oldItem, 2))->value();

    QTreeWidgetItem *item = newTreeWidgetItem(oldItem->text(0), QSize(width, height), to < from ? to : to+1);

    sizeTree->takeTopLevelItem(sizeTree->indexOfTopLevelItem(oldItem));
    delete oldItem;

    sizeTree->setCurrentItem(item);

    emit configChanged();

}



void FrameConfig::itemSelectionChanged()
{

    if(sizeTree->selectedItems().isEmpty()) {
        upButton->setEnabled(false);
        downButton->setEnabled(false);
        removeButton->setEnabled(false);
    } else {
        const int index = sizeTree->indexOfTopLevelItem(sizeTree->selectedItems().first());
        upButton->setEnabled(index != 0);
        downButton->setEnabled(index != sizeTree->topLevelItemCount()-1);
        removeButton->setEnabled(true);
    }

}


void FrameConfig::textChanged(const QString &text)
{

    if (text.isEmpty()) {
        addButton->setDisabled(true);
        return;
    }

    for (int i = 0; i < sizeTree->topLevelItemCount(); i++) {
        QTreeWidgetItem *item = sizeTree->topLevelItem(i);
        if (item->text(0) == text) {
            addButton->setDisabled(true);
            return;
        }
    }
    addButton->setDisabled(false);

}


void FrameConfig::itemChanged(QTreeWidgetItem *item, int column)
{

    Q_UNUSED(item);
    Q_UNUSED(column);

    emit configChanged();

}


KIntNumInput *FrameConfig::newSizeWidget()
{

    KIntNumInput *widget = new KIntNumInput(this);
    widget->setMaximum(4000);
    widget->setMinimum(100);

    connect(widget, SIGNAL(valueChanged(int)), this, SIGNAL(configChanged()));

    return widget;

}


QTreeWidgetItem *FrameConfig::newTreeWidgetItem(const QString &text, const QSize &size, const int &index)
{

    QTreeWidgetItem *item = new QTreeWidgetItem;
    item->setText(0, text);
    item->setFlags(Qt::ItemIsEditable|Qt::ItemIsEnabled|Qt::ItemIsSelectable);

    sizeTree->insertTopLevelItem(index, item);

    KIntNumInput *widget = newSizeWidget();
    widget->setValue(size.width());
    sizeTree->setItemWidget(item, 1, widget);

    widget = newSizeWidget();
    widget->setValue(size.height());
    sizeTree->setItemWidget(item, 2, widget);

    return item;

}


#include "frameconfig.moc"

