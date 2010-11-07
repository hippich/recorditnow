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


#ifndef RECORDITNOW_COLLECTIONITEMDELEGATE_H
#define RECORDITNOW_COLLECTIONITEMDELEGATE_H


// Qt
#include <QtGui/QStyledItemDelegate>
#include <QtGui/QTextLayout>



namespace RecordItNow {


class CollectionItemDelegatePrivate;
class CollectionItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT


public:
    explicit CollectionItemDelegate(QAbstractItemView *parent, const bool &itemList = true);
    ~CollectionItemDelegate();

    inline int spacing() const;

    void setContentsMargins(const int &left, const int &top, const int &right, const int &bottom);
    void getContentsMargins(int *left, int *top, int *right, int *bottom) const;
    void setSpacing(const int &spacing);


    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;


private:
    CollectionItemDelegatePrivate *d;


};


} // namespace RecordItNow


#endif // RECORDITNOW_COLLECTIONITEMDELEGATE_H
