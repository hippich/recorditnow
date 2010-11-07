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
#include "collection.h"
#include "collection_p.h"
#include "collectionitem_p.h"

// KDE
#include <kdebug.h>
#include <kpixmapcache.h>

// Qt
#include <QtCore/QDataStream>
#include <QtCore/QFileInfo>


namespace RecordItNow {


typedef QList<RecordItNow::CollectionItem*>::const_iterator CL_IT;
CollectionPrivate::CollectionPrivate(Collection *parent)
    : QObject(parent),
    q(parent)
{

    q->setValue(CollectionItem::CollectionRole, true);

}


CollectionPrivate::~CollectionPrivate()
{

    qDeleteAll(items);

}


void CollectionPrivate::itemChanged(RecordItNow::CollectionItem *item, const RecordItNow::CollectionItem::DataRole &role)
{



}



Collection::Collection()
    : CollectionItem(0),
    d(new CollectionPrivate(this))
{



}


Collection::~Collection()
{

    kDebug();
    delete d;

}


bool Collection::isCollection() const
{

    return true;

}


QList<RecordItNow::CollectionItem*> Collection::items() const
{

    return d->items;

}


RecordItNow::CollectionItem *Collection::takeItem(RecordItNow::CollectionItem *item)
{

    return takeItems(QList<RecordItNow::CollectionItem*>() << item).at(0);

}


QList<RecordItNow::CollectionItem*> Collection::takeItems(const QList<RecordItNow::CollectionItem*> &items)
{

    QList<RecordItNow::CollectionItem*> removed;
    for (CL_IT i = items.constBegin(); i != items.constEnd(); ++i) {
        if (d->items.removeAll(*i) < 1) {
            kWarning() << "Item not found:" << *i;
        } else {
            removed.append((*i));
        }
        (*i)->disconnect(this);
        (*i)->disconnect(d);
    }
    emit itemsRemoved(removed);

    return items;

}


QString Collection::icon() const
{

    return value(IconRole).toString();

}


void Collection::setIcon(const QString &icon)
{

    setValue(IconRole, icon);

}


QByteArray Collection::save() const
{

    QByteArray itemBlob;
    QDataStream itemStream(&itemBlob, QIODevice::WriteOnly);
    for (CL_IT i = d->items.constBegin(); i != d->items.constEnd(); ++i) {
        itemStream << (*i)->save();
    }

    QByteArray blob;
    QDataStream blobStream(&blob, QIODevice::WriteOnly);
    blobStream << CollectionItem::save();
    blobStream << itemBlob;

    return blob;

}


void Collection::load(const QByteArray &data)
{

    QDataStream blobStream(data);
    QByteArray collectionBlob, itemBlob;
    blobStream >> collectionBlob;
    blobStream >> itemBlob;

    CollectionItem::load(collectionBlob);

    QDataStream itemStream(itemBlob);
    while (!itemStream.atEnd()) {
        QByteArray itemData;
        itemStream >> itemData;

        RecordItNow::CollectionItem *item = new RecordItNow::CollectionItem(this);
        item->load(itemData);

        d->items.append(item);
    }

}


void Collection::addItem(RecordItNow::CollectionItem *item)
{

    addItems(QList<RecordItNow::CollectionItem*>() << item);

}


void Collection::addItems(const QList<RecordItNow::CollectionItem*> &items)
{

    for (CL_IT i = d->items.constBegin(); i != d->items.constEnd(); ++i) {
        CollectionItem *item = (*i);
        if (item->parent() != this) {
            item->setParent(this);
        }
        connect(item, SIGNAL(changed(RecordItNow::CollectionItem*,RecordItNow::CollectionItem::DataRole)),
                d, SLOT(itemChanged(RecordItNow::CollectionItem*,RecordItNow::CollectionItem::DataRole)));
    }
    d->items.append(items);

    emit itemsAdded(items);

}



} // namespace RecordItNow


#include "collection_p.moc"
#include "collection.moc"

