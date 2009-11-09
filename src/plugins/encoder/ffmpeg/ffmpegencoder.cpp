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
#include "ffmpegencoder.h"
#include <recorditnow_ffmpeg.h>

// KDE
#include <klocalizedstring.h>
#include <kplugininfo.h>
#include <kdebug.h>
#include <kprocess.h>
#include <kstandarddirs.h>

// Qt
#include <QtCore/QDir>
#include <QtCore/QTime>

// C
#include <sys/types.h>
#include <signal.h>


static const QStringList formats = QStringList() << "avi" << "flv";


K_PLUGIN_FACTORY(myFactory, registerPlugin<FfmpegEncoder>();)
K_EXPORT_PLUGIN(myFactory("ffmpeg_encoder"))
FfmpegEncoder::FfmpegEncoder(QObject *parent, const QVariantList &args)
    : AbstractEncoder(parent, args)
{

    m_duration = -1;
    m_ffmpeg = 0;

}



FfmpegEncoder::~FfmpegEncoder()
{

    if (m_ffmpeg) {
        delete m_ffmpeg;
    }

}


void FfmpegEncoder::encode(const Data &d)
{

    emit status(i18n("Starting ffmpeg!"));

    // check input file
    if (!QFile::exists(d.file)) { // should never happen
        emit error(i18nc("%1 = file", "%1 no such file!", d.file));
        return;
    }

    // reload cfg
    Settings::self()->readConfig();

    m_outputFile = d.file;
    m_tmpFile = getTemporaryFile(d.workDir);


    // move to wokdir
    if (!move(d.file, m_tmpFile)) {
        return;
    }


    // fix format
    if (m_outputFile.length() > 4 && m_outputFile[m_outputFile.length()-4] == '.') {
        m_outputFile.remove(m_outputFile.length()-4, 4);
    }
    const QString format = formats[Settings::format()];
    m_outputFile.append('.'+format);


    // set output file
    if (!d.overwrite) {
        m_outputFile = unique(m_outputFile);
    } else {
        QFile file(m_outputFile);
        if (file.exists()) {
            if (!remove(m_outputFile)) {
                return;
            }
        }
    }
    emit outputFileChanged(m_outputFile); // update gui


    // args
    QStringList args;
    if (Settings::useFormat()) {
        args << "-i";
        args << m_tmpFile;

        if (format == "avi") {
        } else if (format == "flv") {
        } else {
            emit error(i18n("Unkown format."));
            return;
        }
        args << "-sameq";
        args << "-xerror";
        args << m_outputFile;
    } else {
        QString cmd = Settings::command();
        cmd = cmd.arg(m_tmpFile).arg(m_outputFile);
        args = cmd.split(' ');
        kDebug() << "cmd:" << cmd;
        kDebug() << "args:" << args;
    }

    // exe
    const QString exe = KGlobal::dirs()->findExe("ffmpeg");
    if (exe.isEmpty()) {
        emit error(i18n("Cannot find ffmpeg!\n"
                        "Please install ffmpeg or use another plugin."));
        return;
    }

    // process
    if (m_ffmpeg) { // should never happen
        m_ffmpeg->disconnect(this);
        m_ffmpeg->deleteLater();
    }

    m_ffmpeg = new KProcess(this);
    m_ffmpeg->setOutputChannelMode(KProcess::MergedChannels);
    m_ffmpeg->setProgram(exe, args);

    connect(m_ffmpeg, SIGNAL(finished(int)), this, SLOT(ffmpegFinished(int)));
    connect(m_ffmpeg, SIGNAL(readyReadStandardOutput()), this, SLOT(newFfmpegOutput()));

    m_ffmpeg->start();

}


void FfmpegEncoder::pause()
{

    if (!m_paused) {
        emit status(i18n("Paused!"));
        kill(m_ffmpeg->pid(), SIGSTOP);
        m_paused = true;
    } else {
        emit status(i18n("Capturing!"));
        kill(m_ffmpeg->pid(), SIGCONT);
        m_paused = false;
    }

}


void FfmpegEncoder::stop()
{

    if (m_ffmpeg) {
        kill(m_ffmpeg->pid(), SIGINT);
        m_paused = false;
    }

}


bool FfmpegEncoder::remove(const QString &file)
{

    QFile f(file);
    if (!f.remove()) {
        emit error(i18nc("%1 = file, %2 = error string", "Remove failed: %1.\n"
                         "Reason: %2", file, f.errorString()));
        return false;
    }
    return true;

}


bool FfmpegEncoder::move(const QString &from, const QString &to)
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


void FfmpegEncoder::newFfmpegOutput()
{

    QString output = m_ffmpeg->readAllStandardOutput().trimmed();
    if (output.contains(QRegExp("[0-9][0-9]:[0-9][0-9]:[0-9][0-9]"))) {
        output.remove(QRegExp(".*Duration:"));
        output.remove(output.indexOf(','), output.length());
        output = output.trimmed();
        if (output.contains('.')) {
            output.remove(output.indexOf('.'), output.length());
        }
        
        QTime time = QTime::fromString(output, "hh:mm:ss");
        m_duration = time.second();
        m_duration += (time.minute()*60);
        m_duration += (time.hour()*60*60);
        return;
    } else if (output.contains("time=") && m_duration > 0) {
        output.remove(QRegExp(".*time="));
        output = output.trimmed();
        output.remove(QRegExp(" .*"));
        if (output.contains('.')) {
            output.remove(output.indexOf('.'), output.length());
        }
        bool ok;
        const int time = output.toInt(&ok);
        if (!ok) {
            return;
        }
        const QString progress = QString::number((time*100)/m_duration);
        emit status(i18nc("%1 = progress in %", "Encoding: %1", progress+'%'));
        return;
    }
    kDebug() << "!parsed:" << output;

}


void FfmpegEncoder::ffmpegFinished(const int &ret)
{

    kDebug() << "ffmpeg finished:" << ret;

    QFile file(m_tmpFile);
    if (file.exists()) {
        remove(m_tmpFile);
    }

    m_ffmpeg->disconnect(this);
    m_ffmpeg->deleteLater();
    m_ffmpeg = 0;

    emit finished(ret == 0 ? Normal: Crash);

}

