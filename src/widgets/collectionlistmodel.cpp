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

// own
#include "collectionlistmodel_p.h"
#include "collectionlistwidgetitem.h"
#include "collectionlistwidget.h"

// KDE
#include <kdebug.h>


namespace RecordItNow {


CollectionListModel::CollectionListModel(CollectionListWidget *parent)
    : QStandardItemModel(parent),
    m_list(parent)
{


}


RecordItNow::CollectionListWidgetItem *CollectionListModel::item(const int &row) const
{

    return static_cast<CollectionListWidgetItem*>(QStandardItemModel::item(row));

}


} // namespace RecordItNow


#include "collectionlistmodel_p.moc"


