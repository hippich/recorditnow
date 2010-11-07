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


#ifndef RECORDITNOW_COLLECTIONLISTSEARCHWIDGET_P_H
#define RECORDITNOW_COLLECTIONLISTSEARCHWIDGET_P_H

// own
#include "ui_collectionlistsearchwidget.h"
#include "collection/collection.h"

// Qt
#include <QtCore/QObject>
#include <QtCore/QWeakPointer>


class QListWidget;
namespace RecordItNow {


class CollectionView;
class CollectionListWidget;
class CollectionListSearchWidget;
class CollectionListSearchWidgetPrivate : public QObject
{
    Q_OBJECT


public:
    CollectionListSearchWidgetPrivate(CollectionListSearchWidget *parent);
    ~CollectionListSearchWidgetPrivate();

    Ui::CollectionListSearchWidget *ui;
    CollectionView *view;
    CollectionListWidget *list;


private:
    CollectionListSearchWidget *q;


private slots:
    void search();


};


} // namespace RecordItNow


#endif // RECORDITNOW_COLLECTIONLISTSEARCHWIDGET_P_H
