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


namespace RecordItNow {
    class ListLayout;
};


class TimelineConfig : public RecordItNow::ConfigPage, Ui::TimelineConfig
{
    Q_OBJECT


public:
    explicit TimelineConfig(KConfig *cfg, QWidget *parent = 0);
    ~TimelineConfig();

    void saveConfig();
    void setDefaults();
    void loadConfig();

    static void saveTopics(const QList<RecordItNow::Topic> &topics, KConfig *cfg);
    static QList<RecordItNow::Topic> loadTopics(KConfig *cfg);
    static QList<RecordItNow::Topic> defaultTopics();


private:
    RecordItNow::ListLayout *m_layout;
    
    QList<RecordItNow::Topic> currentTopics() const;

    void setTopics(const QList<RecordItNow::Topic> &topics);


private slots:
    void addTopic();
    void removeTopic(QWidget *widget);
    void updateTotalDuration();


};


#endif // TIMELINECONFIG_H
