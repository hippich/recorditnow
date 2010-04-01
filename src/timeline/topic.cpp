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
    : RecordItNow::ConfigItem()
{

    setCurrentSecond(0);

}


Topic::~Topic()
{


}


unsigned long Topic::currentSecond() const
{

    return data<unsigned long>("CurrentSecond");

}


QString Topic::icon() const
{

    return data<QString>("Icon");

}


QString Topic::title() const
{

    return data<QString>("Title");

}


QTime Topic::duration() const
{

    return data<QTime>("Duration");

}


unsigned long Topic::durationToSeconds() const
{

    unsigned long seconds = 0;
    const QTime durationTime = duration();

    seconds += durationTime.second();
    seconds += durationTime.minute()*60;
    seconds += (durationTime.hour()*60)*60;

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

    return m_progressBar.data() ? true : false;

}


void Topic::setCurrentSecond(const unsigned long &second)
{

    setData("CurrentSecond", second);

    if (duration() != QTime()) {
        if (m_progressBar.data()) {
            m_progressBar.data()->setValue(second);
        }
    } else {
        if (m_progressBar.data()) {
            m_progressBar.data()->setValue(0);
        }
    }

}


void Topic::setIcon(const QString &icon)
{

    setData("Icon", icon);

}


void Topic::setTitle(const QString &title)
{

    setData("Title", title);

}


void Topic::setDuration(const QTime &duration)
{

    setData("Duration", duration);
    if (m_progressBar.data()) {
        m_progressBar.data()->setMaximum(durationToSeconds());
    }

}


void Topic::setProgressBar(TopicProgressBar *bar)
{

    m_progressBar = bar;
    m_progressBar.data()->setMaximum(durationToSeconds());

}


bool Topic::operator!=(const Topic &other) const
{

    return (other.title() != title() && other.duration() != duration() && other.icon() != icon());

}


} // namespace Timeline


} // namespace RecordItNow



