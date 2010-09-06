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
#include "pluginmanager.h"
#include <config-recorditnow.h>
#include "helper.h"
#include "libs/recorder/abstractrecorder.h"
#include "libs/encoder/abstractencoder.h"
#ifdef HAVE_QTSCRIPT
    #include "script/scriptmanager.h"
#endif

// KDE
#include <kservice.h>
#include <kservicetypetrader.h>
#include <kdebug.h>
#include <kconfig.h>
#include <kconfiggroup.h>
#include <ksycoca.h>


namespace RecordItNow {


#define RECORDITNOW_PM_RAU_P "__RecordItNow_RemoveAfterUnload__"
PluginManager::PluginManager(QObject *parent)
    : QObject(parent)
{


}


PluginManager::~PluginManager()
{

    clear();

}


void PluginManager::init()
{

    loadPluginList();
    connect(KSycoca::self(), SIGNAL(databaseChanged(QStringList)), this, SLOT(sycocaChanged(QStringList)));

}


RecordItNow::Plugin *PluginManager::loadPlugin(const QString &name)
{

    QHashIterator<KPluginInfo, RecordItNow::Plugin*> it(m_plugins);
    KPluginInfo info;
    while (it.hasNext()) {
        it.next();
        if (it.key().name().toLower() == name.toLower()) {
            if (it.value()) { // already loaded?
                return it.value();
            } else { // get info and load
                info = it.key();
                break;
            }
        }
    }

#ifdef HAVE_QTSCRIPT
    foreach (const KPluginInfo &scriptInfo, Helper::self()->scriptManager()->availableScripts()) {
        if (scriptInfo.name().toLower() == name.toLower()) {
            info = scriptInfo;
            break;
        }
    }
#endif

    if (!info.isValid()) {
        return 0;
    }

    kDebug() << "load plugin:" << name;

#ifdef HAVE_QTSCRIPT
    if (info.category() == QLatin1String("Script")) {
        if (info.property("X-RecordItNow-ScriptType").toString() == QLatin1String("Recorder")) {
            return RecordItNow::Helper::self()->scriptManager()->loadRecorder(info);
        } else if (info.property("X-RecordItNow-ScriptType").toString() == QLatin1String("Encoder")) {
            return RecordItNow::Helper::self()->scriptManager()->loadEncoder(info);
        }
    }
#endif

    KService::Ptr service = info.service();
    KPluginLoader loader(service->library());
    KPluginFactory *factory = loader.factory();

    if (!factory) {
        kWarning() << "KPluginFactory could not load the plugin:" << service->library() <<
        "Reason:" << loader.errorString();
        return 0;
    }

    RecordItNow::Plugin *plugin = factory->create<RecordItNow::Plugin>(this, QVariantList() << qVariantFromValue(info));
    delete factory;
    if (!plugin) {
        kWarning() << "factory::create<>() failed " << service->library();
        return 0;
    }

    return (m_plugins[info] = plugin);

}


void PluginManager::unloadPlugin(RecordItNow::Plugin *plugin)
{

    if (!plugin) {
        return;
    }

    KPluginInfo info; // isValid == false
    QHashIterator<KPluginInfo, RecordItNow::Plugin*> it(m_plugins);
    while (it.hasNext()) {
        it.next();

        if (it.value() && it.value() == plugin) {
            info = it.key();
            break;
        }
    }

    if (info.isValid()) {
        if (plugin->property(RECORDITNOW_PM_RAU_P).toBool()) {
            const int count = m_plugins.remove(info);
            Q_ASSERT(count == 1);
        } else {
            m_plugins[info] = 0;
        }
    }
    delete plugin;

}


QList<KPluginInfo> PluginManager::getList(const QString &category) const
{

    QHashIterator<KPluginInfo, RecordItNow::Plugin*> it(m_plugins);
    QList<KPluginInfo> infoList;

    while (it.hasNext()) {
        it.next();
        if (it.key().category() == category) {
            // ignore deleted plugins
            if (!it.value() || (it.value() && !it.value()->property(RECORDITNOW_PM_RAU_P).toBool())) {
                infoList.append(it.key());
            }
        }
    }

    return infoList;

}


QList<KPluginInfo> PluginManager::getRecorderList() const
{

    QList<KPluginInfo> infos = getList("Recorder");
#ifdef HAVE_QTSCRIPT
    foreach (const KPluginInfo &info,  Helper::self()->scriptManager()->availableScripts()) {
        if (info.property("X-RecordItNow-ScriptType").toString() == QLatin1String("Recorder")) {
            infos.append(info);
        }
    }
#endif
    return infos;

}


QList<KPluginInfo> PluginManager::getEncoderList() const
{

    QList<KPluginInfo> infos = getList("Encoder");
#ifdef HAVE_QTSCRIPT
    foreach (const KPluginInfo &info,  Helper::self()->scriptManager()->availableScripts()) {
        if (info.property("X-RecordItNow-ScriptType").toString() == QLatin1String("Encoder")) {
            infos.append(info);
        }
    }
#endif
    return infos;

}


void PluginManager::clear()
{

    QHashIterator<KPluginInfo, RecordItNow::Plugin*> it(m_plugins);
    while (it.hasNext()) {
        it.next();
        if (it.value()) {
            it.value()->deleteLater();
        }
    }
    m_plugins.clear();

}


void PluginManager::loadPluginList()
{

    QHash<KPluginInfo, RecordItNow::Plugin*> oldPlugins = m_plugins;
    m_plugins.clear();

    loadInfos("RecordItNowRecorder");
    if (m_plugins.isEmpty()) {
        printf("*********************************\n");
        printf("Please run \"kbuildsycoca4\".\n");
        printf("*********************************\n");
    }
    loadInfos("RecordItNowEncoder");


    QMutableHashIterator<KPluginInfo, RecordItNow::Plugin*> oldIt(oldPlugins);
    while (oldIt.hasNext()) {
        oldIt.next();
        if (oldIt.value()) {
            bool remove = true;
            QHashIterator<KPluginInfo, RecordItNow::Plugin*> it(m_plugins);
            while (it.hasNext()) {
                it.next();
                if (it.key().name() == oldIt.key().name()) {
                    m_plugins.remove(it.key());
                    remove = false;
                    break;
                }
            }
            oldIt.value()->setProperty(RECORDITNOW_PM_RAU_P, remove);
            m_plugins.insert(oldIt.key(), oldIt.value());
            oldIt.remove();
        }
    }

    emit pluginsChanged();

}


void PluginManager::loadInfos(const QString &type)
{

    KConfig cfg("recorditnowrc");
    KConfigGroup pCfg(&cfg, "Plugins");

    KService::List offers = KServiceTypeTrader::self()->query(type);
    KService::List::const_iterator iter;

    for (iter = offers.begin(); iter < offers.end(); ++iter) {
        KService::Ptr service = *iter;
        KPluginInfo info(service);

        if (!info.isValid()) {
            kWarning() << "invalid plugin info:" << service->entryPath();
            continue;
        } else {
            info.setConfig(pCfg);
            info.load(pCfg);
            m_plugins[info] = 0;
        }
    }

}


void PluginManager::sycocaChanged(const QStringList &changed)
{

    if (changed.contains(QLatin1String("services"))) {
        loadPluginList();
    }

}


} // namespace RecordItNow


#include "pluginmanager.moc"
