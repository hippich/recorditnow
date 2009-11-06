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
    kDebug() << "plugin not found:" << name;
    return 0;

}


AbstractUploader *RecordItNowPluginManager::loadUploaderPlugin(const KPluginInfo &info)
{

    if (!m_uploaderPlugins.contains(info)) {
        kWarning() << "no such plugin:" << info.name();
        return 0;
    } else if (m_uploaderPlugins[info]) {
        return m_uploaderPlugins[info];
    }

    kDebug() << "load uploader:" << info.name();

    KService::Ptr service = info.service();
    KPluginLoader loader(service->library());
    KPluginFactory *factory = loader.factory();

    if (!factory) {
        kWarning() << "KPluginFactory could not load the plugin:" << service->library() <<
        "Reason:" << loader.errorString();
        return 0;
    }

    AbstractUploader *uploader = factory->create<AbstractUploader>(this);
    if (!uploader) {
        kWarning() << "factory::create<>() failed " << service->library();
        return 0;
    }

    return (m_uploaderPlugins[info] = uploader);

}


AbstractEncoder *RecordItNowPluginManager::loadEncoderPlugin(const KPluginInfo &info)
{

    if (!m_encoderPlugins.contains(info)) {
        kWarning() << "no such plugin:" << info.name();
        return 0;
    } else if (m_encoderPlugins[info]) {
        return m_encoderPlugins[info];
    }

    kDebug() << "load encoder:" << info.name();

    KService::Ptr service = info.service();
    KPluginLoader loader(service->library());
    KPluginFactory *factory = loader.factory();

    if (!factory) {
        kWarning() << "KPluginFactory could not load the plugin:" << service->library() <<
        "Reason:" << loader.errorString();
        return 0;
    }

    AbstractEncoder *encoder = factory->create<AbstractEncoder>(this);
    if (!encoder) {
        kWarning() << "factory::create<>() failed " << service->library();
        return 0;
    }

    return (m_encoderPlugins[info] = encoder);

}


AbstractEncoder *RecordItNowPluginManager::loadEncoderPlugin(const QString &name)
{

    QHashIterator<KPluginInfo, AbstractEncoder*> it(m_encoderPlugins);
    while (it.hasNext()) {
        it.next();
        if (it.key().name().toLower() == name.toLower()) {
            return loadEncoderPlugin(it.key());
        }
    }
    kDebug() << "plugin not found:" << name;
    return 0;

}


AbstractUploader *RecordItNowPluginManager::loadUploaderPlugin(const QString &name)
{

    QHashIterator<KPluginInfo, AbstractUploader*> it(m_uploaderPlugins);
    while (it.hasNext()) {
        it.next();
        if (it.key().name().toLower() == name.toLower()) {
            return loadUploaderPlugin(it.key());
        }
    }
    kDebug() << "plugin not found:" << name;
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


void RecordItNowPluginManager::unloadUploaderPlugin(const KPluginInfo &info)
{

    if (!m_uploaderPlugins.contains(info)) {
        kWarning() << "no such plugin";
        return;
    } else if (!m_uploaderPlugins[info]) {
        kDebug() << "plugin not loaded:" << info.name();
        return;
    }

    kDebug() << "unload uploader:" << info.name();

    m_uploaderPlugins[info]->deleteLater();
    m_uploaderPlugins[info] = 0;

}


void RecordItNowPluginManager::unloadUploaderPlugin(AbstractUploader *uploader)
{

    unloadUploaderPlugin(m_uploaderPlugins.key(uploader));

}


void RecordItNowPluginManager::unloadEncoderPlugin(const KPluginInfo &info)
{

    if (!m_encoderPlugins.contains(info)) {
        kWarning() << "no such plugin";
        return;
    } else if (!m_encoderPlugins[info]) {
        kDebug() << "plugin not loaded:" << info.name();
        return;
    }

    kDebug() << "unload encoder:" << info.name();

    m_encoderPlugins[info]->deleteLater();
    m_encoderPlugins[info] = 0;

}


void RecordItNowPluginManager::unloadEncoderPlugin(AbstractEncoder *encoder)
{

    unloadEncoderPlugin(m_encoderPlugins.key(encoder));

}


QList<KPluginInfo> RecordItNowPluginManager::getRecorderList() const
{

    return m_recorderPlugins.keys();

}


QList<KPluginInfo> RecordItNowPluginManager::getEncoderList() const
{

    return m_encoderPlugins.keys();

}


QList<KPluginInfo> RecordItNowPluginManager::getUploaderList() const
{

    return m_uploaderPlugins.keys();

}


void RecordItNowPluginManager::clear()
{

    // recorder plugins
    QHashIterator<KPluginInfo, AbstractRecorder*> it(m_recorderPlugins);
    while (it.hasNext()) {
        it.next();
        if (it.value()) {
            delete it.value();
        }
    }
    m_recorderPlugins.clear();

    // encoder plugins
    QHashIterator<KPluginInfo, AbstractEncoder*> eit(m_encoderPlugins);
    while (eit.hasNext()) {
        eit.next();
        if (eit.value()) {
            delete eit.value();
        }
    }
    m_encoderPlugins.clear();

    // upload plugins
    QHashIterator<KPluginInfo, AbstractUploader*> uit(m_uploaderPlugins);
    while (uit.hasNext()) {
        uit.next();
        if (uit.value()) {
            delete uit.value();
        }
    }
    m_uploaderPlugins.clear();

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
            m_recorderPlugins[info] = 0;
        }
    }
    kDebug() << ">>> RecordItNowRecorder finished!";
    kDebug() << ">>> found" << m_recorderPlugins.size() << "recorder!";

    if (m_recorderPlugins.isEmpty()) {
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
            m_encoderPlugins[info] = 0;
        }
    }
    kDebug() << ">>> RecordItNowEncoder finished!";
    kDebug() << ">>> found" << m_encoderPlugins.size() << "encoder!";


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
            m_uploaderPlugins[info] = 0;
        }
    }
    kDebug() << ">>> RecordItNowUploader finished!";
    kDebug() << ">>> found" << m_uploaderPlugins.size() << "uploader!";

    emit pluginsChanged();

}


