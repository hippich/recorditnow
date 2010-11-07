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


#ifndef RECORDITNOW_COLLECTIONVIEW_H
#define RECORDITNOW_COLLECTIONVIEW_H


// Qt
#include <QtGui/QWidget>


namespace RecordItNow {


class CollectionListWidget;
class CollectionItem;
class Collection;
class CollectionViewPrivate;
class CollectionView : public QWidget
{
    Q_OBJECT


public:
    explicit CollectionView(QWidget *parent = 0);
    ~CollectionView();

    QList<RecordItNow::Collection*> collections() const;
    RecordItNow::CollectionItem *selectedItem() const;
    RecordItNow::CollectionListWidget *listForCollection(RecordItNow::Collection *collection) const;

    QByteArray saveState() const;
    void restoreState(const QByteArray &state);

    void addCollection(RecordItNow::Collection *collection);
    void removeCollection(RecordItNow::Collection *collection);

    void ensureVisible(const RecordItNow::CollectionItem *item);
    void selectItem(const RecordItNow::CollectionItem *item);


public slots:
    void deleteItem(RecordItNow::CollectionItem *item);
    void editItem(RecordItNow::CollectionItem *item);
    void createItem(RecordItNow::Collection *collection = 0);


private:
    CollectionViewPrivate *d;


signals:
    void playRequested(RecordItNow::CollectionItem *item);
    void uploadRquested(RecordItNow::CollectionItem *item);
    void currentCollectionChanged(RecordItNow::Collection *collection);


};


} // namespace RecordItNow


#endif // RECORDITNOW_COLLECTIONVIEW_H
