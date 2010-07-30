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
#include "mouse/cursorwidget.h"

// KDE
#include <kdebug.h>

// Qt
#include <QtCore/QDir>


namespace RecordItNow {


RecorderManager::RecorderManager(QObject *parent, RecordItNowPluginManager *manager)
    : QObject(parent), m_manager(manager)
{



}


RecorderManager::~RecorderManager()
{

    clean();

}


RecordItNow::AbstractRecorder::State RecorderManager::currentState() const
{

    if (m_recorder) {
        return m_recorder->state();
    } else {
        return AbstractRecorder::Idle;
    }

}


QList<RecordItNow::RecorderData> RecorderManager::getRecorder() const
{

    QList<RecordItNow::RecorderData> list;
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


bool RecorderManager::hasFeature(const QString &feature, const QString &recorder) const
{

    foreach (const KPluginInfo &info, m_manager->getRecorderList()) {
        if (info.name().toLower() == recorder.toLower()) {
            return info.property("X-RecordItNow-"+feature).toBool();
        }
    }
    return false;

}


QString RecorderManager::getDefaultFile(const QString &name) const
{

    foreach (const KPluginInfo &info, m_manager->getRecorderList()) {
        if (info.name().toLower() == name.toLower()) {
            QString file = info.property("X-RecordItNow-DefaultFile").toString();
            if (file.contains("${home}")) {
                file.replace("${home}", QDir::homePath());
            }
            const QRegExp rx("\\$\\{config:.*:.*\\}");
            rx.indexIn(file);
            QString var = rx.cap();
            kDebug() << "var:" << var;
            if (!var.isEmpty()) {
                KConfig config("recorditnowrc");
                KConfigGroup cfg(&config, info.property("X-RecordItNow-ConfigGroup").toString());
                if (cfg.isValid()) {
                    QString key = var.remove("${config:").remove('}');
                    key.remove(QRegExp(":.*"));
                    QString defaultValue = var.mid(var.indexOf(":")+1);
                    kDebug() << "key:" << key << "default:" << defaultValue;
                    QString value = cfg.readEntry(key, defaultValue);
                    kDebug() << "value:" << value;
                    file.replace(rx, value);
                } else {
                    kWarning() << "invalid config!";
                }
            }
            return file;
        }
    }
    return QString();

}


void RecorderManager::startRecord(const QString &recorder,
                                  const RecordItNow::AbstractRecorder::Data &data,
                                  RecordItNow::CursorWidget *cursor)
{

    if (m_recorder) {
        m_manager->unloadPlugin(m_recorder);
    }

    m_recorder = static_cast<RecordItNow::AbstractRecorder*>(m_manager->loadPlugin(recorder));

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
    connect(m_recorder, SIGNAL(finished(RecordItNow::AbstractRecorder::ExitStatus)), this,
            SLOT(recorderFinished(RecordItNow::AbstractRecorder::ExitStatus)));
    connect(m_recorder, SIGNAL(stateChanged(RecordItNow::AbstractRecorder::State)), this,
            SIGNAL(stateChanged(RecordItNow::AbstractRecorder::State)));

    if (cursor && recorder == "Kasti") { // FIXME
        cursor->setRecorder(m_recorder);
    }
    
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


void RecorderManager::zoom(const bool &in)
{

    if (m_recorder) {
        int factor = m_recorder->zoomFactor();
        
        if (in) {
            factor++;
        } else {
            factor--;
        }
        
        m_recorder->setZoomFactor(factor);
    }
        
}


void RecorderManager::resetZoom()
{

    if (m_recorder) {
        m_recorder->setZoomFactor(1);
    }

}


void RecorderManager::clean()
{

    if (m_recorder) {
        m_recorder->disconnect();
        m_manager->unloadPlugin(m_recorder);
        m_recorder = 0;
    }

}


void RecorderManager::recorderError(const QString &error)
{

    clean();
    emit finished(error);

}


void RecorderManager::recorderFinished(const RecordItNow::AbstractRecorder::ExitStatus &status)
{

    QString error;
    if (status == RecordItNow::AbstractRecorder::Crash) {
        error = i18n("The recorder has crashed!");
    }
    emit finished(error, m_recorder->isVideoRecorder());
    clean();

}


} // namespace RecordItNow


#include "recordermanager.moc"

