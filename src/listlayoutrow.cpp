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
#include "listlayoutrow.h"

// KDE
#include <kicon.h>
#include <klocalizedstring.h>
#include <kseparator.h>

// Qt
#include <QtGui/QHBoxLayout>
#include <QtGui/QToolButton>


namespace RecordItNow {

    
ListLayoutRow::ListLayoutRow(QWidget *widget, QWidget *parent)
    : QFrame(parent)
{

    m_widget = widget;

    setMidLineWidth(0);
    setLineWidth(1);
    setFrameStyle(QFrame::Box);
    setFrameShadow(QFrame::Sunken);    

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QToolButton *removeButton = new QToolButton;
    removeButton->setIcon(KIcon("list-remove"));
    removeButton->setText(i18n("Remove"));
    removeButton->setToolTip(i18n("Remove"));
    connect(removeButton, SIGNAL(clicked()), this, SLOT(removeClicked()));

    KSeparator *separator = new KSeparator(Qt::Vertical);
    
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(removeButton);
    layout->addWidget(separator);
    layout->addWidget(m_widget);
    setLayout(layout);
    
}
    

QWidget *ListLayoutRow::widget() const
{
    
    return m_widget;
    
}


void ListLayoutRow::setWidget(QWidget *widget)
{

    if (m_widget) {
        layout()->removeWidget(m_widget);
    }
    m_widget = widget;
    
    if (widget) {
        layout()->addWidget(widget);
    }
    
}


void ListLayoutRow::removeClicked()
{

    emit removeRequested(this);
    
}

    
    
} // namespace RecordItNow


#include "listlayoutrow.moc"
