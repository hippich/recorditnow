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

#ifndef RECORDITNOW_COLLECTION_H
#define RECORDITNOW_COLLECTION_H


// own
#include "collectionitem.h"

// KDE
#include <kicon.h>


namespace RecordItNow {


class CollectionPrivate;
class Collection : public RecordItNow::CollectionItem
{
    Q_OBJECT
    friend class CollectionPrivate;
    friend class CollectionItemPrivate;
    friend class CollectionItem;


public:
    explicit Collection();
    ~Collection();

    virtual bool isCollection() const;

    QList<RecordItNow::CollectionItem*> items() const;
    RecordItNow::CollectionItem *takeItem(RecordItNow::CollectionItem *item);
    QList<RecordItNow::CollectionItem*> takeItems(const QList<RecordItNow::CollectionItem*> &items);

    QString icon() const;
    void setIcon(const QString &icon);

    virtual QByteArray save() const;
    virtual void load(const QByteArray &data);

    void addItem(RecordItNow::CollectionItem *item);
    void addItems(const QList<RecordItNow::CollectionItem*> &items);


private:
    CollectionPrivate *d;


signals:
    void itemsRemoved(const QList<RecordItNow::CollectionItem*> &items);
    void itemsAdded(const QList<RecordItNow::CollectionItem*> &items);


};


} // namespace RecordItNow



#endif // RECORDITNOW_COLLECTION_H
