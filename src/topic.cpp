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
#include "topic.h"

// KDE
#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>

// Qt
#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QToolButton>
#include <QtGui/QStyle>
#include <QtGui/QStyleOptionProgressBarV2>


Topic::Topic(QWidget *parent, const QTime duration, const QString &title, const QString &icon)
    : QProgressBar(parent)
{

    m_currentSecond = 0;
    m_icon = icon;

    setTextVisible(false);

    m_button = new QToolButton(this);
    m_button->setAutoRaise(true);
    m_titleLabel = new QLabel(this);
    m_timeLabel = new QLabel(this);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(m_button);
    layout->addWidget(m_titleLabel);
    layout->addWidget(m_timeLabel);
    setLayout(layout);

    setTitle(title);
    setIcon(icon);
    m_duration = duration;

    setMaximum(durationToSeconds());
    setValue(0);

    updateSize();

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

    return m_titleLabel->text();

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

    while (secs >= hour) {
        hours++;
        secs -= hour;
    }

    while (secs >= minute) {
        minutes++;
        secs -= minute;
    }

    return QTime(hours, minutes, secs, 0);

}


void Topic::setCurrentSecond(const unsigned long &second)
{

    m_currentSecond = second;

    if (m_duration != QTime()) {
        setValue(second);
    } else {
        setValue(0);
    }

}


void Topic::setIcon(const QString &icon)
{

    m_button->setIcon(KIcon(icon));

}


void Topic::setTitle(const QString &title)
{

    m_titleLabel->setText(title);

}


void Topic::updateSize()
{

    QStyleOptionProgressBarV2 option;
    initStyleOption(&option);

    QRect rect = style()->subElementRect(QStyle::SE_ProgressBarContents, &option, this);

    setMinimumHeight(rect.height());

}


bool Topic::event(QEvent *event)
{

    if (event->type() == QEvent::StyleChange || event->type() == QEvent::FontChange) {
        updateSize();
    }

    return QProgressBar::event(event);

}


#include "topic.moc"

