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


#ifndef RECORDITNOW_COLLECTIONLISTWIDGETITEM_P_H
#define RECORDITNOW_COLLECTIONLISTWIDGETITEM_P_H

// own
#include "collection/collectionitem.h"

// Qt
#include <QtCore/QObject>
#include <QtCore/QWeakPointer>


class QTimer;
namespace RecordItNow {


class CollectionListWidget;
class CollectionListWidgetItem;
class CollectionListWidgetItemPrivate : public QObject
{
    Q_OBJECT


public:
    CollectionListWidgetItemPrivate(CollectionListWidgetItem *parent, CollectionItem *item, CollectionListWidget *itemList);
    ~CollectionListWidgetItemPrivate();

    QWeakPointer<CollectionItem> itemPtr;
    CollectionListWidget *list;
    QTimer *busyTimer;


private:
    CollectionListWidgetItem *q;


private slots:
    void itemDataChanged(RecordItNow::CollectionItem *self, const RecordItNow::CollectionItem::DataRole &role);
    void thumbnailBusyTick();
    void thumbnailUpdateStarted();
    void thumbnailUpdateFinished();


};


}


#endif // RECORDITNOW_COLLECTIONLISTWIDGETITEM_P_H
