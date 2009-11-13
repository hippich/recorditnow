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
#include "recordmydesktoprecorder.h"
#include <recorditnow_recordmydesktop.h>

// KDE
#include <kplugininfo.h>
#include <kprocess.h>
#include <kglobal.h>
#include <kstandarddirs.h>
#include <kdebug.h>

// Qt
#include <QtGui/QWidget>
#include <QtCore/QDir>

// C
#include <sys/types.h>
#include <signal.h>



K_PLUGIN_FACTORY(myFactory, registerPlugin<RecordMyDesktopRecorder>();)
K_EXPORT_PLUGIN(myFactory("recordmydesktop_recorder"))
RecordMyDesktopRecorder::RecordMyDesktopRecorder(QObject *parent, const QVariantList &args)
    : AbstractRecorder(parent, args)
{

    m_recorder = 0;
    m_data.paused = false;

}


RecordMyDesktopRecorder::~RecordMyDesktopRecorder()
{

    clean();

}


bool RecordMyDesktopRecorder::hasFeature(const AbstractRecorder::Feature &feature) const
{

    switch (feature) {
    case AbstractRecorder::Sound:
    case AbstractRecorder::Fps:
    case AbstractRecorder::Pause:
    case AbstractRecorder::Stop: return true;
    default: return false;
    }

}


QString RecordMyDesktopRecorder::getDefaultOutputFile() const
{

    return QDir::homePath()+"/desktop_video.ogv";

}


void RecordMyDesktopRecorder::record(const AbstractRecorder::Data &d)
{

    const QString recordMyDesktop = KGlobal::dirs()->findExe("recordmydesktop");
    if (recordMyDesktop.isEmpty()) {
        emit error(i18n("Cannot find recordmydesktop!\n"
                        "Please install recordmydesktop or use another plugin."));
        return;
    }

    QStringList args;

    // default cfg
    if (!d.sound) {
        args << "--no-sound";
    }
    m_data.overwrite = d.overwrite;
    m_data.outputFile = d.outputFile;
    m_data.tmpFile = getTemporaryFile(d.workDir)+".ogv";
    args << "-o" << m_data.tmpFile;

    if (d.winId != -1) {
        args << "--windowid" << QString::number(d.winId);
    } else {
        if (d.geometry.x() > 0) {
            args << QString("-x") << QString::number(d.geometry.x());
        }
        if (d.geometry.y() > 0) {
            args << QString("-y") << QString::number(d.geometry.y());
        }
        args << QString("-width") << QString::number(d.geometry.width());
        args << QString("-height") << QString::number(d.geometry.height());
    }
    args << "--fps" << QString::number(d.fps);
    args << "--workdir" << d.workDir;

    // recordmydesktop cfg
    Settings::self()->readConfig(); // cfg changed?

    // image
    if (Settings::__no_cursor()) {
        args << "--no-cursor";
    }
    if (Settings::__no_shared()) {
        args << "--no-shared";
    }
    if (Settings::__full_shots()) {
        args << "--full-shots";
    }
    if (Settings::__subsampling()) {
        args << "--quick-subsampling";
    }

    // sound
    if (Settings::__channels() != -1) {
        args << "--channels" << QString::number(Settings::__channels());
    }
    if (Settings::__freq() != -1) {
        args << "--freq" << QString::number(Settings::__freq());
    }
    if (Settings::__buffer_size() != -1) {
        args << "--buffer-size" << QString::number(Settings::__buffer_size());
    }
    if (Settings::__ring_buffer_size() != -1) {
        args << "--ring-buffer-size" << QString::number(Settings::__ring_buffer_size());
    }
    if (Settings::__device() != "hw:0,0" && !Settings::__device().isEmpty()) {
        args << "--device" << Settings::__device();
    }
    if (!Settings::__use_jack().isEmpty()) {
        args << "--use-jack" << Settings::__use_jack();
    }

    // encoding
    if (Settings::__on_the_fly_encoding()) {
        args << "--on-the-fly-encoding";
    }

    args << "--v_quality" << QString::number(Settings::__v_quality());
    args << "--v_bitrate" << QString::number(Settings::__v_bitrate());
    args << "--s_quality" << QString::number(Settings::__s_quality());

    // misc
    if (Settings::__no_wm_check()) {
        args << "--no-wm-check";
    }
    if (Settings::__compress_cache()) {
        args << "--compress-cache";
    }
    if (Settings::followMouse()) {
        args << "--follow-mouse";
    }
    if (!Settings::frame()) {
        args << "--no-frame";
    }

    // create/start
    m_recorder = new KProcess(this);
    m_recorder->setOutputChannelMode(KProcess::MergedChannels);

    connect(m_recorder, SIGNAL(readyReadStandardOutput()), this, SLOT(newRecorderOutput()));
    connect(m_recorder, SIGNAL(finished(int)), this, SLOT(recorderFinished(int)));

    kDebug() << "starting:" << recordMyDesktop << "with:" << args;

    m_recorder->setProgram(recordMyDesktop, args);
    m_recorder->start();

}


void RecordMyDesktopRecorder::pause()
{

    if (!m_recorder) {
        return;
    }

    if (!m_data.paused) {
        emit status(i18n("Paused!"));
        kill(m_recorder->pid(), SIGSTOP);
        m_data.paused = true;
    } else {
        emit status(i18n("Capturing!"));
        kill(m_recorder->pid(), SIGCONT);
        m_data.paused = false;
    }

}


void RecordMyDesktopRecorder::stop()
{

    if (m_recorder) {
        kill(m_recorder->pid(), SIGINT);
        m_data.paused = false;
    }

}


void RecordMyDesktopRecorder::newRecorderOutput()
{

    if (!m_recorder) {
        return;
    }

    QString output = QString(m_recorder->readAllStandardOutput()).trimmed();

    if (output.isEmpty()) {
        return;
    }

    foreach (QString line, output.split('\n')) {
        kDebug() << "New Line:" << line;
        line = line.trimmed();


        if (line.startsWith("Cached")) {
            QString cached = line.remove(0, 7);
            QString rec = line.remove(0, line.indexOf("from ")+5);

            cached.remove(cached.indexOf(','), cached.length());
            rec.remove(rec.indexOf("that"), rec.length());

            emit status(i18n("Cached: %1", cached));
            emit status(i18n("Received: %1", rec));
        } else if (line.startsWith("Saved ")) {
            QString frames = line.remove("Saved ");
            frames.remove(frames.indexOf("frames"), frames.length());
            emit status(i18n("Frames: %1", frames));
        } else if (line.startsWith("Output file:")) {
            m_data.tmpFile = line.remove(0, 13);
        } else if (line.startsWith("[")) {
            for (int i = 0; i < line.length(); i++) {
                if (line[i] == '[') {
                    line.remove(0, i+1);
                    const int percent = line.mid(0, line.indexOf('%', 0)).toInt();
                    line.remove(0, line.indexOf("]"));
                    i = 0;
                    emit status(i18n("Encoding: %1",
                                     QString::number(percent > 100 ? 100 : percent)+'%'));
                }
            }
        }
        if (line == "Capturing!") {
            emit status(i18n("Capturing!"));
        } else if (line == "Shutting down..") {
            emit status(i18n("Shutting down.."));
        } else if (line == "Encoding started!") {
            emit status(i18n("Encoding started!"));
        } else if (line == "Done!!!") {
            emit status(i18n("Done!"));
        } else if (line.startsWith("Cannot open file")) {
            line = line.remove("Cannot open file ");
            line = line.remove(" for writting!");
            emit error(i18n("Cannot open file %1 for writting!", line));
        } else if (line.startsWith("Could not create temporary directory")) {
            emit error(i18n("Could not create temporary directory, check your config."));
        } else if (line.startsWith("recordMyDesktop is not compiled with Jack support!")) {
            emit error(i18n("recordMyDesktop is not compiled with Jack support."));
        } else if (line.startsWith("Error while opening/configuring soundcard")) {
            emit error(i18n("Error while opening/configuring soundcard."));
        }
    }

}


bool RecordMyDesktopRecorder::remove(const QString &file)
{

    QFile f(file);
    if (!f.remove()) {
        emit error(i18nc("%1 = file, %2 = error string", "recordmydesktop: Remove failed: %1.\n"
                         "Reason: %2", file, f.errorString()));
        return false;
    }
    return true;

}


bool RecordMyDesktopRecorder::move(const QString &from, const QString &to)
{

    QFile file;
    if (!file.rename(from, to)) {
        emit error(i18nc("%1 = source, %1 = destination, %3 = error string",
                         "Move failed: \"%1\" to \"%2\".\n"
                         "Reason: %3", from, to, file.errorString()));
        return false;
    }
    return true;

}


void RecordMyDesktopRecorder::clean()
{

    if (m_recorder) {
        m_recorder->disconnect(this);
        if (m_recorder->state() != KProcess::NotRunning) {
            m_recorder->terminate();
        }
        m_recorder->waitForFinished(-1);
        m_recorder->deleteLater();
        m_recorder = 0;
    }

}


void RecordMyDesktopRecorder::recorderFinished(int)
{

    QFile outputFile(m_data.outputFile);
    if (outputFile.exists()) {
        if (m_data.overwrite) {
            if (!remove(m_data.outputFile)) {
                clean();
                return;
            }
        } else {
            m_data.outputFile = unique(m_data.outputFile);
            emit outputFileChanged(m_data.outputFile);
        }
    }

    if (!move(m_data.tmpFile, m_data.outputFile)) {
        clean();
        return;
    }

    const KProcess::ExitStatus status = m_recorder->exitStatus();
    clean();

    if (status == KProcess::CrashExit) {
        emit finished(Crash);
    } else {
        emit finished(Normal);
    }

}


#include "recordmydesktoprecorder.moc"
