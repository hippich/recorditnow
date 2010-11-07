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
#include "thumbnailmanager.h"
#include "thumbnailmanager_p.h"

// KDE
#include <kglobal.h>
#include <kpixmapcache.h>
#include <kio/previewjob.h>
#include <kio/job.h>
#include <kdebug.h>

// Qt
#include <QtGui/QPixmap>
#include <QtCore/QFileInfo>


namespace RecordItNow {


ThumbnailManagerPrivate::ThumbnailManagerPrivate(ThumbnailManager *parent)
    : QObject(parent),
    q(parent)
{

    cache = new KPixmapCache("RecordItNow_Thumbnails");

}


ThumbnailManagerPrivate::~ThumbnailManagerPrivate()
{

    delete cache;

}


QString ThumbnailManagerPrivate::getKey(const KUrl &file, const QSize &size) const
{

    QString key = file.pathOrUrl();
    key.append(QString("-%1-%2").arg(size.width()).arg(size.height()));
    return key;

}


bool ThumbnailManagerPrivate::updateThumbnail(const KUrl &file, const QSize &size)
{

    if (file.isEmpty() || !file.isLocalFile() || !QFileInfo(file.path()).exists()) {
        return false;
    }

    QHashIterator<KJob*, ThumbnailUpdateData> it(m_runningJobs);
    while (it.hasNext()) {
        it.next();
        if (it.value().file == file && it.value().size == size) {
            kDebug() << "job for " << file << "is already running...";
            return true;
        }
    }

    KIO::PreviewJob *job = KIO::filePreview(KUrl::List() << file,
                                            size.width(),
                                            size.height(),
                                            0,
                                            0,
                                            false,
                                            false);

    job->setAutoDelete(false);
    connect(job, SIGNAL(gotPreview(KFileItem,QPixmap)), this,
            SLOT(thumbnailUpdateFinished(KFileItem,QPixmap)));

    ThumbnailUpdateData data;
    data.file = file;
    data.size = size;
    m_runningJobs.insert(job, data);

    return true;

}


void ThumbnailManagerPrivate::thumbnailUpdateFailed(const KFileItem &item)
{

    KJob *job = static_cast<KJob*>(sender());

    const ThumbnailUpdateData data = m_runningJobs.take(job);
    kWarning() << "Thumbnail update failed:" << data.file << data.size;    
    job->deleteLater();

    emit q->thumbnailUpdateFailed(data.file, data.size);

}


void ThumbnailManagerPrivate::thumbnailUpdateFinished(const KFileItem &item, const QPixmap &preview)
{

    KJob *job = static_cast<KJob*>(sender());

    const ThumbnailUpdateData data = m_runningJobs.take(job);
    job->deleteLater();

    cache->insert(getKey(data.file, data.size), preview);
    emit q->thumbnailUpdated(data.file, data.size);

}



ThumbnailManager::ThumbnailManager(QObject *parent)
    : QObject(parent),
    d(new ThumbnailManagerPrivate(this))
{


}


ThumbnailManager::~ThumbnailManager()
{

    delete d;

}


class ThumbnailManagerSingleton
{
    public:
    ThumbnailManager self;
};


K_GLOBAL_STATIC(ThumbnailManagerSingleton, privateSelf)


RecordItNow::ThumbnailManager *ThumbnailManager::self()
{

    return &privateSelf->self;

}


bool ThumbnailManager::getThumbnail(QPixmap *pixmap, const KUrl &file, const QSize &size)
{

    if (file.isEmpty() || size.height() == 0 || size.width() == 0) {
        return false;
    }

    ThumbnailManager *manager = ThumbnailManager::self();
    if (manager->d->cache->find(manager->d->getKey(file, size), *pixmap)) {
        return true;
    } else {
        return false;
    }

}


bool ThumbnailManager::updateThumbnail(const KUrl &file, const QSize &size)
{

    return ThumbnailManager::self()->d->updateThumbnail(file, size);

}


void ThumbnailManager::cacheThumbnail(const KUrl &file, const QSize &size, const QPixmap *pixmap)
{

    ThumbnailManager *manager = ThumbnailManager::self();
    manager->d->cache->insert(manager->d->getKey(file, size), *pixmap);

}


} // namespace RecordItNow


#include "thumbnailmanager_p.moc"
#include "thumbnailmanager.moc"
