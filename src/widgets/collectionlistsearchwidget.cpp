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
#include "collectionlistsearchwidget.h"
#include "collectionlistsearchwidget_p.h"
#include "collectionitemdelegate.h"
#include "collectionlistwidgetitem.h"
#include "collectionlistwidget.h"
#include "collectionview.h"
#include "collection/collection.h"

// KDE
#include <kglobal.h>
#include <klocale.h>
#include <kicon.h>

// Qt
#include <QtGui/QListWidget>


namespace RecordItNow {


CollectionListSearchWidgetPrivate::CollectionListSearchWidgetPrivate(CollectionListSearchWidget *parent)
    : QObject(parent),
    q(parent)
{

    list = 0;
    view = 0;

    ui = new Ui::CollectionListSearchWidget;
    ui->setupUi(q);

    ui->searchButton->setIcon(KIcon("system-search"));
    connect(ui->searchButton, SIGNAL(clicked()), this, SLOT(search()));

}


CollectionListSearchWidgetPrivate::~CollectionListSearchWidgetPrivate()
{

    delete ui;

}


void CollectionListSearchWidgetPrivate::search()
{

    const QString text = ui->searchLine->text();
    if (text.isEmpty() && list) {
        list->clear();
    }

    if (!view || !list || text.isEmpty()) {
        return;
    }

    CollectionItem::DataRole role;
    switch (ui->searchBox->currentIndex()) {
    case 0: role = CollectionItem::TitleRole; break;
    case 1: role = CollectionItem::CommentRole; break;
    case 2: role = CollectionItem::AuthorRole; break;
    case 3: role = CollectionItem::RatingRole; break;
    case 4: role = CollectionItem::UrlRole; break;
    default: return;
    }
    list->clear();

    foreach (Collection *collection, view->collections()) {
        CollectionListWidget *currentList = view->listForCollection(collection);
        Q_ASSERT(currentList);

        const QAbstractItemModel *model = currentList->model();
        const QModelIndexList indexes = model->match(model->index(0, 0, QModelIndex()),
                                                     (int) role,
                                                     QVariant(text),
                                                     -1,
                                                     Qt::MatchContains|Qt::MatchRecursive);

        for (int i = 0; i < indexes.size(); ++i) {
            CollectionListWidgetItem *item = new CollectionListWidgetItem(currentList->item(indexes.at(i).row())->item(), list);
            list->addItem(item);
        }
    }

    emit q->activated();

}



CollectionListSearchWidget::CollectionListSearchWidget(QWidget *parent)
    : QWidget(parent),
    d(new CollectionListSearchWidgetPrivate(this))
{


}


CollectionListSearchWidget::~CollectionListSearchWidget()
{

    delete d;

}


void CollectionListSearchWidget::setListWidget(RecordItNow::CollectionListWidget *list)
{

    d->list = list;

}


void CollectionListSearchWidget::setView(RecordItNow::CollectionView *view)
{

    d->view = view;

}


} // namespace RecordItNow


#include "collectionlistsearchwidget_p.moc"
#include "collectionlistsearchwidget.moc"

