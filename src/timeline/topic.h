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

#ifndef RECORDITNOW_TOPIC_H
#define RECORDITNOW_TOPIC_H

// own
#include "configitem.h"

//KDE
#include <kicon.h>

// Qt
#include <QtCore/QTime>
#include <QtCore/QObject>
#include <QtCore/QWeakPointer>


namespace RecordItNow {


class TopicProgressBar;
class Topic : public RecordItNow::ConfigItem
{


public:
    explicit Topic();
    ~Topic();

    unsigned long currentSecond() const;
    QString icon() const;
    QString title() const;
    QTime duration() const;
    unsigned long durationToSeconds() const;
    static QTime secondsToTime(const unsigned long seconds);
    bool isValid() const;

    void setCurrentSecond(const unsigned long &second);
    void setIcon(const QString &icon);
    void setTitle(const QString &title);
    void setDuration(const QTime &duration);
    void setProgressBar(TopicProgressBar *bar);

    bool operator!=(const Topic &other) const;


private:
    QWeakPointer<TopicProgressBar> m_progressBar;


};


} // namespace RecordItNow


#endif // RECORDITNOW_TOPIC_H
