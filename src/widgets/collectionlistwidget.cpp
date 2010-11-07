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
#include "collectionlistwidget.h"
#include "collectionlistwidget_p.h"
#include "collection/collection.h"
#include "collectionitemdelegate.h"
#include "collectionlistwidgetitem.h"
#include "listwidgetbuttoncontainer_p.h"
#include "collectionlistmodel_p.h"

// KDE
#include <kdebug.h>
#include <klocalizedstring.h>

// Qt
#include <QtGui/QLabel>
#include <QtGui/QStandardItemModel>


namespace RecordItNow {


CollectionListWidgetPrivate::CollectionListWidgetPrivate(CollectionListWidget *parent, const bool &itemList)
    : QObject(parent),
    q(parent)
{

    q->setModel(new CollectionListModel(q));
    q->setViewMode(QListView::ListMode);
    q->setItemDelegate(new CollectionItemDelegate(q, itemList));

    buttonContainer = new ListWidgetButtonContainer(q);
    connect(buttonContainer, SIGNAL(sizeChanged()), this, SLOT(buttonContainerSizeChanged()));
    connect(buttonContainer, SIGNAL(playClicked()), this, SLOT(playClicked()));
    connect(buttonContainer, SIGNAL(addClicked()), this, SLOT(addClicked()));
    connect(buttonContainer, SIGNAL(editClicked()), this, SLOT(editClciked()));
    connect(buttonContainer, SIGNAL(deleteClicked()), this, SLOT(deleteClicked()));
    connect(buttonContainer, SIGNAL(uploadClicked()), this, SLOT(uploadClicked()));
    buttonContainer->show();

    if (itemList) {
        q->setIconSize(QSize(80, 80));
        q->setSpacing(0);
    }

    q->setAlternatingRowColors(true);
    q->setMovement(QListView::Static);
    q->setResizeMode(QListView::Adjust);
    q->setSelectionBehavior(QAbstractItemView::SelectItems);
    q->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(q, SIGNAL(itemSelectionChanged()), this, SLOT(selectionChanged()));
    selectionChanged();

}


CollectionListWidgetPrivate::~CollectionListWidgetPrivate()
{

    delete buttonContainer;

}


void CollectionListWidgetPrivate::itemsAdded(const QList<RecordItNow::CollectionItem*> &items)
{

    foreach (CollectionItem *item, items) {
        CollectionListWidgetItem *listItem = new CollectionListWidgetItem(item, q);
        q->addItem(listItem);

        if (item->thumbnail(q->iconSize()).isNull()) {
            item->createThumbnail(q->iconSize());
        }
    }

}


void CollectionListWidgetPrivate::itemsRemoved(const QList<RecordItNow::CollectionItem*> &items)
{

    foreach (CollectionItem *item, items) {
        for (int row = 0; row < q->count(); row++) {
            CollectionListWidgetItem *listItem = static_cast<CollectionListWidgetItem*>(q->item(row));

            if (listItem->item() && listItem->item() == item) {
                q->takeItem(row);
                delete listItem;

                break;
            }
        }
    }

}


void CollectionListWidgetPrivate::buttonContainerSizeChanged()
{

    q->setViewportMargins(0, buttonContainer->height(), 0, 0);

}



void CollectionListWidgetPrivate::selectionChanged()
{

    const CollectionListWidgetItem *item = q->selectedItems().isEmpty() ? 0 : q->selectedItems().at(0);
    if (buttonContainer->button(CollectionListWidget::AddButton)) {
        buttonContainer->button(CollectionListWidget::AddButton)->setEnabled(true);
    }
    if (buttonContainer->button(CollectionListWidget::EditButton)) {
        buttonContainer->button(CollectionListWidget::EditButton)->setEnabled((item && item->item() && item->item()->collection()));
    }
    if (buttonContainer->button(CollectionListWidget::DeleteButton)) {
        buttonContainer->button(CollectionListWidget::DeleteButton)->setEnabled((item && item->item() && item->item()->collection()));
    }
    if (buttonContainer->button(CollectionListWidget::PlayButton)) {
        buttonContainer->button(CollectionListWidget::PlayButton)->setEnabled(item && item->item());
    }
    if (buttonContainer->button(CollectionListWidget::UploadButton)) {
        buttonContainer->button(CollectionListWidget::UploadButton)->setEnabled(item && item->item());
    }

}


void CollectionListWidgetPrivate::addClicked()
{

    if (collection) { // new item
        emit q->addRequested(collection.data());
    } else { // new collection
        emit q->addRequested();
    }

}


void CollectionListWidgetPrivate::editClciked()
{

    RecordItNow::CollectionItem *item = q->selectedItem();
    if (item) {
        emit q->editRequested(item);
    }

}


void CollectionListWidgetPrivate::deleteClicked()
{

    RecordItNow::CollectionItem *item = q->selectedItem();
    if (item) {
        emit q->deleteRequested(item);
    }

}


void CollectionListWidgetPrivate::playClicked()
{

    RecordItNow::CollectionItem *item = q->selectedItem();
    if (item) {
        emit q->playRequested(item);
    }

}


void CollectionListWidgetPrivate::uploadClicked()
{

    RecordItNow::CollectionItem *item = q->selectedItem();
    if (item) {
        emit q->uploadRequested(item);
    }

}



CollectionListWidget::CollectionListWidget(QWidget *parent, const bool &itemList)
    : QListView(parent),
    d(new CollectionListWidgetPrivate(this, itemList))
{

    setSelectionModel(new QItemSelectionModel(model()));

    if (itemList) {
        d->buttonContainer->reLayout();
    }

}


CollectionListWidget::~CollectionListWidget()
{

    delete d;

}


RecordItNow::Collection *CollectionListWidget::collection() const
{

    if (d->collection) {
        return d->collection.data();
    } else {
        return 0;
    }

}


RecordItNow::CollectionItem *CollectionListWidget::selectedItem() const
{

    QModelIndexList list = selectedIndexes();
    if (list.isEmpty()) {
        return 0;
    }
    return static_cast<CollectionListWidgetItem*>(item(list.at(0).row()))->item();

}


RecordItNow::CollectionListWidgetItem *CollectionListWidget::item(const int &row) const
{

    return static_cast<CollectionListWidgetItem*>(model()->item(row));

}


QList<RecordItNow::CollectionListWidgetItem*> CollectionListWidget::items() const
{

    QList<RecordItNow::CollectionListWidgetItem*> list;
    for (int row = 0; row < count(); row++) {
        list.append(item(row));
    }
    return list;

}


QList<RecordItNow::CollectionListWidgetItem*> CollectionListWidget::selectedItems() const
{

    QList<RecordItNow::CollectionListWidgetItem*> selected;
    foreach (const QModelIndex &index, selectionModel()->selectedRows()) {
        selected.append(static_cast<CollectionListWidgetItem*>(model()->itemFromIndex(index)));
    }

    return selected;

}


QStandardItemModel *CollectionListWidget::model() const
{

    return static_cast<QStandardItemModel*>(QListView::model());

}


int CollectionListWidget::count() const
{

    return model()->rowCount();

}


int CollectionListWidget::row(const RecordItNow::CollectionListWidgetItem *item) const
{

    return model()->indexFromItem(item).row();

}


RecordItNow::CollectionListWidgetItem *CollectionListWidget::takeItem(const int &row)
{

    QList<QStandardItem*> items = model()->takeRow(row);
    Q_ASSERT(!items.isEmpty());
    Q_ASSERT(items.size() == 1);

    return static_cast<CollectionListWidgetItem*>(items.at(0));

}


void CollectionListWidget::addItem(RecordItNow::CollectionListWidgetItem *item)
{

    model()->appendRow(item);

}


void CollectionListWidget::clear()
{

    for (int i = 0; i < count(); i++) {
        delete takeItem(0);
    }

}


void CollectionListWidget::setCollection(RecordItNow::Collection *collection)
{

    if (d->collection) {
        d->collection.data()->disconnect(d);
    }
    d->collection = collection;

    if (d->collection) {
        connect(d->collection.data(), SIGNAL(itemsAdded(QList<RecordItNow::CollectionItem*>)), d,
                SLOT(itemsAdded(QList<RecordItNow::CollectionItem*>)));
        connect(d->collection.data(), SIGNAL(itemsRemoved(QList<RecordItNow::CollectionItem*>)), d,
                SLOT(itemsRemoved(QList<RecordItNow::CollectionItem*>)));

        d->itemsAdded(d->collection.data()->items());
    }

    d->selectionChanged();

}


void CollectionListWidget::setButtons(const RecordItNow::CollectionListWidget::ButtonCodes &buttons)
{

    d->buttonContainer->setButtons(buttons);
    d->selectionChanged(); // update buttons

}


void CollectionListWidget::ensureVisible(const RecordItNow::CollectionItem *item)
{

    foreach (CollectionListWidgetItem *listItem, items()) {
        if (listItem->item() == item) {
            scrollTo(model()->indexFromItem(listItem), QListView::EnsureVisible);
            break;
        }
    }

}


void CollectionListWidget::selectItem(const RecordItNow::CollectionItem *item)
{

    foreach (CollectionListWidgetItem *listItem, items()) {
        if (listItem->item() == item) {
            selectionModel()->select(listItem->index(), QItemSelectionModel::ClearAndSelect);
            break;
        }
    }

}


void CollectionListWidget::setSelectionModel(QItemSelectionModel *selectionModel)
{

    QListView::setSelectionModel(selectionModel);
    connect(selectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SIGNAL(itemSelectionChanged()));

}


void CollectionListWidget::setModel(QAbstractItemModel *model)
{

    QListView::setModel(model);

}


void CollectionListWidget::resizeEvent(QResizeEvent *event)
{

    QListView::resizeEvent(event);
    if (d->buttonContainer) {
        d->buttonContainer->reLayout();
    }

}


} // namespace RecordItNow


#include "collectionlistwidget_p.moc"
#include "collectionlistwidget.moc"

