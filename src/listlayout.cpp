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
#include "listlayout.h"
#include "listlayoutrow.h"

// Qt
#include <QtGui/QWidget>


namespace RecordItNow {
    
    
ListLayout::ListLayout(QWidget *parent)
    : QVBoxLayout(parent)
{
    
    m_spacer = new QWidget;
    m_spacer->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    addWidget(m_spacer);
    
}


ListLayout::~ListLayout()
{
    
    
    
}
    

QList<QWidget*> ListLayout::rows() const
{
    
    QList<QWidget*> list;
    for (int i = 0; i < count(); i++) {
        if (itemAt(i)->widget() && itemAt(i)->widget() != m_spacer) {
            QWidget *widget = static_cast<ListLayoutRow*>(itemAt(i)->widget())->widget();
            list.append(widget);
        }
    }
    return list;
    
}


void ListLayout::addRow(QWidget *widget)
{
    
    removeWidget(m_spacer);
    
    ListLayoutRow *row = new ListLayoutRow(widget);
    connect(row, SIGNAL(removeRequested(RecordItNow::ListLayoutRow*)), this, 
            SLOT(removeClicked(RecordItNow::ListLayoutRow*)));
    addWidget(row);
    
    addWidget(m_spacer);

}


void ListLayout::removeRow(QWidget *widget)
{
    
    bool found = false;
    for (int i = 0; i < count(); i++) {
        if (itemAt(i)->widget() && itemAt(i)->widget() != m_spacer) {
            ListLayoutRow *row = static_cast<ListLayoutRow*>(itemAt(i)->widget());
            if (row->widget() == widget) {
                found = true;
                removeWidget(row);
                delete row;
            }
        }
    }
    Q_ASSERT(found);
    
}
    
    
void ListLayout::clear()
{

    foreach (QWidget *row, rows()) {
        removeRow(row);
    }
    
}

    
void ListLayout::removeClicked(RecordItNow::ListLayoutRow *row)
{

    emit removeRequested(row->widget());
    
}


} // namespace RecordItNow


#include "listlayout.moc"
