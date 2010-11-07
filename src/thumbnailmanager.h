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


#ifndef RECORDITNOW_THUMBNAILMANAGER_H
#define RECORDITNOW_THUMBNAILMANAGER_H

// KDE
#include <kurl.h>

// Qt
#include <QtCore/QObject>


class QPixmap;
namespace RecordItNow {


class ThumbnailManagerPrivate;
class ThumbnailManager : public QObject
{
    Q_OBJECT
    friend class ThumbnailManagerSingleton;
    friend class ThumbnailManagerPrivate;


public:
    ~ThumbnailManager();

    static RecordItNow::ThumbnailManager *self();
    static bool getThumbnail(QPixmap *pixmap, const KUrl &file, const QSize &size);
    static bool updateThumbnail(const KUrl &file, const QSize &size);
    static void cacheThumbnail(const KUrl &file, const QSize &size, const QPixmap *pixmap);


private:
    explicit ThumbnailManager(QObject *parent = 0);

    ThumbnailManagerPrivate *d;


signals:
    void thumbnailUpdated(const KUrl &url, const QSize &size);
    void thumbnailUpdateFailed(const KUrl &url, const QSize &size);


};


} // namespace RecordItNow


#endif // RECORDITNOW_THUMBNAILMANAGER_H
