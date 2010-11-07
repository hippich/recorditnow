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


#ifndef RECORDITNOW_COLLECTIONITEM_P_H
#define RECORDITNOW_COLLECTIONITEM_P_H


// KDE
#include <kurl.h>
#include <kfileitem.h>

// Qt
#include <QtCore/QObject>
#include <QtCore/QDateTime>
#include <QtGui/QPixmap>

class KJob;
namespace RecordItNow {


class CollectionItem;
class CollectionItemPrivate : public QObject
{
    Q_OBJECT


public:
    CollectionItemPrivate(CollectionItem *parent);
    ~CollectionItemPrivate();

    template < typename T >
    inline T getData(const int &role) const { return data.value(role).value<T>(); };
    void setData(const int &role, const QVariant &value);

    QHash<int, QVariant> data;


private:
    CollectionItem *q;


private slots:
    void __debugParent(QObject *parent);

    void thumbnailUpdateFinished(const KUrl &file, const QSize &size);
    void thumbnailUpdateFailed(const KUrl &file, const QSize &size);


};


}


#endif // RECORDITNOW_COLLECTIONITEM_P_H
