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
#include "collectionlistwidgetitem.h"
#include "collectionlistwidgetitem_p.h"
#include "collectionlistwidget.h"

// KDE
#include <kicon.h>
#include <kdebug.h>
#include <kiconloader.h>

// Qt
#include <QtGui/QMovie>
#include <QtGui/QPainter>
#include <QtCore/QTimer>


namespace RecordItNow {


CollectionListWidgetItemPrivate::CollectionListWidgetItemPrivate(CollectionListWidgetItem *parent,
                                                                 CollectionItem *item,
                                                                 CollectionListWidget *itemList)
    : QObject(0),
    itemPtr(item),
    list(itemList),
    q(parent)
{

    q->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);

    if (itemPtr) {
        connect(itemPtr.data(), SIGNAL(changed(RecordItNow::CollectionItem*,RecordItNow::CollectionItem::DataRole)),
                this, SLOT(itemDataChanged(RecordItNow::CollectionItem*,RecordItNow::CollectionItem::DataRole)));

        connect(itemPtr.data(), SIGNAL(thumbnailUpdateStarted()), this, SLOT(thumbnailUpdateStarted()));
        connect(itemPtr.data(), SIGNAL(thumbnailUpdateFinished()), this, SLOT(thumbnailUpdateFinished()));

        const QList<CollectionItem::DataRole> roles = QList<CollectionItem::DataRole>()
                                                      << CollectionItem::TitleRole
                                                      << CollectionItem::ThumbnailRole
                                                      << CollectionItem::CommentRole
                                                      << CollectionItem::DateRole
                                                      << CollectionItem::UrlRole
                                                      << CollectionItem::AuthorRole
                                                      << CollectionItem::RatingRole
                                                      << CollectionItem::IconRole
                                                      << CollectionItem::CollectionRole;

        for (int i = 0; i < roles.size(); i++) {
            itemDataChanged(itemPtr.data(), roles.at(i));
        }
    }
    busyTimer = new QTimer(this);
    busyTimer->setInterval(600);
    connect(busyTimer, SIGNAL(timeout()), this, SLOT(thumbnailBusyTick()));

    q->setData(false, CollectionListWidgetItem::ThumbnailBusyRole);

}


CollectionListWidgetItemPrivate::~CollectionListWidgetItemPrivate()
{

    delete busyTimer;

}


void CollectionListWidgetItemPrivate::itemDataChanged(RecordItNow::CollectionItem *self, const RecordItNow::CollectionItem::DataRole &role)
{

    switch (role) {
    case CollectionItem::ThumbnailRole:
        if (list) {
            q->setIcon(KIcon(self->thumbnail(list->iconSize())));
            q->setData(self->thumbnail(list->iconSize()), role);
        }
        break;
    case CollectionItem::IconRole: q->setIcon(KIcon(self->value(role).toString())); break;
    default: q->setData(self->value(role), role); break;
    }

}


void CollectionListWidgetItemPrivate::thumbnailBusyTick()
{

    int tick = busyTimer->property("_RIN_Tick").toInt()+1;
    if (tick < 0 || tick >= 8) {
        tick = 0;
    }

    q->setData(tick, CollectionListWidgetItem::ThumbnailBusyFrameRole);
    busyTimer->setProperty("_RIN_Tick", tick);

}


void CollectionListWidgetItemPrivate::thumbnailUpdateStarted()
{

    q->setThumbnailBusy(true);

}


void CollectionListWidgetItemPrivate::thumbnailUpdateFinished()
{

    q->setThumbnailBusy(false);

}


CollectionListWidgetItem::CollectionListWidgetItem(RecordItNow::CollectionItem *item, RecordItNow::CollectionListWidget *itemList)
    : QStandardItem(),
    d(new CollectionListWidgetItemPrivate(this, item, itemList))
{


}


CollectionListWidgetItem::~CollectionListWidgetItem()
{

    delete d;

}



RecordItNow::CollectionListWidget *CollectionListWidgetItem::list() const
{

    return d->list;

}


RecordItNow::CollectionItem *CollectionListWidgetItem::item() const
{

    if (d->itemPtr) {
        return d->itemPtr.data();
    } else {
        return 0;
    }

}


bool CollectionListWidgetItem::thumbnailBusy() const
{

    return data(ThumbnailBusyRole).toBool();

}


void CollectionListWidgetItem::setThumbnailBusy(const bool &busy)
{

    if (thumbnailBusy() == busy) {
        return;
    }

    setData(busy, ThumbnailBusyRole);
    if (busy) {
        d->busyTimer->start();
    } else {
        d->busyTimer->stop();
    }

}


} // namespace RecordItNow



#include "collectionlistwidgetitem_p.moc"

