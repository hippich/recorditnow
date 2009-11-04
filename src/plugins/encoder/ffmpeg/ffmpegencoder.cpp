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
#include <ffmpeg.h>

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


void FfmpegEncoder::encode(const QString &file)
{

    emit status(i18n("Starting ffmpeg!"));

    // reload cfg
    Settings::self()->readConfig();

    m_outputFile = file;
    m_tmpFile = file;

    // fix format
    if (m_outputFile.length() > 4 && m_outputFile[m_outputFile.length()-4] == '.') {
        m_outputFile.remove(m_outputFile.length()-4, 4);
    }
    const QString format = formats[Settings::format()];
    m_outputFile.append('.'+format);

    while (QFile(m_outputFile).exists()) {
        m_outputFile.insert(m_outputFile.length()-4, '_');
    }

    emit outputFileChanged(m_outputFile);

    // tmp dir
    QString tmpDir = KGlobal::dirs()->locateLocal("tmp", "");

    if (tmpDir.isEmpty()) {
        tmpDir = QDir::homePath();
    }

    if (!tmpDir.endsWith('/')) {
        tmpDir.append('/');
    }

    m_tmpFile = tmpDir+"recorditnow_ffmpeg";
    QDir dir;
    while (dir.exists(m_tmpFile)) {
        m_tmpFile.append('_');
    }

    if (!dir.rename(file, m_tmpFile)) {
        emit error(i18n("Rename failed: \"%1\" to \"%2\"", file, m_tmpFile));
        return;
    }

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
        emit error(i18n("Cannot find ffmpeg."));
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
        emit status(i18n("Encoding: %1", progress+'%'));
        return;
    }
    kDebug() << "!parsed:" << output;

}


void FfmpegEncoder::ffmpegFinished(const int &ret)
{

    kDebug() << "ffmpeg finished:" << ret;

    QFile file(m_tmpFile);
    if (file.exists()) {
        file.remove();
    }

    m_ffmpeg->disconnect(this);
    m_ffmpeg->deleteLater();
    m_ffmpeg = 0;

    emit finished();

}

