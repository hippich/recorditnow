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
#include "timeline.h"
#include "timelinetopicsdialog.h"
#include "topic.h"

// KDE
#include <kdebug.h>
#include <knotification.h>

// Qt
#include <QtCore/QTimer>


TimeLine::TimeLine(QWidget *parent)
    : QWidget(parent)
{

    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(updateTime()));

    setupUi(this);

    editButton->setIcon(KIcon("document-edit"));
    connect(editButton, SIGNAL(clicked()), this, SLOT(configure()));

    connect(topicWidget, SIGNAL(topicChanged(Topic*)), this, SLOT(topicChanged(Topic*)));
    connect(topicWidget, SIGNAL(durationChanged(ulong)), this, SLOT(durationChanged(ulong)));

    m_notifications = false;
    slider->setMaximum(0);
    resetSlider();
    setState(Idle);

}


TimeLine::~TimeLine()
{

    delete m_timer;

}


unsigned long TimeLine::duration() const
{

    return slider->maximum();

}


void TimeLine::loadTopics(KConfigGroup *cfg)
{

    topicWidget->clear();
    const int count = cfg->readEntry("Topics", 0);
    for (int i = 0; i < count; i++) {
        const QString title = cfg->readEntry(QString("Topic %1 Title").arg(i), i18n("Untitled"));
        const QString icon = cfg->readEntry(QString("Topic %1 Icon").arg(i), "dialog-information");
        const unsigned long duration = cfg->readEntry(QString("Topic %1 Duration").arg(i), 10);

        topicWidget->addTopic(Topic::secondsToTime(duration), title, icon);
    }

    if (count == 0) {
        topicWidget->addTopic(Topic::secondsToTime(60), "Linux!", "computer");
        topicWidget->addTopic(Topic::secondsToTime(30), "RecordItNow", "recorditnow");
        topicWidget->addTopic(Topic::secondsToTime(300), "KDE SC", "kde-start-here");
    }
    resetSlider();

}


void TimeLine::saveTopics(KConfigGroup *cfg)
{

    int count = 0;
    foreach (Topic *topic, topicWidget->topics()) {
        cfg->writeEntry(QString("Topic %1 Title").arg(count), topic->title());
        cfg->writeEntry(QString("Topic %1 Icon").arg(count), topic->icon());
        cfg->writeEntry(QString("Topic %1 Duration").arg(count), (int)topic->durationToSeconds());
        count++;
    }
    cfg->writeEntry("Topics", count);

}


void TimeLine::enableNotifications(const bool &enable)
{

    m_notifications = enable;

}


void TimeLine::start()
{

    if (state() != Idle) {
        return;
    }

    setState(Running);
    slider->setValue(0);
    m_timer->start(1000);

}


void TimeLine::stop()
{

    if (m_state == Idle) {
        return;
    }
    setState(Idle);
    m_timer->stop();

    QTimer::singleShot(100, this, SLOT(resetSlider()));

}


void TimeLine::pauseOrContinue()
{

    if (state() == Paused) {
        m_timer->start(1000);
        setState(Running);
    } else if (state() == Running) {
        setState(Paused);
        m_timer->stop();
    }

}


TimeLine::State TimeLine::state() const
{

    return m_state;

}


void TimeLine::setState(const State &state)
{

    m_state = state;
    switch (m_state) {
    case Idle: editButton->setEnabled(true); break;
    case Running: editButton->setEnabled(false); break;
    default: break;
    }

}


void TimeLine::updateTime()
{

    slider->setValue(slider->value()+1);
    topicWidget->setCurrentSecond(slider->value());

    const QString total = KGlobal::locale()->formatTime(Topic::secondsToTime(slider->maximum()),
                                                        true, true);
    const QString passed = KGlobal::locale()->formatTime(Topic::secondsToTime(slider->value()),
                                                         true, true);
    timeLabel->setText(passed+"/"+total);

    if (slider->value() == slider->maximum()) {
        stop();
        emit finished();
    }

}


void TimeLine::configure()
{

    TimeLineTopicsDialog *dialog = new TimeLineTopicsDialog(this, topicWidget);
    dialog->show();

}


void TimeLine::topicChanged(Topic *topic)
{

    if (!m_notifications) {
        return;
    }

    KNotification::event(KNotification::Notification,
                         i18n("New Topic: <i>%1</i>", topic->title()),
                         KIcon(topic->icon()).pixmap(KIconLoader::SizeMedium, KIconLoader::SizeMedium),
                         this);

}


void TimeLine::resetSlider()
{

    slider->setValue(0);
    topicWidget->setCurrentSecond(-1);
    const QString total = KGlobal::locale()->formatTime(Topic::secondsToTime(slider->maximum()),
                                                        true, true);
    const QString passed = KGlobal::locale()->formatTime(QTime(0, 0, 0, 0), true, true);
    timeLabel->setText(passed+"/"+total);

}


void TimeLine::durationChanged(const unsigned long newDuration)
{

    slider->setMaximum(newDuration);
    resetSlider();

}


