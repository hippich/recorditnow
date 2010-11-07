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
#include "collectionview.h"
#include "collectionview_p.h"
#include "collection/collection.h"
#include "collectionitemdelegate.h"
#include "collectionlistwidget.h"
#include "collectionlistwidgetitem.h"
#include "collectionitemdialog.h"

// KDE
#include <kicon.h>
#include <kdebug.h>
#include <klocalizedstring.h>
#include <kiconloader.h>

// Qt
#include <QtGui/QStackedLayout>
#include <QtGui/QStyledItemDelegate>
#include <QtGui/QScrollBar>
#include <QtGui/QSortFilterProxyModel>
#include <QtGui/QStandardItemModel>
#include <QtGui/QSplitter>


namespace RecordItNow {


CollectionViewPrivate::CollectionViewPrivate(CollectionView *parent)
    : QObject(parent),
    ui(new Ui::CollectionView),
    q(parent)
{

    ui->setupUi(q);

    ui->viewLayout->removeWidget(ui->collectionList);
    ui->viewLayout->removeWidget(ui->contentWidget);
    QSplitter *splitter = new QSplitter(q);
    splitter->addWidget(ui->collectionList);
    splitter->addWidget(ui->contentWidget);
    ui->viewLayout->addWidget(splitter);


    ui->contentWidgetLayout->removeWidget(ui->searchWidget);
    ui->contentWidgetLayout->removeWidget(ui->collectionItemWidget);
    splitter = new QSplitter(q);
    splitter->setOrientation(Qt::Vertical);
    splitter->addWidget(ui->searchWidget);
    splitter->addWidget(ui->collectionItemWidget);
    ui->contentWidgetLayout->addWidget(splitter);


    ui->collectionList->setButtons(CollectionListWidget::AddButton|CollectionListWidget::DeleteButton|
                                   CollectionListWidget::EditButton);

    layout = new QStackedLayout;
    layout->setSpacing(0);
    layout->setContentsMargins(4, 0, 4, 4);
    ui->collectionItemWidget->setLayout(layout);

    connect(ui->collectionList, SIGNAL(itemSelectionChanged()), this,
            SLOT(collectionSelectionChanged()));
    connect(layout, SIGNAL(currentChanged(int)), this, SLOT(currentCollectionListChanged(int)));

    searchList = new CollectionListWidget(q, true);
    searchList->setButtons(CollectionListWidget::PlayButton|CollectionListWidget::EditButton|
                           CollectionListWidget::DeleteButton);
    connect(searchList, SIGNAL(deleteRequested(RecordItNow::CollectionItem*)), q,
            SLOT(deleteItem(RecordItNow::CollectionItem*)));
    connect(searchList, SIGNAL(editRequested(RecordItNow::CollectionItem*)), q,
            SLOT(editItem(RecordItNow::CollectionItem*)));
    connect(searchList, SIGNAL(playRequested(RecordItNow::CollectionItem*)), q,
            SIGNAL(playRequested(RecordItNow::CollectionItem*)));
    connect(searchList, SIGNAL(uploadRequested(RecordItNow::CollectionItem*)), q,
            SIGNAL(uploadRquested(RecordItNow::CollectionItem*)));


    layout->addWidget(searchList);
    CollectionListWidgetItem *item = new CollectionListWidgetItem(0, searchList);
    item->setText(i18n("Search results"));
    item->setIcon(KIcon("system-search"));
    ui->collectionList->addItem(item);

    connect(ui->collectionList, SIGNAL(deleteRequested(RecordItNow::CollectionItem*)), q,
            SLOT(deleteItem(RecordItNow::CollectionItem*)));
    connect(ui->collectionList, SIGNAL(editRequested(RecordItNow::CollectionItem*)), q,
            SLOT(editItem(RecordItNow::CollectionItem*)));
    connect(ui->collectionList, SIGNAL(addRequested(RecordItNow::Collection*)), q,
            SLOT(createItem(RecordItNow::Collection*)));

    ui->searchWidget->setView(q);
    ui->searchWidget->setListWidget(searchList);
    connect(ui->searchWidget, SIGNAL(activated()), this, SLOT(activateSearchList()));

}


CollectionViewPrivate::~CollectionViewPrivate()
{

    delete ui;

}


CollectionListWidget *CollectionViewPrivate::currentList() const
{

    QWidget *list = layout->currentWidget();
    if (!list) {
        return 0;
    }
    return static_cast<CollectionListWidget*>(list);

}


void CollectionViewPrivate::currentCollectionListChanged(const int &index)
{

    currentItemChanged();

    for (int i = 0; i < layout->count(); i++) {
        static_cast<CollectionListWidget*>(layout->widget(i))->disconnect(this);
    }

    CollectionListWidget *list = static_cast<CollectionListWidget*>(layout->widget(index));
    if (!list) {
        return;
    }

    connect(list, SIGNAL(itemSelectionChanged()), this,
            SLOT(currentItemChanged()));

    for (int i = 0; i < ui->collectionList->count(); i++) {
        CollectionListWidgetItem *item = ui->collectionList->item(i);
        if (item->list() && item->list() == list) {
            if (!list->selectionModel()->isSelected(item->index())) {
                list->selectionModel()->select(item->index(), QItemSelectionModel::ClearAndSelect);
            }
            break;
        }
    }

}


void CollectionViewPrivate::collectionSelectionChanged()
{

    QList<CollectionListWidgetItem*> items = ui->collectionList->selectedItems();
    if (items.isEmpty()) {
        layout->setCurrentIndex(-1);
        return;
    }

    CollectionListWidgetItem *item = items.at(0);
    for (int i = 0; i < layout->count(); i++) {
        if (layout->widget(i) == item->list()) {
            layout->setCurrentIndex(i);
        }
    }

}


void CollectionViewPrivate::currentItemChanged()
{



}


void CollectionViewPrivate::activateSearchList()
{

    foreach (CollectionListWidgetItem *item, ui->collectionList->items()) {
        if (item->list() == searchList) {
            const QModelIndex index = ui->collectionList->model()->index(ui->collectionList->row(item), 0);
            ui->collectionList->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
        }
    }

}



CollectionView::CollectionView(QWidget *parent)
    : QWidget(parent),
    d(new CollectionViewPrivate(this))
{


}


CollectionView::~CollectionView()
{

    delete d;

}


QList<RecordItNow::Collection*> CollectionView::collections() const
{

    QList<RecordItNow::Collection*> list;
    foreach (CollectionListWidgetItem *item, d->ui->collectionList->items()) {
        if (item->item() && item->item()->isCollection()) {
            list.append(static_cast<RecordItNow::Collection*>(item->item()));
        }
    }
    return list;

}


RecordItNow::CollectionItem *CollectionView::selectedItem() const
{

    CollectionListWidget *list = d->currentList();
    if (!list) {
        return 0;
    }
    return list->selectedItem();

}


RecordItNow::CollectionListWidget *CollectionView::listForCollection(RecordItNow::Collection *collection) const
{

    for (int i = 0; i < d->layout->count(); i++) {
        CollectionListWidget *list = static_cast<CollectionListWidget*>(d->layout->widget(i));

        if (list->collection() && list->collection() == collection) {
            return list;
        }
    }
    return 0;

}


void CollectionView::addCollection(RecordItNow::Collection *collection)
{

    CollectionListWidget *list = new CollectionListWidget(d->ui->collectionItemWidget, true);

    list->setButtons(CollectionListWidget::PlayButton|CollectionListWidget::UploadButton|
                     CollectionListWidget::AddButton|CollectionListWidget::EditButton|
                     CollectionListWidget::DeleteButton);

    connect(list, SIGNAL(editRequested(RecordItNow::CollectionItem*)), this, SLOT(editItem(RecordItNow::CollectionItem*)));
    connect(list, SIGNAL(deleteRequested(RecordItNow::CollectionItem*)), this, SLOT(deleteItem(RecordItNow::CollectionItem*)));
    connect(list, SIGNAL(playRequested(RecordItNow::CollectionItem*)), this, SIGNAL(playRequested(RecordItNow::CollectionItem*)));
    connect(list, SIGNAL(addRequested(RecordItNow::Collection*)), this, SLOT(createItem(RecordItNow::Collection*)));
    connect(list, SIGNAL(uploadRequested(RecordItNow::CollectionItem*)), this, SIGNAL(uploadRquested(RecordItNow::CollectionItem*)));
    list->setCollection(collection);

    CollectionListWidgetItem *item = new CollectionListWidgetItem(collection, list);
    d->ui->collectionList->addItem(item);

    d->layout->addWidget(list);
    d->layout->setCurrentWidget(list);

}


void CollectionView::removeCollection(RecordItNow::Collection *collection)
{

    for (int i = 0; i < d->layout->count(); i++) {
        CollectionListWidget *list = static_cast<CollectionListWidget*>(d->layout->widget(i));
        if (list->collection() == collection) {
            for (int r = 0; r < d->ui->collectionList->count(); r++) {
                CollectionListWidgetItem *item = d->ui->collectionList->item(r);
                if (item->list() == list) {
                    delete d->ui->collectionList->takeItem(r);
                    break;
                }
            }
            d->layout->removeWidget(list);
        }
    }

}


void CollectionView::ensureVisible(const RecordItNow::CollectionItem *item)
{

    if (item->isCollection()) {
        for (int i = 0; i < d->ui->collectionList->count(); i++) {
            if (d->ui->collectionList->item(i)->item() == item) {
                d->layout->setCurrentWidget(d->ui->collectionList->item(i)->list());
                break;
            }
        }
    } else if (item->collection()) {
        for (int i = 0; i < d->layout->count(); i++) {
            CollectionListWidget *list = static_cast<CollectionListWidget*>(d->layout->widget(i));

            if (list->collection() && list->collection() == item->collection()) {
                list->ensureVisible(item);
                break;
            }
        }
    }

}


void CollectionView::selectItem(const RecordItNow::CollectionItem *item)
{

    if (item->isCollection()) {
        for (int i = 0; i < d->ui->collectionList->count(); i++) {
            if (d->ui->collectionList->item(i)->item() == item) {
                d->ui->collectionList->selectItem(item);
                break;
            }
        }
    } else if (item->collection()) {
        for (int i = 0; i < d->layout->count(); i++) {
            CollectionListWidget *list = static_cast<CollectionListWidget*>(d->layout->widget(i));

            if (list->collection() && list->collection() == item->collection()) {
                list->selectItem(item);
                break;
            }
        }
    }

}


void CollectionView::deleteItem(RecordItNow::CollectionItem *item)
{

    Q_ASSERT(item);

    if (item->isCollection()) {
        for (int i = 0; i < d->ui->collectionList->count(); i++) {
            if (d->ui->collectionList->item(i)->item() == item) {
                delete d->ui->collectionList->takeItem(i);
                break;
            }
        }
    } else {
        item->collection()->takeItem(item);
    }
    item->deleteLater();

}


void CollectionView::editItem(RecordItNow::CollectionItem *item)
{

    Q_ASSERT(item);

    CollectionItemDialog *dialog = new CollectionItemDialog(this);
    dialog->setItem(item);

    dialog->exec();

}


void CollectionView::createItem(RecordItNow::Collection *collection)
{

    if (collection) {
        Q_ASSERT(collection->isCollection());

        CollectionItem *item = new CollectionItem(collection);
        item->setTitle(i18n("untitled"));
        item->setDate(QDateTime::currentDateTime());
        collection->addItem(item);

        editItem(item);
    } else {
        collection = new Collection();
        collection->setTitle(i18n("untitled"));
        collection->setDate(QDateTime::currentDateTime());

        addCollection(collection);
        editItem(collection);
    }

}


} // namespace RecordItNow


#include "collectionview_p.moc"
#include "collectionview.moc"



