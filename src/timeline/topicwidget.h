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


#ifndef RECORDITNOW_TOPICWIDGET_H
#define RECORDITNOW_TOPICWIDGET_H

// own
#include "topic.h"

// KDE
#include <kicon.h>

// Qt
#include <QtGui/QProgressBar>
#include <QtCore/QHash>
#include <QtGui/QStackedLayout>


namespace RecordItNow {


class TopicProgressBar;
class TopicWidget : public QWidget
{
    Q_OBJECT


public:
    TopicWidget(QWidget *parent = 0);
    ~TopicWidget();

    RecordItNow::Topic addTopic(const QTime &duration, const QString title, const QString &icon);
    RecordItNow::Topic addTopic(const RecordItNow::Topic &topic);

    QList<RecordItNow::Topic> topics() const;

    void setTime(const unsigned long &seconds);
    void setCurrentSecond(const unsigned long &second);
    void clear();


private:
    unsigned long m_seconds;
    QStackedLayout *m_layout;
    TopicProgressBar *m_noTopic;


signals:
    void topicChanged(const RecordItNow::Topic &newTopic);
    void durationChanged(const unsigned long &newDuration);


};


} // namespace RecordItNow


#endif // RECORDITNOW_TOPICWIDGET_H
