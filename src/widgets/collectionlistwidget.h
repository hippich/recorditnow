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


#ifndef RECORDITNOW_COLLECTIONLISTWIDGET_H
#define RECORDITNOW_COLLECTIONLISTWIDGET_H


// Qt
#include <QtGui/QListView>
#include <QtCore/QFlags>


class QStandardItemModel;
namespace RecordItNow {


class CollectionListWidgetItem;
class CollectionItem;
class CollectionListWidgetPrivate;
class Collection;
class CollectionListWidget : public QListView
{
    Q_OBJECT
    Q_ENUMS(ButtonCode)
    friend class CollectionListWidgetPrivate;


public:
    enum ButtonCode {
        PlayButton = 0x0001,
        AddButton = 0x0002,
        EditButton = 0x0004,
        DeleteButton = 0x0008,
        UploadButton = 0x0020
    };
    Q_DECLARE_FLAGS(ButtonCodes, ButtonCode)

    CollectionListWidget(QWidget *parent = 0, const bool &itemList = false);
    ~CollectionListWidget();

    RecordItNow::Collection *collection() const;
    RecordItNow::CollectionItem *selectedItem() const;
    RecordItNow::CollectionListWidgetItem *item(const int &row) const;
    QList<RecordItNow::CollectionListWidgetItem*> items() const;
    QList<RecordItNow::CollectionListWidgetItem*> selectedItems() const;
    QStandardItemModel *model() const;
    int count() const;
    int row(const RecordItNow::CollectionListWidgetItem *item) const;
    RecordItNow::CollectionListWidgetItem *takeItem(const int &row);

    void addItem(RecordItNow::CollectionListWidgetItem *item);
    void clear();


    void setCollection(RecordItNow::Collection *collection);
    void setButtons(const RecordItNow::CollectionListWidget::ButtonCodes &buttons);
    void ensureVisible(const RecordItNow::CollectionItem *item);
    void selectItem(const RecordItNow::CollectionItem *item);
    void setSelectionModel(QItemSelectionModel *selectionModel);


private:
    CollectionListWidgetPrivate *d;
    void setModel(QAbstractItemModel *model);


protected:
    void resizeEvent(QResizeEvent *event);


signals:
    void playRequested(RecordItNow::CollectionItem *item);
    void editRequested(RecordItNow::CollectionItem *item);
    void deleteRequested(RecordItNow::CollectionItem *item);
    void addRequested(RecordItNow::Collection *collection = 0);
    void uploadRequested(RecordItNow::CollectionItem *item);
    void itemSelectionChanged();


};


} // namespace RecordItNow


Q_DECLARE_OPERATORS_FOR_FLAGS(RecordItNow::CollectionListWidget::ButtonCodes)


#endif // RECORDITNOW_COLLECTIONLISTWIDGET_H
