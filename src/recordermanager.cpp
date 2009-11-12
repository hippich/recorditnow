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
#include "recordermanager.h"
#include "recorditnowpluginmanager.h"


RecorderManager::RecorderManager(QObject *parent, RecordItNowPluginManager *manager)
    : QObject(parent), m_manager(manager)
{



}


RecorderManager::~RecorderManager()
{

    clean();

}


QList<RecorderData> RecorderManager::getRecorder() const
{

    QList<RecorderData> list;
    foreach (const KPluginInfo &info, m_manager->getRecorderList()) {
        if (info.isPluginEnabled()) {
            RecorderData data;
            data.first = info.name();
            data.second = KIcon(info.icon());
            list.append(data);
        }
    }
    return list;

}


QHash<AbstractRecorder::Feature, bool> RecorderManager::getFeatures(const QString &recorder,
                                                                    QString &defaultFile)
{

    AbstractRecorder *plugin = static_cast<AbstractRecorder*>(m_manager->loadPlugin(recorder));
    QHash<AbstractRecorder::Feature, bool> features;

    if (plugin) {
        features[AbstractRecorder::Sound] = plugin->hasFeature(AbstractRecorder::Sound);
        features[AbstractRecorder::Fps] = plugin->hasFeature(AbstractRecorder::Fps);
        features[AbstractRecorder::Pause] = plugin->hasFeature(AbstractRecorder::Pause);
        features[AbstractRecorder::Stop] = plugin->hasFeature(AbstractRecorder::Stop);
        defaultFile = plugin->getDefaultOutputFile();
        m_manager->unloadPlugin(plugin);
    } else {
        features[AbstractRecorder::Sound] = false;
        features[AbstractRecorder::Fps] = false;
        features[AbstractRecorder::Pause] = false;
        features[AbstractRecorder::Stop] = false;
        defaultFile.clear();
    }
    return features;

}


void RecorderManager::startRecord(const QString &recorder, const AbstractRecorder::Data &data)
{

    if (m_recorder) {
        m_manager->unloadPlugin(m_recorder);
    }

    m_recorder = static_cast<AbstractRecorder*>(m_manager->loadPlugin(recorder));

    if (!m_recorder) {
        recorderError(i18n("Cannot load Recorder %1.", recorder));
        return;
    } else if (data.outputFile.isEmpty()) {
        recorderError(i18n("No output file specified."));
        return;
    }

    connect(m_recorder, SIGNAL(error(QString)), this, SLOT(recorderError(QString)));
    connect(m_recorder, SIGNAL(outputFileChanged(QString)), this, SIGNAL(fileChanged(QString)));
    connect(m_recorder, SIGNAL(status(QString)), this, SIGNAL(status(QString)));
    connect(m_recorder, SIGNAL(finished(AbstractRecorder::ExitStatus)), this,
            SLOT(recorderFinished(AbstractRecorder::ExitStatus)));

    m_recorder->record(data);

}


void RecorderManager::pauseOrContinue()
{

    if (m_recorder) {
        m_recorder->pause();
    }

}


void RecorderManager::stop()
{

    if (m_recorder) {
        m_recorder->stop();
    }

}


void RecorderManager::clean()
{

    if (m_recorder) {
        m_manager->unloadPlugin(m_recorder);
        m_recorder = 0;
    }

}


void RecorderManager::recorderError(const QString &error)
{

    emit finished(error);
    clean();

}


void RecorderManager::recorderFinished(const AbstractRecorder::ExitStatus &status)
{

    QString error;
    if (status == AbstractRecorder::Crash) {
        error = i18n("The recorder has crashed!");
    }
    emit finished(error, m_recorder->isVideoRecorder());
    clean();

}
