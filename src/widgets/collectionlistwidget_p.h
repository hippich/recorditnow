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


#ifndef RECORDITNOW_COLLECTIONLISTWIDGET_P_H
#define RECORDITNOW_COLLECTIONLISTWIDGET_P_H


// Qt
#include <QtCore/QObject>
#include <QtCore/QWeakPointer>


class QListWidgetItem;
namespace RecordItNow {


class ListWidgetButtonContainer;
class CollectionItem;
class Collection;
class CollectionListWidget;
class CollectionListWidgetPrivate : public QObject
{
    Q_OBJECT
    friend class CollectionListWidget;


public:
    CollectionListWidgetPrivate(CollectionListWidget *parent, const bool &itemList);
    ~CollectionListWidgetPrivate();

    QWeakPointer<Collection> collection;
    ListWidgetButtonContainer *buttonContainer;


private:
    CollectionListWidget *q;


private slots:
    void itemsAdded(const QList<RecordItNow::CollectionItem*> &items);
    void itemsRemoved(const QList<RecordItNow::CollectionItem*> &items);
    void buttonContainerSizeChanged();
    void selectionChanged();
    void addClicked();
    void editClciked();
    void deleteClicked();
    void playClicked();
    void uploadClicked();


};


} // namespace RecordItNow



#endif // RECORDITNOW_COLLECTIONLISTWIDGET_P_H
