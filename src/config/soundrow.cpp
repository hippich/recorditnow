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
#include "soundrow.h"
#include "helper.h"

// KDE
#include <kurlrequester.h>
#include <kstandarddirs.h>

// Qt
#include <QtGui/QHBoxLayout>
#include <QtGui/QToolButton>


SoundRow::SoundRow(QWidget *parent)
    : QWidget(parent)
{
    
    QHBoxLayout *layout = new QHBoxLayout;

    m_button = new MouseButtonWidget;
    m_requester = new KUrlRequester;

    m_requester->setMode(KFile::File|KFile::LocalOnly);
    const QStringList dirs = KGlobal::dirs()->resourceDirs("sound");
    if (!dirs.isEmpty()) { // FIXME
        m_requester->setStartDir(dirs.last());
    }
    

    QToolButton *playButton = new QToolButton;
    playButton->setIcon(KIcon("media-playback-start"));

    layout->addWidget(m_button);
    layout->addWidget(m_requester);
    layout->addWidget(playButton);

    setLayout(layout);


    connect(m_button, SIGNAL(buttonChanged(MouseButtonWidget::Button,MouseButtonWidget::Button)),
            this, SIGNAL(buttonChanged(MouseButtonWidget::Button,MouseButtonWidget::Button)));
    connect(playButton, SIGNAL(clicked()), this, SLOT(playClicked()));
    connect(m_requester, SIGNAL(textChanged(QString)), this, SIGNAL(changed()));

}


QString SoundRow::sound() const
{

    return m_requester->text();

}


int SoundRow::code() const
{

    return m_button->getXButton();

}


MouseButtonWidget::Button SoundRow::button() const
{

    return m_button->getMouseButtonWidget();

}


void SoundRow::setSound(const QString &sound)
{

    m_requester->setText(sound);

}


void SoundRow::setButton(const MouseButtonWidget::Button &button)
{

    m_button->setButton(button);
    
}


void SoundRow::playClicked()
{

    if (!sound().isEmpty()) {
        RecordItNow::Helper::self()->playSound(sound());
    }
    
}


#include "soundrow.moc"

