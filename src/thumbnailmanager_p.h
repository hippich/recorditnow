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


#ifndef RECORDITNOW_THUMBNAILMANAGER_P_H
#define RECORDITNOW_THUMBNAILMANAGER_P_H

// KDE
#include <kurl.h>
#include <kfileitem.h>

// Qt
#include <QtCore/QObject>
#include <QtGui/QPixmap>

class KPixmapCache;
namespace RecordItNow {


class ThumbnailManager;
class ThumbnailManagerPrivate : public QObject
{
    Q_OBJECT


public:
    ThumbnailManagerPrivate(ThumbnailManager *parent);
    ~ThumbnailManagerPrivate();

    QString getKey(const KUrl &file, const QSize &size) const;

    bool updateThumbnail(const KUrl &file, const QSize &size);

    KPixmapCache *cache;


private:
    struct ThumbnailUpdateData {
        QSize size;
        KUrl file;
    };

    ThumbnailManager *q;

    QHash<KJob*, ThumbnailUpdateData> m_runningJobs;


private slots:
    void thumbnailUpdateFailed(const KFileItem &item);
    void thumbnailUpdateFinished(const KFileItem &item, const QPixmap &preview);


};


} // namespace RecordItNow


#endif // RECORDITNOW_THUMBNAILMANAGER_P_H
