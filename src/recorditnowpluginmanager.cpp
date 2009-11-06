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
#include "libs/encoder/abstractencoder.h"
#include "libs/upload/abstractuploader.h"

// KDE
#include <kservice.h>
#include <kservicetypetrader.h>
#include <kdebug.h>
#include <kconfig.h>
#include <kconfiggroup.h>
#include <ksycoca.h>


RecordItNowPluginManager::RecordItNowPluginManager(QObject *parent)
    : QObject(parent)
{


}


RecordItNowPluginManager::~RecordItNowPluginManager()
{

    clear();

}


void RecordItNowPluginManager::init()
{

    loadPluginList();

}


RecordItNowPlugin *RecordItNowPluginManager::loadPlugin(const QString &name)
{

    QHashIterator<KPluginInfo, RecordItNowPlugin*> it(m_plugins);
    KPluginInfo info;
    while (it.hasNext()) {
        it.next();
        if (it.key().name().toLower() == name.toLower()) {
            if (it.value()) {
                return it.value();
            } else {
                info = it.key();
                break;
            }
        }
    }
    if (!info.isValid()) {
        kWarning() << "Plugin:" << name << "not found!";
        return 0;
    }

    kDebug() << "load plugin:" << name;

    KService::Ptr service = info.service();
    KPluginLoader loader(service->library());
    KPluginFactory *factory = loader.factory();

    if (!factory) {
        kWarning() << "KPluginFactory could not load the plugin:" << service->library() <<
        "Reason:" << loader.errorString();
        return 0;
    }

    RecordItNowPlugin *plugin = factory->create<RecordItNowPlugin>(this);
    if (!plugin) {
        kWarning() << "factory::create<>() failed " << service->library();
        return 0;
    }

    return (m_plugins[info] = plugin);

}


void RecordItNowPluginManager::unloadPlugin(RecordItNowPlugin *plugin)
{

    QHashIterator<KPluginInfo, RecordItNowPlugin*> it(m_plugins);
    while (it.hasNext()) {
        it.next();
        if (it.value() && it.value() == plugin) {
            delete it.value();
            kDebug() << "unload plugin:" << it.key().name();
            m_plugins[it.key()] = 0;
            return;
        }
    }
    kWarning() << "plugin not found!"; // should never happen

}


QList<KPluginInfo> RecordItNowPluginManager::getList(const QString &category) const
{


    QHashIterator<KPluginInfo, RecordItNowPlugin*> it(m_plugins);
    QList<KPluginInfo> infos;

    while (it.hasNext()) {
        it.next();
        if (it.key().category() == category) {
            infos.append(it.key());
        }
    }

    return infos;

}


QList<KPluginInfo> RecordItNowPluginManager::getRecorderList() const
{

    return getList("Recorder");

}


QList<KPluginInfo> RecordItNowPluginManager::getEncoderList() const
{

    return getList("Encoder");

}


QList<KPluginInfo> RecordItNowPluginManager::getUploaderList() const
{

    return getList("Uploader");

}


void RecordItNowPluginManager::clear()
{

    QHashIterator<KPluginInfo, RecordItNowPlugin*> it(m_plugins);
    while (it.hasNext()) {
        it.next();
        if (it.value()) {
            delete it.value();
        }
    }
    m_plugins.clear();

}


void RecordItNowPluginManager::loadPluginList()
{

    clear();

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
            m_plugins[info] = 0;
        }
    }
    kDebug() << ">>> RecordItNowRecorder finished!";

    if (m_plugins.isEmpty()) {
        printf("*********************************\n");
        printf("Please run \"kbuildsycoca4\".\n");
        printf("*********************************\n");
    }

    kDebug() << ">>> RecordItNowEncoder";

    KConfigGroup encoderCfg(&cfg, "Plugins");
    offers = KServiceTypeTrader::self()->query("RecordItNowEncoder");
    for (iter = offers.begin(); iter < offers.end(); ++iter) {
        KService::Ptr service = *iter;
        KPluginInfo info(service);

        if (!info.isValid()) {
            kWarning() << "invalid plugin info:" << service->entryPath();
            continue;
        } else {
            kDebug() << "found RecordItNowEncoder:" << info.name();
            info.setConfig(encoderCfg);
            info.load(encoderCfg);
            m_plugins[info] = 0;
        }
    }
    kDebug() << ">>> RecordItNowEncoder finished!";


    kDebug() << ">>> RecordItUploader";

    KConfigGroup uploaderCfg(&cfg, "Plugins");
    offers = KServiceTypeTrader::self()->query("RecordItNowUploader");
    for (iter = offers.begin(); iter < offers.end(); ++iter) {
        KService::Ptr service = *iter;
        KPluginInfo info(service);

        if (!info.isValid()) {
            kWarning() << "invalid plugin info:" << service->entryPath();
            continue;
        } else {
            kDebug() << "found RecordItNowUploader:" << info.name();
            info.setConfig(uploaderCfg);
            info.load(uploaderCfg);
            m_plugins[info] = 0;
        }
    }
    kDebug() << ">>> RecordItNowUploader finished!";

    emit pluginsChanged();

}


