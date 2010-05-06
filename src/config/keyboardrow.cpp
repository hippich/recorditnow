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
#include "keyboardrow.h"

// KDE
#include <kicondialog.h>

// Qt
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>


KeyboardRow::KeyboardRow(QWidget *parent)
    : QWidget(parent)
{

    QHBoxLayout *layout = new QHBoxLayout;
    
    m_iconButton = new KIconButton(this);
    m_textLabel = new QLabel(this);
    
    m_code = -1;
    m_iconButton->setIconSize(KIconLoader::SizeMedium);
    m_iconButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_textLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    
    layout->addWidget(m_iconButton);
    layout->addWidget(m_textLabel);
    
    setLayout(layout);
    
    connect(m_iconButton, SIGNAL(iconChanged(QString)), this, SIGNAL(changed()));

}


QString KeyboardRow::icon() const
{

    return m_iconButton->icon();
    
}


QString KeyboardRow::text() const
{

    return m_textLabel->text();
    
}


int KeyboardRow::code() const
{

    return m_code;
    
}


void KeyboardRow::setIcon(const QString &icon)
{

    m_iconButton->setIcon(icon);
    
}


void KeyboardRow::setText(const QString &text)
{

    m_textLabel->setText(text);
    
}


void KeyboardRow::setCode(const int &code)
{

    m_code = code;
    
}


#include "keyboardrow.moc"
