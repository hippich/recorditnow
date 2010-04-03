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
#include "config/mouseconfig.h"
#include "config/keyboardconfig.h"
#include <recorditnow.h>
#include "keymon/deviceinfo.h"
#include "keymonmanager.h"

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

    QListWidgetItem *mouseItem = new QListWidgetItem;
    mouseItem->setText(i18n("Create mouse configuration..."));
    mouseItem->setIcon(KIcon("task-recurring"));

    QListWidgetItem *keyboardItem = new QListWidgetItem;
    keyboardItem->setText(i18n("Create keyboard configuration..."));
    keyboardItem->setIcon(KIcon("task-recurring"));

    listWidget->addItem(frameItem);
    listWidget->addItem(timelineItem);
    listWidget->addItem(mouseItem);
    listWidget->addItem(keyboardItem);

    QFuture<void> future = QtConcurrent::run(createConfig, frameItem, timelineItem, mouseItem,
                                             keyboardItem);

    QFutureWatcher<void> *watcher = new QFutureWatcher<void>();
    watcher->setFuture(future);

    connect(watcher, SIGNAL(finished()), watcher, SLOT(deleteLater()));
    connect(watcher, SIGNAL(finished()), this, SLOT(configCreated()));

}


void CreateConfigPage::createConfig(QListWidgetItem *frameItem, QListWidgetItem *timelineItem,
                                    QListWidgetItem *mouseItem, QListWidgetItem *keyboardItem)
{

    FrameConfig::writeSizes(FrameConfig::defaultSizes(), Settings::self()->config());
    frameItem->setIcon(KIcon("task-complete"));

    TimelineConfig::saveTopics(TimelineConfig::defaultTopics(), Settings::self()->config());
    timelineItem->setIcon(KIcon("task-complete"));


    QList<KeyMon::DeviceInfo> mouseInfos;
    QList<KeyMon::DeviceInfo> keyboardInfos;

    foreach (const KeyMon::DeviceInfo &info, KeyMonManager::self()->getInputDeviceList()) {
        if (info.type == KeyMon::DeviceInfo::KeyboardType) {
            keyboardInfos.append(info);
        } else  if (info.type == KeyMon::DeviceInfo::MouseType) {
            mouseInfos.append(info);
        }
    }

    if (!mouseInfos.isEmpty() && Settings::mouseDevice().isEmpty()) {
        Settings::self()->setMouseDevice(mouseInfos.first().uuid);
    }

    if (!keyboardInfos.isEmpty() && Settings::keyboardDevice().isEmpty()) {
        Settings::self()->setKeyboardDevice(keyboardInfos.first().uuid);
    }

    MouseConfig::saveConfig(Settings::self()->config(), MouseConfig::defaultButtons());
    mouseItem->setIcon(KIcon("task-complete"));

    KeyboardConfig::saveConfig(KeyboardConfig::defaultKeys(), Settings::self()->config());
    keyboardItem->setIcon(KIcon("task-complete"));

}


void CreateConfigPage::configCreated()
{

    m_complete = true;
    emit completeChanged();

}


#include "createconfigpage.moc"