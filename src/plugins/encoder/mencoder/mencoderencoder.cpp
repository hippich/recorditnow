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
#include "mencoderencoder.h"
#include <mencoder.h>

// KDE
#include <klocalizedstring.h>
#include <kplugininfo.h>
#include <kdebug.h>
#include <kprocess.h>
#include <kstandarddirs.h>

// Qt
#include <QtCore/QDir>

// C
#include <sys/types.h>
#include <signal.h>


static const QStringList formats = QStringList() << "avi" << "flv";


K_PLUGIN_FACTORY(myFactory, registerPlugin<MencoderEncoder>();)
K_EXPORT_PLUGIN(myFactory("mencoder_encoder"))
MencoderEncoder::MencoderEncoder(QObject *parent, const QVariantList &args)
    : AbstractEncoder(parent, args)
{

    m_mencoder = 0;

}



MencoderEncoder::~MencoderEncoder()
{

    if (m_mencoder) {
        delete m_mencoder;
    }

}


void MencoderEncoder::encode(const Data &d)
{

    emit status(i18n("Starting mencoder!"));

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
        args << "-idx";
        args << m_tmpFile;

        if (format == "avi") {
            args << "-ovc";
            args << "lavc";
            args << "-oac";
            args << "mp3lame";
            args << "-lavcopts";
            args << "vcodec=mpeg4:vqscale=2:vhq:v4mv:trell:autoaspect";
        } else if (format == "flv") {
            args << "-of";
            args << "lavf";
            args << "-oac";
            args << "mp3lame";
            args << "-ovc";
            args << "lavc";
            args << "-lavcopts";
            args << "vcodec=flv";
        } else {
            emit error(i18n("Unkown format."));
            return;
        }
        args << "-o";
        args << m_outputFile;
    } else {
        QString cmd = Settings::command();
        cmd = cmd.arg(m_tmpFile).arg(m_outputFile);
        args = cmd.split(' ');
        kDebug() << "cmd:" << cmd;
        kDebug() << "args:" << args;
    }


    // exe
    const QString exe = KGlobal::dirs()->findExe("mencoder");
    if (exe.isEmpty()) {
        emit error(i18n("Cannot find mencoder!\n"
                        "Please install mencoder or use another plugin."));
        return;
    }


    // process
    if (m_mencoder) { // should never happen
        m_mencoder->disconnect(this);
        m_mencoder->deleteLater();
    }

    m_mencoder = new KProcess(this);
    m_mencoder->setOutputChannelMode(KProcess::MergedChannels);
    m_mencoder->setProgram(exe, args);

    connect(m_mencoder, SIGNAL(finished(int)), this, SLOT(mencoderFinished(int)));
    connect(m_mencoder, SIGNAL(readyReadStandardOutput()), this, SLOT(newMencoderOutput()));

    m_mencoder->start();

}


void MencoderEncoder::pause()
{

    if (!m_paused) {
        emit status(i18n("Paused!"));
        kill(m_mencoder->pid(), SIGSTOP);
        m_paused = true;
    } else {
        emit status(i18n("Capturing!"));
        kill(m_mencoder->pid(), SIGCONT);
        m_paused = false;
    }

}


void MencoderEncoder::stop()
{

    if (m_mencoder) {
        kill(m_mencoder->pid(), SIGINT);
        m_paused = false;
    }

}


bool MencoderEncoder::remove(const QString &file)
{

    QFile f(file);
    if (!f.remove()) {
        emit error(i18nc("%1 = file, %2 = error string", "Remove failed: %1.\n"
                         "Reason: %2", file, f.errorString()));
        return false;
    }
    return true;

}


bool MencoderEncoder::move(const QString &from, const QString &to)
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


void MencoderEncoder::newMencoderOutput()
{

    QString output = m_mencoder->readAllStandardOutput().trimmed();
//"Pos:   6.7s    101f (99%) 33.95fps Trem:   0min   1mb  A-V:0.066 [1228:192]"
    if (output.contains('%')) {
        output.remove(output.indexOf('%'), output.length());
        output.remove(0, output.lastIndexOf('(')+1);
        bool ok;
        output.toInt(&ok);
        if (ok) {
            emit status(i18nc("%1 = progress in %", "Encoding: %1", output+'%'));
        } else {
            kDebug() << "!ok:" << output;
        }
    } else {
        kDebug() << "!parsed:" << output;
    }


}


void MencoderEncoder::mencoderFinished(const int &ret)
{

    kDebug() << "Mencoder finished:" << ret;

    QFile file(m_tmpFile);
    if (file.exists()) {
        remove(m_tmpFile);
    }

    m_mencoder->disconnect(this);
    m_mencoder->deleteLater();
    m_mencoder = 0;

    emit finished();

}

