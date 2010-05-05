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
#include "topicrow.h"
#include "timeline/topic.h"

// KDE
#include <kicondialog.h>
#include <klineedit.h>
#include <klocalizedstring.h>

// Qt
#include <QtGui/QTimeEdit>
#include <QtGui/QHBoxLayout>


TopicRow::TopicRow(QWidget *parent)
    : QWidget(parent)
{
    
    QHBoxLayout *layout = new QHBoxLayout;
    
    m_titleEdit = new KLineEdit(this);
    m_iconButton = new KIconButton(this);
    m_durationEdit = new QTimeEdit(this);
    
    m_titleEdit->setText(i18n("Untitled"));
    m_iconButton->setIcon("dialog-information");
    m_durationEdit->setDisplayFormat("hh:mm:ss");
    
    m_iconButton->setIconSize(KIconLoader::SizeMedium);
    
    layout->addWidget(m_titleEdit);
    layout->addWidget(m_iconButton);
    layout->addWidget(m_durationEdit);
    
    setLayout(layout);

    
    connect(m_durationEdit, SIGNAL(timeChanged(QTime)), this, SIGNAL(changed()));
    connect(m_iconButton, SIGNAL(iconChanged(QString)), this, SIGNAL(changed()));
    connect(m_titleEdit, SIGNAL(textChanged(QString)), this, SIGNAL(changed()));

}
    


QString TopicRow::title() const
{
    
    return m_titleEdit->text();
    
}


QTime TopicRow::duration() const
{
    
    return m_durationEdit->time();
    
}


QString TopicRow::icon() const
{
    
    return m_iconButton->icon();
    
}


void TopicRow::setTitle(const QString &title)
{
    
    m_titleEdit->setText(title);
    
}

    
void TopicRow::setDuration(const QTime &time)
{
    
    m_durationEdit->setTime(time);
    
}

    
void TopicRow::setIcon(const QString &icon)
{
    
    m_iconButton->setIcon(icon);
    
}


#include "topicrow.moc"
