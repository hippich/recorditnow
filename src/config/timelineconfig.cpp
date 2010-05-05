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
#include "timelineconfig.h"
#include "timeline/topic.h"
#include "topictreeitem.h"
#include "helper.h"
#include <recorditnow.h>
#include "listlayout.h"
#include "topicrow.h"

// KDE
#include <kicon.h>
#include <kicondialog.h>


using namespace RecordItNow::Timeline;
TimelineConfig::TimelineConfig(KConfig *cfg, QWidget *parent)
    : RecordItNow::ConfigPage(cfg, parent)
{

    setupUi(this);

    m_layout = new RecordItNow::ListLayout(0, true);
    topicListWidget->setLayout(m_layout);
    
    addTopicButton->setIcon(KIcon("list-add"));
    
    connect(addTopicButton, SIGNAL(clicked()), this, SLOT(addTopic()));
    connect(m_layout, SIGNAL(removeRequested(QWidget*)), this, SLOT(removeTopic(QWidget*)));
    connect(m_layout, SIGNAL(layoutChanged()), this, SLOT(updateTotalDuration()));

}



TimelineConfig::~TimelineConfig()
{


}


void TimelineConfig::saveConfig()
{

    saveTopics(currentTopics(), Settings::self()->config());

}


void TimelineConfig::setDefaults()
{

    setTopics(defaultTopics());

}


void TimelineConfig::loadConfig()
{

    setTopics(loadTopics(config()));

}


void TimelineConfig::saveTopics(const QList<RecordItNow::Timeline::Topic> &topics, KConfig *cfg)
{

    KConfigGroup group(cfg, "Timeline");
    group.writeEntry("Topics", RecordItNow::Helper::listToArray(topics));
    group.sync();

}


QList<RecordItNow::Timeline::Topic> TimelineConfig::loadTopics(KConfig *cfg)
{

    KConfigGroup group(cfg, "Timeline");
    const QByteArray data = group.readEntry("Topics", QByteArray());
    return RecordItNow::Helper::listFromArray<RecordItNow::Timeline::Topic>(data);

}


QList<RecordItNow::Timeline::Topic> TimelineConfig::defaultTopics()
{

    RecordItNow::Timeline::Topic linuxTopic;
    RecordItNow::Timeline::Topic recorditnowTopic;
    RecordItNow::Timeline::Topic kdeTopic;

    linuxTopic.setDuration(RecordItNow::Timeline::Topic::secondsToTime(60));
    linuxTopic.setTitle("Linux!");
    linuxTopic.setIcon("computer");

    recorditnowTopic.setDuration(RecordItNow::Timeline::Topic::secondsToTime(30));
    recorditnowTopic.setTitle("RecordItNow");
    recorditnowTopic.setIcon("recorditnow");

    kdeTopic.setDuration(RecordItNow::Timeline::Topic::secondsToTime(300));
    kdeTopic.setTitle("KDE SC");
    kdeTopic.setIcon("start-here-kde");

    QList<RecordItNow::Timeline::Topic> topics;
    topics.append(linuxTopic);
    topics.append(recorditnowTopic);
    topics.append(kdeTopic);

    return topics;

}


QList<RecordItNow::Timeline::Topic> TimelineConfig::currentTopics() const
{

    QList<RecordItNow::Timeline::Topic> topics;
    foreach (QWidget *widget, m_layout->rows()) {
        TopicRow *row = static_cast<TopicRow*>(widget);

        RecordItNow::Timeline::Topic topic;
        topic.setTitle(row->title());
        topic.setIcon(row->icon());
        topic.setDuration(row->duration());

        topics.append(topic);
    }
    return topics;

}


void TimelineConfig::setTopics(const QList<RecordItNow::Timeline::Topic> &topics)
{
    
    m_layout->clear();
    for (int i = 0; i < topics.size(); i++) {
        RecordItNow::Timeline::Topic topic = topics.at(i);
            
        TopicRow *row = new TopicRow(this);
        connect(row, SIGNAL(changed()), this, SLOT(updateTotalDuration()));
        row->setTitle(topic.title());
        row->setIcon(topic.icon());
        row->setDuration(topic.duration());

        m_layout->addRow(row);
    }
    updateTotalDuration();

}


void TimelineConfig::addTopic()
{

    TopicRow *row = new TopicRow(this);
    connect(row, SIGNAL(changed()), this, SLOT(updateTotalDuration()));
    m_layout->addRow(row);
    
    updateTotalDuration();

}


void TimelineConfig::removeTopic(QWidget *widget)
{
    
    m_layout->removeRow(widget);
    updateTotalDuration();

}


void TimelineConfig::updateTotalDuration()
{

    unsigned long duration = 0;
    foreach (const RecordItNow::Timeline::Topic &topic, currentTopics()) {
        duration += topic.durationToSeconds();
    }
    totalDurationLabel->setText(QString(" ")+KGlobal::locale()->formatDuration(duration*1000));

    setConfigChanged(loadTopics(config()) != currentTopics());

}


#include "timelineconfig.moc"
