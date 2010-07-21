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
#include "createconfigpage.h"
#include "config/frameconfig.h"
#include "config/timelineconfig.h"
#include <recorditnow.h>

// Qt
#include <QtGui/QListWidgetItem>
#include <QtCore/QtConcurrentRun>
#include <QtCore/QFutureWatcher>


CreateConfigPage::CreateConfigPage(QWidget *parent)
    : QWizardPage(parent), m_complete(false)
{

    setupUi(this);

}


bool CreateConfigPage::isComplete() const
{

    return m_complete;

}


void CreateConfigPage::initializePage()
{

    listWidget->clear();

    QListWidgetItem *frameItem = new QListWidgetItem;
    frameItem->setText(i18n("Create frame configuration..."));
    frameItem->setIcon(KIcon("task-recurring"));

    QListWidgetItem *timelineItem = new QListWidgetItem;
    timelineItem->setText(i18n("Create timeline configuration..."));
    timelineItem->setIcon(KIcon("task-recurring"));

    listWidget->addItem(frameItem);
    listWidget->addItem(timelineItem);

    QFuture<void> future = QtConcurrent::run(createConfig, frameItem, timelineItem);

    QFutureWatcher<void> *watcher = new QFutureWatcher<void>();
    watcher->setFuture(future);

    connect(watcher, SIGNAL(finished()), watcher, SLOT(deleteLater()));
    connect(watcher, SIGNAL(finished()), this, SLOT(configCreated()));

}


void CreateConfigPage::createConfig(QListWidgetItem *frameItem, QListWidgetItem *timelineItem)
{

    FrameConfig::writeSizes(FrameConfig::defaultSizes(), Settings::self()->config());
    frameItem->setIcon(KIcon("task-complete"));

    TimelineConfig::saveTopics(TimelineConfig::defaultTopics(), Settings::self()->config());
    timelineItem->setIcon(KIcon("task-complete"));

}


void CreateConfigPage::configCreated()
{

    m_complete = true;
    emit completeChanged();

}


#include "createconfigpage.moc"
