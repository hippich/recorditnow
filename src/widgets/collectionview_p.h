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

#ifndef RECORDITNOW_COLLECTIONVIEW_P_H
#define RECORDITNOW_COLLECTIONVIEW_P_H


// own
#include "ui_collectionview.h"
#include "collection/collectionitem.h"

// Qt
#include <QtCore/QObject>


class QStackedLayout;
namespace RecordItNow {


class CollectionListFrame;
class CollectionListWidget;
class Collection;
class CollectionView;
class CollectionViewPrivate : public QObject
{
    Q_OBJECT


public:
    CollectionViewPrivate(CollectionView *parent);
    ~CollectionViewPrivate();

    CollectionListWidget *currentList() const;


    Ui::CollectionView *ui;
    QStackedLayout *layout;
    CollectionListWidget *searchList;


private:
    CollectionView *q;


private slots:
    void currentCollectionListChanged(const int &index);
    void collectionSelectionChanged();
    void currentItemChanged();
    void activateSearchList();


};


}


#endif // RECORDITNOW_COLLECTIONVIEW_P_H


