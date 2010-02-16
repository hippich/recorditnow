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
#include "sizewidget.h"

// KDE
#include <kdebug.h>

// Qt
#include <QtGui/QTreeWidgetItem>


typedef QPair<QString, QSize> Size;
FrameConfig::FrameConfig(const QList<QPair<QString,QSize> > &sizes, QWidget *parent)
    : QWidget(parent)
{

    setupUi(this);

    foreach (const Size &s, sizes) {

        QTreeWidgetItem *item = new QTreeWidgetItem;
        item->setText(0, s.first);
        item->setFlags(Qt::ItemIsEditable|Qt::ItemIsEnabled|Qt::ItemIsSelectable);

        sizeTree->addTopLevelItem(item);

        SizeWidget *widget = newSizeWidget();
        widget->setSize(s.second);

        sizeTree->setItemWidget(item, 1, widget);
    }

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


QList<Size> FrameConfig::sizes() const
{

    QList<Size> sizes;
    for (int i = 0; i < sizeTree->topLevelItemCount(); i++) {
        QTreeWidgetItem *item = sizeTree->topLevelItem(i);
        sizes.append(qMakePair(item->text(0),
                               static_cast<SizeWidget*>(sizeTree->itemWidget(item, 1))->getSize()));
    }
    return sizes;

}


void FrameConfig::add()
{

    if (sizeEdit->text().isEmpty()) {
        return;
    }

    QTreeWidgetItem *item = new QTreeWidgetItem;
    item->setText(0, sizeEdit->text());

    sizeTree->addTopLevelItem(item);
    sizeTree->setItemWidget(item, 1, new SizeWidget(this));

    sizeEdit->clear();

    emit configChanged();

}


void FrameConfig::remove()
{

    QList<QTreeWidgetItem*> items = sizeTree->selectedItems();
    if (items.isEmpty()) {
        return;
    }

    delete sizeTree->takeTopLevelItem(sizeTree->indexOfTopLevelItem(items.first()));

    emit configChanged();

}


void FrameConfig::up()
{

    QList<QTreeWidgetItem*> items = sizeTree->selectedItems();
    if (items.isEmpty()) {
        return;
    }

    const int index = sizeTree->indexOfTopLevelItem(items.first());
    if (index < 1) {
        return;
    }

    SizeWidget *widget = newSizeWidget();
    QTreeWidgetItem *item = new QTreeWidgetItem;
    item->setFlags(Qt::ItemIsEditable|Qt::ItemIsEnabled|Qt::ItemIsSelectable);

    item->setText(0, items.first()->text(0));
    widget->setSize(static_cast<SizeWidget*>(sizeTree->itemWidget(items.first(), 1))->getSize());

    sizeTree->insertTopLevelItem(index-1, item);
    sizeTree->setItemWidget(item, 1, widget);

    delete sizeTree->takeTopLevelItem(sizeTree->indexOfTopLevelItem(items.first()));
    sizeTree->setCurrentItem(item);

    emit configChanged();

}


void FrameConfig::down()
{

    QList<QTreeWidgetItem*> items = sizeTree->selectedItems();
    if (items.isEmpty()) {
        return;
    }

    const int index = sizeTree->indexOfTopLevelItem(items.first());
    if (index+1 == -1 || index+1 >= sizeTree->topLevelItemCount()) {
        return;
    }

    SizeWidget *widget = newSizeWidget();
    QTreeWidgetItem *item = new QTreeWidgetItem;
    item->setFlags(Qt::ItemIsEditable|Qt::ItemIsEnabled|Qt::ItemIsSelectable);

    item->setText(0, items.first()->text(0));
    widget->setSize(static_cast<SizeWidget*>(sizeTree->itemWidget(items.first(), 1))->getSize());

    sizeTree->insertTopLevelItem(index+2, item);
    sizeTree->setItemWidget(item, 1, widget);

    delete sizeTree->takeTopLevelItem(sizeTree->indexOfTopLevelItem(items.first()));
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


void FrameConfig::updateColumnSize()
{

    sizeTree->header()->resizeSections(QHeaderView::ResizeToContents);
    sizeTree->header()->setStretchLastSection(true);

}


void FrameConfig::itemChanged(QTreeWidgetItem *item, int column)
{

    Q_UNUSED(item);
    Q_UNUSED(column);

    emit configChanged();

}


SizeWidget *FrameConfig::newSizeWidget()
{

    SizeWidget *widget = new SizeWidget(this);
    connect(widget, SIGNAL(valueChanged()), this, SIGNAL(configChanged()));
    connect(widget, SIGNAL(sizeChanged()), this, SLOT(updateColumnSize()));

    return widget;

}


#include "frameconfig.moc"

