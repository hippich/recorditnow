/***************************************************************************
 *   Copyright (C) 210 by Kai Dombrowe <just89@gmx.de>                     *
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
#include "colorrow.h"

// KDE
#include <kcolorbutton.h>

// Qt
#include <QtGui/QHBoxLayout>
#include <QtGui/QToolButton>


ColorRow::ColorRow(QWidget *parent)
    : QFrame(parent)
{

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    
    setMidLineWidth(0);
    setLineWidth(1);
    setFrameStyle(QFrame::Box);
    setFrameShadow(QFrame::Sunken);
    
    QHBoxLayout *layout = new QHBoxLayout;


    QToolButton *removeButton = new QToolButton;
    removeButton->setIcon(KIcon("list-remove"));

    m_button = new MouseButtonWidget;
    m_colorButton = new KColorButton;


    layout->addWidget(removeButton);
    layout->addWidget(m_button);
    layout->addWidget(m_colorButton);


    setLayout(layout);


    connect(removeButton, SIGNAL(clicked()), this, SIGNAL(removeRequested()));
    connect(m_button, SIGNAL(buttonChanged(MouseButtonWidget::Button,MouseButtonWidget::Button)),
            this, SIGNAL(buttonChanged(MouseButtonWidget::Button,MouseButtonWidget::Button)));
    connect(m_colorButton, SIGNAL(changed(QColor)), this, SIGNAL(changed()));

}


QColor ColorRow::color() const
{

    return m_colorButton->color();

}


int ColorRow::code() const
{

    return m_button->getXButton();

}


MouseButtonWidget::Button ColorRow::button() const
{

    return m_button->getMouseButtonWidget();

}


void ColorRow::setColor(const QColor &color)
{

    m_colorButton->setColor(color);

}


void ColorRow::setButton(const MouseButtonWidget::Button &button)
{

    m_button->setButton(button);
    
}


#include "colorrow.moc"

