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
#include "recorditnowpluginmanager.h"
#include "libs/recorder/abstractrecorder.h"

// KDE
#include <kservice.h>
#include <kservicetypetrader.h>
#include <kdebug.h>
#include <kconfig.h>
#include <kconfiggroup.h>



RecordItNowPluginManager::RecordItNowPluginManager(QObject *parent)
    : QObject(parent)
{

    loadPluginList();

}


RecordItNowPluginManager::~RecordItNowPluginManager()
{

    QHashIterator<KPluginInfo, AbstractRecorder*> it(m_recorderPlugins);
    while (it.hasNext()) {
        it.next();
        delete it.value();
    }

}


AbstractRecorder *RecordItNowPluginManager::loadRecorderPlugin(const KPluginInfo &info)
{

    if (!m_recorderPlugins.contains(info)) {
        kWarning() << "no such plugin:" << info.name();
        return 0;
    } else if (m_recorderPlugins[info]) {
        return m_recorderPlugins[info];
    }

    kDebug() << "load recorder:" << info.name();

    KService::Ptr service = info.service();
    KPluginLoader loader(service->library());
    KPluginFactory *factory = loader.factory();

    if (!factory) {
        kWarning() << "KPluginFactory could not load the plugin:" << service->library() <<
        "Reason:" << loader.errorString();
        return 0;
    }

    AbstractRecorder *recorder = factory->create<AbstractRecorder>(this);
    if (!recorder) {
        kWarning() << "factory::create<>() failed " << service->library();
        return 0;
    }

    return (m_recorderPlugins[info] = recorder);

}


AbstractRecorder *RecordItNowPluginManager::loadRecorderPlugin(const QString &name)
{

    QHashIterator<KPluginInfo, AbstractRecorder*> it(m_recorderPlugins);
    while (it.hasNext()) {
        it.next();
        if (it.key().name().toLower() == name.toLower()) {
            return loadRecorderPlugin(it.key());
        }
    }
    return 0;

}


void RecordItNowPluginManager::unloadRecorderPlugin(const KPluginInfo &info)
{

    if (!m_recorderPlugins.contains(info)) {
        kWarning() << "no such plugin";
        return;
    } else if (!m_recorderPlugins[info]) {
        kDebug() << "plugin not loaded:" << info.name();
        return;
    }

    kDebug() << "unload recorder:" << info.name();

    m_recorderPlugins[info]->deleteLater();
    m_recorderPlugins[info] = 0;

}


void RecordItNowPluginManager::unloadRecorderPlugin(AbstractRecorder *recorder)
{

    unloadRecorderPlugin(m_recorderPlugins.key(recorder));

}


QList<KPluginInfo> RecordItNowPluginManager::getRecorderList() const
{

    return m_recorderPlugins.keys();

}


void RecordItNowPluginManager::loadPluginList()
{

    kDebug() << "load plugin list..";

    KConfig cfg("recorditnowrc");


    kDebug() << ">>> RecordItNowRecorder";

    KConfigGroup recorderCfg(&cfg, "Plugins");
    KService::List offers = KServiceTypeTrader::self()->query("RecordItNowRecorder");
    KService::List::const_iterator iter;
    for (iter = offers.begin(); iter < offers.end(); ++iter) {
        KService::Ptr service = *iter;
        KPluginInfo info(service);

        if (!info.isValid()) {
            kWarning() << "invalid plugin info:" << service->entryPath();
            continue;
        } else {
            kDebug() << "found RecordItNowRecorder:" << info.name();
            info.setConfig(recorderCfg);
            info.load(recorderCfg);
            m_recorderPlugins[info] = 0;
        }
    }
    kDebug() << ">>> RecordItNowRecorder finished!";

}
