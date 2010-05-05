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

// KDE
#include <kdebug.h>

// Qt
#include <QtGui/QWidget>
#include <QtGui/QLayoutItem>


namespace RecordItNow {
    
    
ListLayout::ListLayout(QWidget *parent, const bool &moveEnabled)
    : QVBoxLayout(parent)
{
    
    m_spacer = new QWidget;
    m_spacer->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    addWidget(m_spacer);
   
    m_moveEnabled = moveEnabled;

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


QWidget *ListLayout::take(QWidget *widget)
{

    for (int i = 0; i < count(); i++) {
        if (itemAt(i)->widget() && itemAt(i)->widget() != m_spacer) {
            ListLayoutRow *row = static_cast<ListLayoutRow*>(itemAt(i)->widget());
            if (row->widget() == widget) {
                removeWidget(row);
                
                row->removeWidget();
                delete row;
                
                return widget;
            }
        }
    }
    updateButtons();
    return 0;
    
}


void ListLayout::addRow(QWidget *widget)
{
    
    insertRow(widget, count());
    updateButtons();

}


void ListLayout::insertRow(QWidget *widget, const int &index)
{

    removeWidget(m_spacer);

    ListLayoutRow *row = new ListLayoutRow(widget, parentWidget(), m_moveEnabled);
    
    connect(row, SIGNAL(removeRequested(RecordItNow::ListLayoutRow*)), this, 
            SLOT(removeClicked(RecordItNow::ListLayoutRow*)));
    connect(row, SIGNAL(upRequested(RecordItNow::ListLayoutRow*)), this, 
            SLOT(upRequested(RecordItNow::ListLayoutRow*)));
    connect(row, SIGNAL(downRequested(RecordItNow::ListLayoutRow*)), this, 
            SLOT(downRequested(RecordItNow::ListLayoutRow*)));
    
    insertWidget(index, row);
    
    addWidget(m_spacer);

    updateButtons();

}


void ListLayout::removeRow(QWidget *widget)
{
    
    delete take(widget);

}
    
    
void ListLayout::clear()
{

    foreach (QWidget *row, rows()) {
        removeRow(row);
    }
    
}


void ListLayout::move(const int &from, const int &to)
{

    QLayoutItem *item = itemAt(from);
    removeItem(item);
    insertItem(to, item);
    
    updateButtons();
    
    emit layoutChanged();

}


void ListLayout::updateButtons()
{

    // update up/down buttons
    for (int i = 0; i < count(); i++) {
        if (itemAt(i)->widget() && itemAt(i)->widget() != m_spacer) {
            ListLayoutRow *widget = static_cast<ListLayoutRow*>(itemAt(i)->widget());
            
            if (i+3 > count()) {
                widget->setDownEnabled(false);
                widget->setUpEnabled(count() > 2);
            } else if (i == 0) {
                widget->setUpEnabled(false);
                widget->setDownEnabled(count() > 2);
            } else {
                widget->setUpEnabled(true);
                widget->setDownEnabled(true);
            }
        }
    }

}


void ListLayout::removeClicked(RecordItNow::ListLayoutRow *row)
{

    emit removeRequested(row->widget());
    
}

    
void ListLayout::upRequested(RecordItNow::ListLayoutRow *row)
{
    
    const int currentIndex = indexOf(row);
    if (currentIndex-1 < 0) {
        return;
    }
    move(currentIndex, currentIndex-1); 
    
}


void ListLayout::downRequested(RecordItNow::ListLayoutRow *row)
{
    
    const int currentIndex = indexOf(row);
    if (currentIndex+3 > count()) {
        return;
    }
    move(currentIndex, currentIndex+1);  
            
}
    
    
} // namespace RecordItNow


#include "listlayout.moc"
