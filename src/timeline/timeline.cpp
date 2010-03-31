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
#include "timeline.h"
#include "topicprogressbar.h"
#include "helper.h"
#include "config/timelineconfig.h"
#include <recorditnow.h>

// KDE
#include <kdebug.h>
#include <knotification.h>

// Qt
#include <QtCore/QTimer>


Timeline::Timeline(QWidget *parent)
    : QWidget(parent)
{

    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(updateTime()));

    setupUi(this);

    connect(topicWidget, SIGNAL(topicChanged(RecordItNow::Timeline::Topic)), this,
            SLOT(topicChanged(RecordItNow::Timeline::Topic)));
    connect(topicWidget, SIGNAL(durationChanged(ulong)), this, SLOT(durationChanged(ulong)));
    connect(topicWidget, SIGNAL(topicChanged(RecordItNow::Timeline::Topic)), this,
            SIGNAL(currentTopicChanged(RecordItNow::Timeline::Topic)));

    slider->setMaximum(0);
    resetSlider();
    setState(Idle);

}


Timeline::~Timeline()
{

    delete m_timer;

}


unsigned long Timeline::duration() const
{

    return slider->maximum();

}


void Timeline::reload()
{

    topicWidget->clear();

    QList<RecordItNow::Timeline::Topic> topics = TimelineConfig::loadTopics(Settings::self()->config());
    foreach (const RecordItNow::Timeline::Topic &topic, topics) {
        topicWidget->addTopic(topic);
    }
    resetSlider();

}


void Timeline::start()
{

    if (state() != Idle) {
        return;
    }

    setState(Running);
    slider->setValue(0);
    m_timer->start(1000);

}


void Timeline::stop()
{

    if (m_state == Idle) {
        return;
    }
    setState(Idle);
    m_timer->stop();

    QTimer::singleShot(100, this, SLOT(resetSlider()));

}


void Timeline::pauseOrContinue()
{

    if (state() == Paused) {
        m_timer->start(1000);
        setState(Running);
    } else if (state() == Running) {
        setState(Paused);
        m_timer->stop();
    }

}


Timeline::State Timeline::state() const
{

    return m_state;

}


void Timeline::setState(const State &state)
{

    m_state = state;

}


void Timeline::updateTime()
{

    slider->setValue(slider->value()+1);
    topicWidget->setCurrentSecond(slider->value());

    const QString total = KGlobal::locale()->formatTime(RecordItNow::Timeline::Topic::secondsToTime(slider->maximum()),
                                                        true, true);
    const QString passed = KGlobal::locale()->formatTime(RecordItNow::Timeline::Topic::secondsToTime(slider->value()),
                                                         true, true);
    timeLabel->setText(passed+'/'+total);

    if (slider->value() == slider->maximum()) {
        stop();
        emit finished();
    }

}


void Timeline::topicChanged(const RecordItNow::Timeline::Topic &topic)
{

    if (!topic.isValid()) {
        return;
    }

    KNotification *notification = new KNotification("newTopic", this, KNotification::CloseOnTimeout);
    notification->setText(i18n("New Topic: <i>%1</i>", topic.title()));
    notification->setPixmap(KIcon(topic.icon()).pixmap(KIconLoader::SizeMedium, KIconLoader::SizeMedium));
    notification->sendEvent();

}


void Timeline::resetSlider()
{

    slider->setValue(0);
    topicWidget->setCurrentSecond(-1);
    const QString total = KGlobal::locale()->formatTime(RecordItNow::Timeline::Topic::secondsToTime(slider->maximum()),
                                                        true, true);
    const QString passed = KGlobal::locale()->formatTime(QTime(0, 0, 0, 0), true, true);
    timeLabel->setText(passed+'/'+total);

}


void Timeline::durationChanged(const unsigned long newDuration)
{

    slider->setMaximum(newDuration);
    resetSlider();

}


#include "timeline.moc"
