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

// KDE
#include <kicon.h>
#include <kicondialog.h>


using namespace RecordItNow::Timeline;
TimelineConfig::TimelineConfig(KConfig *cfg, QWidget *parent)
    : RecordItNow::ConfigPage(cfg, parent)
{

    setupUi(this);

    addButton->setIcon(KIcon("list-add"));
    removeButton->setIcon(KIcon("list-remove"));
    upButton->setIcon(KIcon("go-up"));
    downButton->setIcon(KIcon("go-down"));

    connect(addButton, SIGNAL(clicked()), this, SLOT(addTopic()));
    connect(removeButton, SIGNAL(clicked()), this, SLOT(removeTopic()));
    connect(upButton, SIGNAL(clicked()), this, SLOT(upClicked()));
    connect(downButton, SIGNAL(clicked()), this, SLOT(downClicked()));
    connect(treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(itemSelectionChanged()));
    connect(treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SIGNAL(configChanged()));

    treeWidget->header()->setResizeMode(QHeaderView::ResizeToContents);

}



TimelineConfig::~TimelineConfig()
{


}


void TimelineConfig::saveConfig()
{

    QList<RecordItNow::Timeline::Topic> topics;

    for (int i = 0; i < treeWidget->invisibleRootItem()->childCount(); i++) {
        QTreeWidgetItem *item = treeWidget->invisibleRootItem()->child(i);

        QString title = item->text(0);
        QString icon = static_cast<KIconButton*>(treeWidget->itemWidget(item, 1))->icon();
        QTime dTime = static_cast<QTimeEdit*>(treeWidget->itemWidget(item, 2))->time();

        RecordItNow::Timeline::Topic topic;
        topic.setDuration(dTime);
        topic.setIcon(icon);
        topic.setTitle(title);

        topics.append(topic);
    }

    saveTopics(topics, Settings::self()->config());

}


void TimelineConfig::setDefaults()
{


    saveTopics(defaultTopics(), Settings::self()->config());
    loadConfig();

}


void TimelineConfig::loadConfig()
{

    treeWidget->clear();
    QList<RecordItNow::Timeline::Topic> topics = loadTopics(Settings::self()->config());

    for (int i = 0; i < topics.size(); i++) {
        addTopic(); // create widgets
    }


    for (int i = 0; i < topics.size(); i++) {
        TopicTreeItem *item = static_cast<TopicTreeItem*>(treeWidget->invisibleRootItem()->child(i));
        item->setTopic(topics.at(i));
    }

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
    kdeTopic.setIcon("computer");

    QList<RecordItNow::Timeline::Topic> topics;
    topics.append(linuxTopic);
    topics.append(recorditnowTopic);
    topics.append(kdeTopic);

    return topics;

}


void TimelineConfig::addTopic()
{

    TopicTreeItem *item = new TopicTreeItem(treeWidget);
    connect(item, SIGNAL(durationChanged()), this, SLOT(updateTotalDuration()));

}


void TimelineConfig::removeTopic()
{

    QList<QTreeWidgetItem*> items = treeWidget->selectedItems();
    foreach (QTreeWidgetItem *child, items) {
        treeWidget->invisibleRootItem()->removeChild(child);
    }
    updateTotalDuration();

}


void TimelineConfig::upClicked()
{

    QList<QTreeWidgetItem*> items = treeWidget->selectedItems();
    if (items.isEmpty() || items.size() > 1) {
        return;
    }
    TopicTreeItem *item = static_cast<TopicTreeItem*>(items[0]);

    const int index = treeWidget->indexOfTopLevelItem(item)-1;

    if (index < 0) {
        return;
    }

    TopicTreeItem *copy = new TopicTreeItem(treeWidget, item, index);
    connect(copy, SIGNAL(durationChanged()), this, SLOT(updateTotalDuration()));

    treeWidget->invisibleRootItem()->removeChild(item);
    treeWidget->setCurrentItem(copy);

    emit configChanged();

}


void TimelineConfig::downClicked()
{

    QList<QTreeWidgetItem*> items = treeWidget->selectedItems();
    if (items.isEmpty() || items.size() > 1) {
        return;
    }
    TopicTreeItem *item = static_cast<TopicTreeItem*>(items[0]);

    const int index = treeWidget->indexOfTopLevelItem(item)+2;

    if (index > treeWidget->topLevelItemCount()) {
        return;
    }

    TopicTreeItem *copy = new TopicTreeItem(treeWidget, item, index);
    connect(copy, SIGNAL(durationChanged()), this, SLOT(updateTotalDuration()));

    treeWidget->invisibleRootItem()->removeChild(item);
    treeWidget->setCurrentItem(copy);

    emit configChanged();

}


void TimelineConfig::itemSelectionChanged()
{

    if(treeWidget->selectedItems().isEmpty() || treeWidget->selectedItems().size() > 1) {
        upButton->setEnabled(false);
        downButton->setEnabled(false);
        removeButton->setEnabled(false);
    } else {
        const int index = treeWidget->indexOfTopLevelItem(treeWidget->selectedItems().first());
        upButton->setEnabled(index != 0);
        downButton->setEnabled(index != treeWidget->topLevelItemCount()-1);
        removeButton->setEnabled(true);
    }

}


void TimelineConfig::updateTotalDuration()
{

    unsigned long duration = 0;
    for (int i = 0; i < treeWidget->invisibleRootItem()->childCount(); i++) {
        TopicTreeItem *item = static_cast<TopicTreeItem*>(treeWidget->invisibleRootItem()->child(i));
        duration += item->duration();
    }
    totalDurationEdit->setTime(RecordItNow::Timeline::Topic::secondsToTime(duration));

}



#include "timelineconfig.moc"
