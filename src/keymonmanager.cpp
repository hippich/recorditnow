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
#include "keymonmanager.h"
#include <config-recorditnow.h>
#include <recorditnow.h>
#include "devicekeylogger/devicekeylogger.h"
#if defined HAVE_RECORDKEYLOGGER
    #include "recordkeylogger/recordkeylogger.h"
#endif

// Qt
#include <QtCore/QTimer>

// KDE
#include <kglobal.h>
#include <kdebug.h>
#include <klocalizedstring.h>



class KeyMonManagerSingleton
{
    public:
        KeyMonManager self;
};


K_GLOBAL_STATIC(KeyMonManagerSingleton, privateSelf)
KeyMonManager::KeyMonManager(QObject *parent)
    : QObject(parent)
{

#if defined HAVE_RECORDKEYLOGGER
    m_logger = new RecordItNow::RecordKeylogger(this);
#else
    m_logger = new DeviceKeylogger(this);
#endif

    connect(m_logger, SIGNAL(keyEvent(RecordItNow::KeyloggerEvent)), this,
            SIGNAL(keyEvent(RecordItNow::KeyloggerEvent)));

    connect(m_logger, SIGNAL(started()), this, SIGNAL(started()));
    connect(m_logger, SIGNAL(stopped()), this, SIGNAL(stopped()));

}


KeyMonManager::~KeyMonManager()
{

    delete m_logger;

}


KeyMonManager *KeyMonManager::self()
{

    return &privateSelf->self;

}


bool KeyMonManager::isRunning() const
{

    return m_logger->isRunning();

}


QString KeyMonManager::error() const
{

    return m_logger->error();

}


bool KeyMonManager::start()
{

    return m_logger->start(Settings::self()->config());

}


void KeyMonManager::stop()
{

    m_logger->stop();

}


bool KeyMonManager::waitForStarted()
{

    return m_logger->waitForStarted();

}


RecordItNow::AbstractKeylogger *KeyMonManager::keylogger() const
{

    return m_logger;

}


#include "keymonmanager.moc"
