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

    
ListLayoutRow::ListLayoutRow(QWidget *widget, QWidget *parent, const bool &moveEnabled)
    : QFrame(parent)
{

    m_widget = widget;
    m_upButton = m_downButton = 0;

    setMidLineWidth(0);
    setLineWidth(1);
    setFrameStyle(QFrame::Box);
    setFrameShadow(QFrame::Sunken);    

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QToolButton *removeButton = new QToolButton(this);
    removeButton->setIcon(KIcon("list-remove"));
    removeButton->setText(i18n("Remove"));
    removeButton->setToolTip(i18n("Remove"));
    connect(removeButton, SIGNAL(clicked()), this, SLOT(removeClicked()));

    KSeparator *separator = new KSeparator(Qt::Vertical);
    
    QHBoxLayout *layout = new QHBoxLayout;
    
    if (moveEnabled) {
        QToolButton *upButton = new QToolButton(this);
        upButton->setIcon(KIcon("go-up"));
        upButton->setText(i18n("Move up"));
        upButton->setToolTip(i18n("Move up"));
    
        QToolButton *downButton = new QToolButton(this);
        downButton->setIcon(KIcon("go-down"));
        downButton->setText(i18n("Move down"));
        downButton->setToolTip(i18n("Move down"));
    
        connect(upButton, SIGNAL(clicked()), this, SLOT(upClicked()));
        connect(downButton, SIGNAL(clicked()), this, SLOT(downClicked()));
        
        layout->addWidget(upButton);
        layout->addWidget(downButton);
    
        m_upButton = upButton;
        m_downButton = downButton;
    }
    
    layout->addWidget(removeButton);
    layout->addWidget(separator);
    layout->addWidget(m_widget);
    setLayout(layout);
    
}
    

QWidget *ListLayoutRow::widget() const
{
    
    return m_widget;
    
}


void ListLayoutRow::removeWidget()
{

    if (m_widget) {
        if (m_widget->parentWidget() == this) {
            m_widget->setParent(parentWidget());
        }
        layout()->removeWidget(m_widget);
    }
    m_widget = 0;
    
}


void ListLayoutRow::setUpEnabled(const bool &enabled)
{

    if (m_upButton) {
        m_upButton->setEnabled(enabled);
    }

}

    
void ListLayoutRow::setDownEnabled(const bool &enabled)
{

    if (m_downButton) {
        m_downButton->setEnabled(enabled);
    }

}


void ListLayoutRow::removeClicked()
{

    emit removeRequested(this);
    
}

    
void ListLayoutRow::upClicked()
{

    emit upRequested(this);
    
}


void ListLayoutRow::downClicked()
{

    emit downRequested(this);
    
}

    
} // namespace RecordItNow


#include "listlayoutrow.moc"
