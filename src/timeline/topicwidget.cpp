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
#include "topicwidget.h"
#include "topicprogressbar.h"
#include "topic.h"

// KDE
#include <kdebug.h>
#include <klocalizedstring.h>

// Qt
#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>
#include <QtGui/QLabel>



namespace RecordItNow {


namespace Timeline {



TopicWidget::TopicWidget(QWidget *parent)
    : QWidget(parent)
{

    m_layout = new QStackedLayout;
    setLayout(m_layout);

    clear();

}


TopicWidget::~TopicWidget()
{


}


void TopicWidget::setTime(const unsigned long &seconds)
{

    m_seconds = seconds;

}


RecordItNow::Timeline::Topic TopicWidget::addTopic(const QTime &duration, const QString title, const QString &icon)
{

    TopicProgressBar *bar = new TopicProgressBar(this);

    RecordItNow::Timeline::Topic newTopic;
    newTopic.setTitle(title);
    newTopic.setIcon(icon);
    newTopic.setDuration(duration);

    bar->setTopic(newTopic);

    m_layout->addWidget(bar);

    unsigned long total = 0;
    for (int i = 0; i < m_layout->count(); i++) {
        TopicProgressBar *topicBar = static_cast<TopicProgressBar*>(m_layout->widget(i));
        if (topicBar != m_noTopic) {
            total += topicBar->topic().durationToSeconds();
        }
    }
    emit durationChanged(total);

    return newTopic;

}


RecordItNow::Timeline::Topic TopicWidget::addTopic(const RecordItNow::Timeline::Topic &topic)
{

    TopicProgressBar *bar = new TopicProgressBar(this);

    bar->setTopic(topic);

    m_layout->addWidget(bar);

    unsigned long total = 0;
    for (int i = 0; i < m_layout->count(); i++) {
        TopicProgressBar *topicBar = static_cast<TopicProgressBar*>(m_layout->widget(i));
        if (topicBar != m_noTopic) {
            total += topicBar->topic().durationToSeconds();
        }
    }
    emit durationChanged(total);

    return topic;

}


QList<RecordItNow::Timeline::Topic> TopicWidget::topics() const
{

    QList<RecordItNow::Timeline::Topic> list;
    for (int i = 0; i < m_layout->count(); i++) {
        TopicProgressBar *topicBar = static_cast<TopicProgressBar*>(m_layout->widget(i));
        if (topicBar != m_noTopic) {
            list.append(topicBar->topic());
        }
    }
    return list;

}


void TopicWidget::setCurrentSecond(const unsigned long &second)
{

    if (second == (unsigned long)-1) {
        m_layout->setCurrentWidget(m_noTopic);
        emit topicChanged(Topic());
        return;
    }


    RecordItNow::Timeline::Topic oldTopic = static_cast<TopicProgressBar*>(m_layout->currentWidget())->topic();

    bool found = false;
    unsigned long duration = 0;
    for (int i = 1; i < m_layout->count(); i++) {
        TopicProgressBar *topicBar = static_cast<TopicProgressBar*>(m_layout->widget(i));
        duration += topicBar->topic().durationToSeconds();

        if (duration > second) {
            m_layout->setCurrentWidget(topicBar);
            found = true;
            break;
        }

    }

    if (!found) {
        m_layout->setCurrentWidget(m_noTopic);
    }

    TopicProgressBar *topicBar = static_cast<TopicProgressBar*>(m_layout->currentWidget());
    if (topicBar) {
        topicBar->topic().setCurrentSecond((duration-second));
        if (!oldTopic.isValid() || topicBar->topic() != oldTopic) {
            emit topicChanged(topicBar->topic());
        }
    }

}


void TopicWidget::clear()
{

    while (!m_layout->isEmpty()) {
        delete m_layout->takeAt(0);
    }
    m_noTopic = new TopicProgressBar(this);

    RecordItNow::Timeline::Topic topic;
    topic.setDuration(QTime(0, 0, 10, 0));
    topic.setTitle(i18n("No Topic"));
    topic.setIcon("dialog-information");

    m_noTopic->setTopic(topic);

    m_layout->addWidget(m_noTopic);

    emit durationChanged(0);

}


} // namespace Timeline


} // namespace RecordItNow


#include "topicwidget.moc"


