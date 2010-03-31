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
#include "topic.h"
#include "topicprogressbar.h"

// KDE
#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>

// Qt

namespace RecordItNow {


namespace Timeline {


Topic::Topic()
{

    m_currentSecond = 0;
    m_progressBar = 0;

}


Topic::~Topic()
{


}


unsigned long Topic::currentSecond() const
{

    return m_currentSecond;

}


QString Topic::icon() const
{

    return m_icon;

}


QString Topic::title() const
{

    return m_title;

}


QTime Topic::duration() const
{

    return m_duration;

}


unsigned long Topic::durationToSeconds() const
{

    unsigned long seconds = 0;

    seconds += m_duration.second();
    seconds += m_duration.minute()*60;
    seconds += (m_duration.hour()*60)*60;

    return seconds;

}


QTime Topic::secondsToTime(const unsigned long seconds)
{

    unsigned long secs = seconds;

    int hours(0), minutes(0);
    int hour(3600), minute(60);

    while (secs >= (unsigned long)hour) {
        hours++;
        secs -= hour;
    }

    while (secs >= (unsigned long)minute) {
        minutes++;
        secs -= minute;
    }

    return QTime(hours, minutes, secs, 0);

}


bool Topic::isValid() const
{

    return m_progressBar ? true : false;

}


void Topic::setCurrentSecond(const unsigned long &second)
{

    m_currentSecond = second;

    if (m_duration != QTime()) {
        if (m_progressBar) {
            m_progressBar->setValue(second);
        }
    } else {
        if (m_progressBar) {
            m_progressBar->setValue(0);
        }
    }

}


void Topic::setIcon(const QString &icon)
{

    m_icon = icon;

}


void Topic::setTitle(const QString &title)
{

    m_title = title;

}


void Topic::setDuration(const QTime &duration)
{

    m_duration = duration;
    if (m_progressBar) {
        m_progressBar->setMaximum(durationToSeconds());
    }

}


void Topic::setProgressBar(TopicProgressBar *bar)
{

    m_progressBar = bar;
    m_progressBar->setMaximum(durationToSeconds());

}


bool Topic::operator!=(const Topic &other) const
{

    return (other.title() != title() && other.duration() != duration() && other.icon() != icon());

}


} // namespace Timeline


} // namespace RecordItNow


QDataStream &operator<<(QDataStream &stream, const RecordItNow::Timeline::Topic &data)
{

    stream << data.duration();
    stream << data.title();
    stream << data.icon();

    return stream;

}


QDataStream &operator>>(QDataStream &stream, RecordItNow::Timeline::Topic &data)
{

    QTime duration;
    QString title;
    QString icon;

    stream >> duration;
    stream >> title;
    stream >> icon;

    data.setDuration(duration);
    data.setTitle(title);
    data.setIcon(icon);

    return stream;

}



