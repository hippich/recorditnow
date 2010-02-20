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
#include "keyboardkeypage.h"
#include "keymonmanager.h"

// KDE
#include <kauth.h>
#include <kdebug.h>
#include <kmessagebox.h>

// Qt
#include <QtCore/QTimer>
#include <QtGui/QKeyEvent>


KeyboardKeyPage::KeyboardKeyPage(const QString &device, QWidget *parent)
    : QWizardPage(parent)
{

    m_grab = false;
    m_key = -1;
    m_device = device;

    setupUi(this);

    connect(startButton, SIGNAL(clicked()), this, SLOT(start()));
    connect(KeyMonManager::self(), SIGNAL(keyEvent(KeyMon::Event)), this,
            SLOT(keyEvent(KeyMon::Event)));
    connect(KeyMonManager::self(), SIGNAL(stopped()), this, SLOT(keymonStopped()));

    registerField("KeyCode*", keyCode);

}


KeyboardKeyPage::~KeyboardKeyPage()
{

    stop();

}


void KeyboardKeyPage::initializePage()
{



}


void KeyboardKeyPage::start()
{

    startButton->setDisabled(true);
    m_key = -1;
    keyEdit->clear();

    if (!KeyMonManager::self()->start(QStringList() << m_device)) {
        KMessageBox::error(this, i18n("An error occurd: %1", KeyMonManager::self()->error()));
        stop();
        return;
    }

    grabKeyboard();
    m_grab = true;

}


void KeyboardKeyPage::stop()
{

    if (m_grab) {
        kDebug() << "stop called";
        m_grab = false;
        releaseKeyboard();
        startButton->setDisabled(false);
        KeyMonManager::self()->stop();
    }

}


void KeyboardKeyPage::keyEvent(const KeyMon::Event &event)
{

    if (m_grab) {
        m_key = event.keyCode;
    }

}


void KeyboardKeyPage::keymonStopped()
{

    if (m_grab) {
        stop();
        KMessageBox::error(this, i18n("An error occurd: %1", KeyMonManager::self()->error()));
    }

}


void KeyboardKeyPage::keyReleaseEvent(QKeyEvent *event)
{

    if (!m_grab) {
        QWizardPage::keyReleaseEvent(event);
        return;
    }

    stop();

    kDebug() << "key:" << m_key;
    if (m_key != -1) {
        keyCode->setValue(m_key);
        
        QString text = event->text().trimmed();
        if (text.isEmpty()) {
            text = i18n("Special key");
        }
        keyEdit->setText(text);
    } else {
        KMessageBox::information(this, i18n("Grab failed!\n"
                                            "Perhaps you have selected the wrong device."));
    }

}




#include "keyboardkeypage.moc"
