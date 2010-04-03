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

#ifndef TIMELINECONFIG_H
#define TIMELINECONFIG_H


// own
#include "configpage.h"
#include "ui_timelineconfig.h"
#include "timeline/topic.h"


class TimelineConfig : public RecordItNow::ConfigPage, Ui::TimelineConfig
{
    Q_OBJECT


public:
    explicit TimelineConfig(KConfig *cfg, QWidget *parent = 0);
    ~TimelineConfig();

    void saveConfig();
    void setDefaults();
    void loadConfig();

    static void saveTopics(const QList<RecordItNow::Timeline::Topic> &topics, KConfig *cfg);
    static QList<RecordItNow::Timeline::Topic> loadTopics(KConfig *cfg);
    static QList<RecordItNow::Timeline::Topic> defaultTopics();


private:
    QList<RecordItNow::Timeline::Topic> currentTopics() const;

    void setTopics(const QList<RecordItNow::Timeline::Topic> &topics);


private slots:
    void addTopic();
    void removeTopic();
    void upClicked();
    void downClicked();
    void itemSelectionChanged();
    void updateTotalDuration();
    void itemChanged(QTreeWidgetItem *item, int column);


};


#endif // TIMELINECONFIG_H
