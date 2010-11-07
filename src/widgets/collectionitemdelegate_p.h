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


#ifndef RECORDITNOW_COLLECTIONITEMDELEGATE_P_H
#define RECORDITNOW_COLLECTIONITEMDELEGATE_P_H


//Qt
#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtCore/QRectF>
#include <QtGui/QFont>
#include <QtGui/QTextOption>
#include <QtGui/QAbstractItemView>

class QToolButton;
class QStyleOptionViewItem;
class QModelIndex;
namespace RecordItNow {

typedef QHash<QString, QRect> Layout;
class CollectionItemDelegate;
class CollectionItemDelegatePrivate : public QObject
{
    Q_OBJECT


public:
    CollectionItemDelegatePrivate(CollectionItemDelegate *parent, QAbstractItemView *v, const bool &itemList);
    ~CollectionItemDelegatePrivate();

    bool itemList;

    QAbstractItemView *view;

    int spacing;
    int leftMargin;
    int topMargin;
    int rightMargin;
    int bottomMargin;

    QList<QPixmap> busyFrames;

    inline QStyle *style() const;
    inline QString getText(const int &role, const QModelIndex *index) const;
    inline QFont titleFont(const QStyleOptionViewItem *option) const;
    inline QFont textFont(const QStyleOptionViewItem *option) const;
    inline QHash<QString, QRect> getLayout(const QModelIndex *index, const QStyleOptionViewItem *option, const bool &minimum = false) const;
    inline QString displayText(const QRect &boundingRect, QPainter *painter, const QTextOption &options, const QString &text) const;
    inline QPalette::ColorGroup colorGroup(const QStyle::State &state) const;

    inline void drawText(QPainter *painter, const QString &text, const QStyleOptionViewItem *option, const QRect &rect, const bool &title = false) const;
    inline void drawBackground(QPainter *painter, const QStyleOptionViewItem *option, const QModelIndex *index, const QRect &rect, const QRect &contentsRect) const;
    inline void drawThumbnail(QPainter *painter, const QStyleOptionViewItem *option, const QModelIndex *index, const QRect &rect) const;


private:
    CollectionItemDelegate *q;


};


} // namespace RecordItNow



#endif // RECORDITNOW_COLLECTIONITEMDELEGATE_P_H
