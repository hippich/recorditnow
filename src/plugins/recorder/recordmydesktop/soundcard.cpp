/***************************************************************************
 *   Copyright (C) 2009 by Kai Dombrowe <just89@gmx.de>                    *
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
#include "soundcard.h"

// KDE
#include <kdebug.h>

// Qt
#include <QtCore/QStringList>

// Phonon
#include <phonon/backendcapabilities.h>


using namespace Phonon;
SoundCard::SoundCard()
{



}


QList<SoundCard> SoundCard::cards()
{

    QList<SoundCard> cards;
    foreach (const AudioOutputDevice &dev, BackendCapabilities::availableAudioOutputDevices()) {
        QStringList deviceIds = dev.property("deviceIds").toStringList();
        if (deviceIds.isEmpty()) {
            continue;
        }
        QString key = "hw:";
        QString id = deviceIds.first();

        const QRegExp cardRX("CARD=.*");
        cardRX.indexIn(id);
        if (cardRX.cap().isEmpty()) {
            continue;
        }
        key += cardRX.cap().remove(QRegExp(",.*")).remove("CARD=");

        key += ',';

        const QRegExp devRX("DEV=.*");
        devRX.indexIn(id);
        if (devRX.cap().isEmpty()) {
            continue;
        }
        key += devRX.cap().remove(QRegExp(",.*")).remove("DEV=");

        SoundCard card;
        card.m_key = key;
        card.m_icon = dev.property("icon").toString();
        card.m_name = dev.property("name").toString();
        cards.append(card);
    }
    return cards;

}


QString SoundCard::name() const
{

    return m_name;

}


QString SoundCard::key() const
{

    return m_key;

}


QString SoundCard::icon() const
{

    return m_icon;

}
