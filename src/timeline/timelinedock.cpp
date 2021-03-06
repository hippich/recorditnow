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
#include "timelinedock.h"
#include "timeline.h"
#include <recorditnow.h>
#include "topicprogressbar.h"

// KDE
#include <klocalizedstring.h>
#include <kconfiggroup.h>
#include <kicon.h>


namespace RecordItNow {


TimelineDock::TimelineDock(QWidget *parent)
    : RecordItNow::DockWidget(parent)
{

    setObjectName("Timeline");
    setDockTitle(i18n("Timeline"));
    setWindowIcon(KIcon("recorditnow-timeline"));
    setAllowedAreas(Qt::AllDockWidgetAreas);
    setFeatures(QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetClosable);
    Timeline *line = new Timeline(this);
    setWidget(line);

    QWidget *title = new QWidget(this);
    ui_titleWidget.setupUi(title);

    resetTitle();

    setTitleBarWidget(title);

    connect(line, SIGNAL(currentTopicChanged(RecordItNow::Topic)), this,
            SLOT(topicChanged(RecordItNow::Topic)));

}


TimelineDock::~TimelineDock()
{



}


Timeline *TimelineDock::timeline() const
{

    return static_cast<Timeline*>(widget());

}


void TimelineDock::resetTitle()
{

    ui_titleWidget.currentTopicLabel->setText(i18n("No Topic"));
    ui_titleWidget.iconButton->setIcon(KIcon("dialog-information"));

}


void TimelineDock::topicChanged(const RecordItNow::Topic &topic)
{

    if (!topic.isValid()) {
        resetTitle();
        return;
    }
    ui_titleWidget.currentTopicLabel->setText(topic.title());
    ui_titleWidget.iconButton->setIcon(KIcon(topic.icon()));

}


} // namespace RecordItNow


#include "timelinedock.moc"
