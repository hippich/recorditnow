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
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QDir>


SoundCard::SoundCard()
{



}


QList<SoundCard> SoundCard::cards()
{

    QList<SoundCard> cards;

    // asound
    QDir asound("/proc/asound");
    if (!asound.exists()) {
        kWarning() << "/proc/asound does not exists.";
        return cards;
    }

    const QRegExp cardRX("^card[0-9]+$");
    const QStringList subDirs = asound.entryList(QStringList(), QDir::Dirs|QDir::NoDotAndDotDot);
    foreach (const QString &dir, subDirs) {
        if (cardRX.exactMatch(dir)) {
            kDebug() << "found card:" << dir;
            cards.append(scanASoundCard("/proc/asound/"+dir));
        }
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


QList<SoundCard> SoundCard::scanASoundCard(const QString &dir)
{

    QList<SoundCard> cards;
    const QRegExp pcmRX("^pcm[0-9]+c$");

    QDir cardDir(dir);
    const QStringList subDirs = cardDir.entryList(QStringList(), QDir::Dirs|QDir::NoDotAndDotDot);
    foreach (const QString &subDir, subDirs) {
        if (pcmRX.exactMatch(subDir)) {
            kDebug() << "found pcm:" << subDir;

            QFile info(dir+'/'+subDir+"/info");
            if (!info.exists()) {
                kWarning() << "info does not exists:" << info.fileName();
                continue;
            }

            if (!info.open(QIODevice::ReadOnly|QIODevice::Text)) {
                kWarning() << "open failed:" << info.error() << info.errorString();
                continue;
            }

            QTextStream stream(&info);
            SoundCard card;

            QString CARD, DEVICE;
            QString line = stream.readLine();
            while (!line.isNull()) {
                if (line.startsWith(QLatin1String("name:"))) {
                    card.m_name = line.remove("name:").trimmed();
                } else if (line.startsWith(QLatin1String("card:"))) {
                    CARD = line.remove("card:").trimmed();
                } else if (line.startsWith(QLatin1String("device:"))) {
                    DEVICE = line.remove("device:").trimmed();
                }
                line = stream.readLine();
            }
            info.close();

            card.m_key = QString("hw:%1,%2").arg(CARD).arg(DEVICE);
            card.m_icon = "audio-card";
            cards.append(card);
        }
    }
    return cards;

}
