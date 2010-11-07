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
#include "collectionitem.h"
#include "collectionitem_p.h"
#include "collection.h"
#include "collection_p.h"
#include "thumbnailmanager.h"

// KDE
#include <kurl.h>
#include <kio/previewjob.h>
#include <kio/job.h>
#include <kio/scheduler.h>
#include <kstandarddirs.h>
#include <kdebug.h>
#include <kpixmapcache.h>

// Qt
#include <QtCore/QCryptographicHash>
#include <QtCore/QFile>
#include <QtCore/QMetaType>


namespace RecordItNow {


#define SET_DATA(ROLE, DATA) \
    d->setData(ROLE, qVariantFromValue(DATA))
#define GET_DATA(ROLE, TYPE) \
    d->getData<TYPE>(ROLE);

CollectionItemPrivate::CollectionItemPrivate(CollectionItem *parent)
    : QObject(parent),
    q(parent)
{

    qRegisterMetaTypeStreamOperators<KUrl>("KUrl");

    connect(RecordItNow::ThumbnailManager::self(), SIGNAL(thumbnailUpdated(KUrl,QSize)),
            this, SLOT(thumbnailUpdateFinished(KUrl,QSize)));

    connect(RecordItNow::ThumbnailManager::self(), SIGNAL(thumbnailUpdateFailed(KUrl,QSize)),
            this, SLOT(thumbnailUpdateFailed(KUrl,QSize)));

}


CollectionItemPrivate::~CollectionItemPrivate()
{


}


void CollectionItemPrivate::setData(const int &role, const QVariant &value)
{

    if (data.value(role) != value) {
        data.insert(role, value);

        emit q->changed(q, static_cast<RecordItNow::CollectionItem::DataRole>(role));
    }

}


void CollectionItemPrivate::__debugParent(QObject *parent)
{

    if (!q->isCollection()) {
        Q_ASSERT(parent);
    } else {
        Q_ASSERT(q->collection() == q);
    }

}


void CollectionItemPrivate::thumbnailUpdateFinished(const KUrl &file, const QSize &size)
{

    if (file != q->url()) {
        return;
    }

    QPixmap pixmap;
    if (RecordItNow::ThumbnailManager::getThumbnail(&pixmap, file, size)) {
        q->setThumbnail(pixmap, size);
    }

    emit q->thumbnailUpdateFinished();

}


void CollectionItemPrivate::thumbnailUpdateFailed(const KUrl &file, const QSize &size)
{

    Q_UNUSED(size);
    if (file != q->url()) {
        return;
    }
    emit q->thumbnailUpdateFinished();

}


CollectionItem::CollectionItem(RecordItNow::Collection *parent)
    : QObject(parent),
    d(new CollectionItemPrivate(this))
{

    // Never Call Virtual Functions during Construction or Destruction
    metaObject()->invokeMethod(d, "__debugParent", Qt::QueuedConnection, Q_ARG(QObject*, parent));

}


CollectionItem::~CollectionItem()
{

    delete d;

}


bool CollectionItem::isCollection() const
{

    return false;

}


RecordItNow::Collection *CollectionItem::collection() const
{

    if (isCollection()) {
        return (RecordItNow::Collection*) this;
    } else {
        return static_cast<RecordItNow::Collection*>(parent());
    }

}


QString CollectionItem::title() const
{

    return GET_DATA(TitleRole, QString);

}


QPixmap CollectionItem::thumbnail(const QSize &size) const
{

    QPixmap thumbnail;
    if (!ThumbnailManager::getThumbnail(&thumbnail, url(), size)) {
        thumbnail = QPixmap(0, 0); // null
    }
    return thumbnail;

}


QString CollectionItem::comment() const
{

    return GET_DATA(CommentRole, QString);

}


int CollectionItem::rating() const
{

    return GET_DATA(RatingRole, int);

}


QDateTime CollectionItem::date() const
{

    return GET_DATA(DateRole, QDateTime);

}


KUrl CollectionItem::url() const
{

    return GET_DATA(UrlRole, KUrl);

}


QString CollectionItem::author() const
{

    return GET_DATA(AuthorRole, QString);

}


QVariant CollectionItem::value(const RecordItNow::CollectionItem::DataRole &role) const
{

    return GET_DATA(role, QVariant);

}


void CollectionItem::setValue(const RecordItNow::CollectionItem::DataRole &role, const QVariant &value)
{

    SET_DATA(role, value);

}


QByteArray CollectionItem::save() const
{

    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);

    stream << d->data;

    return data;

}


void CollectionItem::load(const QByteArray &data)
{

    QDataStream stream(data);
    stream >> d->data;

}


void CollectionItem::setTitle(const QString &title)
{

    SET_DATA(TitleRole, title);

}


void CollectionItem::setThumbnail(const QPixmap &thumbnail, const QSize &size)
{

    if (!url().isEmpty()) {
        ThumbnailManager::cacheThumbnail(url(), size, &thumbnail);
        emit changed(this, ThumbnailRole);
    }

}


void CollectionItem::setComment(const QString &comment)
{

    SET_DATA(CommentRole, comment);

}


void CollectionItem::setRating(const int &rating)
{

    SET_DATA(RatingRole, rating);

}


void CollectionItem::setDate(const QDateTime &date)
{

    SET_DATA(DateRole, date);

}


void CollectionItem::setUrl(const KUrl &url)
{

    SET_DATA(UrlRole, url);

}


void CollectionItem::setAuthor(const QString &author)
{

    SET_DATA(AuthorRole, author);

}


void CollectionItem::createThumbnail(const QSize &size)
{

    QPixmap pixmap;
    if (RecordItNow::ThumbnailManager::getThumbnail(&pixmap, url(), size)) {
        setThumbnail(pixmap, size);
    } else {
        if (RecordItNow::ThumbnailManager::updateThumbnail(url(), size)) {
            emit thumbnailUpdateStarted();
        }
    }

}


} // namespace RecordItNow


#include "collectionitem_p.moc"
#include "collectionitem.moc"

