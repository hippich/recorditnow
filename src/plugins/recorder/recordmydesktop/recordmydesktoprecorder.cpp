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
    m_paused = false;

    // cfg defaults

    // image
    m_cfg["--no-cursor"] = false;
    m_cfg["--no-shared"] = false;
    m_cfg["--full-shots"] = false;
    m_cfg["--quick-subsampling"] = false;

    // sound
    m_cfg["--channels"] = -1;
    m_cfg["--freq"] = -1;
    m_cfg["--buffer-size"] = -1;
    m_cfg["--ring-buffer-size"] = -1;
    m_cfg["--device"] = "hw:0,0";
    m_cfg["--use-jack"] = "";

    // encoding
    m_cfg["--on-the-fly-encoding"] = false;
    m_cfg["--v_quality"] = 63;
    m_cfg["--v_bitrate"] = 45000;
    m_cfg["--s_quality"] = 10;

    // misc
    m_cfg["--no-wm-check"] = false;
    m_cfg["--compress-cache"] = false;
    m_cfg["--workdir"] = QDir::homePath();
    m_cfg["--overwrite"] = false;

}


RecordMyDesktopRecorder::~RecordMyDesktopRecorder()
{

    if (m_recorder) {
        delete m_recorder;
    }

}


bool RecordMyDesktopRecorder::canRecordSound() const
{

    return true;

}


bool RecordMyDesktopRecorder::hasConfigPage() const
{

    return true;

}


QWidget *RecordMyDesktopRecorder::configPage()
{

    QWidget *widget = new QWidget;
    ui_cfg.setupUi(widget);

    // image
    ui_cfg.noCursorCheck->setChecked(m_cfg["--no-cursor"].toBool());
    ui_cfg.noSharedCheck->setChecked(m_cfg["--no-shared"].toBool());
    ui_cfg.fullShotsCheck->setChecked(m_cfg["--full-shots"].toBool());
    ui_cfg.quickSubsamplingCheck->setChecked(m_cfg["--quick-subsampling"].toBool());

    // sound
    ui_cfg.deviceEdit->setText(m_cfg["--device"].toString());
    ui_cfg.channelsSpin->setValue(m_cfg["--channels"].toInt());
    ui_cfg.freqSpin->setValue(m_cfg["--freq"].toInt());
    ui_cfg.bufferSizeSpin->setValue(m_cfg["--buffer-size"].toInt());
    ui_cfg.ringBufferSpin->setValue(m_cfg["--ring-buffer-size"].toInt());
    ui_cfg.jackEdit->setText(m_cfg["--use-jack"].toString());

    // encoding
    ui_cfg.onTheFlyCheck->setChecked(m_cfg["--on-the-fly-encoding"].toBool());
    ui_cfg.videoQualitySpin->setValue(m_cfg["--v_quality"].toInt());
    ui_cfg.audioQualitySpin->setValue(m_cfg["--s_quality"].toInt());
    ui_cfg.bitrateSpin->setValue(m_cfg["--v_bitrate"].toInt());

    // misc
    ui_cfg.overwriteCheck->setChecked(m_cfg["--overwrite"].toBool());
    ui_cfg.wmCheck->setChecked(m_cfg["--no-wm-check"].toBool());
    ui_cfg.compressCacheCheck->setChecked(m_cfg["--compress-cache"].toBool());
    ui_cfg.tmpRequester->setText(m_cfg["--workdir"].toString());

    return widget;

}


void RecordMyDesktopRecorder::record(const Data &d)
{

    kDebug() << "record";

    const QString recordMyDesktop = KGlobal::dirs()->findExe("recordmydesktop");
    if (recordMyDesktop.isEmpty()) {
        emit error(i18n("Cannot find recordmydesktop."));
        return;
    }

    QStringList args;

    // default cfg
    if (!d.sound) {
        args << "--no-sound";
    }
    args << "-o" << d.outputFile;

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
    args << "-fps" << QString::number(d.fps);

    // recordmydesktop cfg
    // image
    if (m_cfg["--no-cursor"].toBool()) {
        args << "--no-cursor";
    }
    if (m_cfg["--no-shared"].toBool()) {
        args << "--no-shared";
    }
    if (m_cfg["--full-shots"].toBool()) {
        args << "--full-shots";
    }
    if (m_cfg["--quick-subsampling"].toBool()) {
        args << "--quick-subsampling";
    }

    // sound
    if (m_cfg["--channels"].toInt() != -1) {
        args << "--channels" << QString::number(m_cfg["--channels"].toInt());
    }
    if (m_cfg["--freq"].toInt() != -1) {
        args << "--freq" << QString::number(m_cfg["--freq"].toInt());
    }
    if (m_cfg["--buffer-size"].toInt() != -1) {
        args << "--buffer-size" << QString::number(m_cfg["--buffer-size"].toInt());
    }
    if (m_cfg["--ring-buffer-size"].toInt() != -1) {
        args << "--ring-buffer-size" << QString::number(m_cfg["--ring-buffer-size"].toInt());
    }
    if (m_cfg["--device"].toString() != "hw:0,0" && m_cfg["--device"].toString().isEmpty()) {
        args << "--device" << m_cfg["--device"].toString();
    }
    if (!m_cfg["--use-jack"].toString().isEmpty()) {
        args << "--use-jack" << m_cfg["--use-jack"].toString();
    }

    // encoding
    if (m_cfg["--on-the-fly-encoding"].toBool()) {
        args << "--on-the-fly-encoding";
    }

    args << "--v_quality" << QString::number(m_cfg["--v_quality"].toInt());
    args << "--v_bitrate" << QString::number(m_cfg["--v_bitrate"].toInt());
    args << "--s_quality" << QString::number(m_cfg["--s_quality"].toInt());

    // misc
    if (m_cfg["--no-wm-check"].toBool()) {
        args << "--no-wm-check";
    }
    if (m_cfg["--compress-cache"].toBool()) {
        args << "--compress-cache";
    }

    args << "--workdir" << m_cfg["--workdir"].toString();

    if (m_cfg["--overwrite"].toBool()) {
        args << "--overwrite";
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

    if (!m_paused) {
        emit status(i18n("Paused!"));
        kill(m_recorder->pid(), SIGSTOP);
        m_paused = true;
    } else {
        emit status(i18n("Capturing!"));
        kill(m_recorder->pid(), SIGCONT);
        m_paused = false;
    }

}


void RecordMyDesktopRecorder::stop()
{

    if (m_recorder) {
        kill(m_recorder->pid(), SIGINT);
        m_paused = false;
    }

}


void RecordMyDesktopRecorder::saveConfig(KSharedConfig::Ptr cfg)
{

    kDebug() << "save cfg";
    KConfigGroup group(cfg, "RecordMyDesktop");

    // image
    m_cfg["--no-cursor"] = ui_cfg.noCursorCheck->isChecked();
    m_cfg["--no-shared"] = ui_cfg.noSharedCheck->isChecked();
    m_cfg["--full-shots"] = ui_cfg.fullShotsCheck->isChecked();
    m_cfg["--quick-subsampling"] = ui_cfg.quickSubsamplingCheck->isChecked();

    // sound
    m_cfg["--device"] = ui_cfg.deviceEdit->text();
    m_cfg["--channels"] = ui_cfg.channelsSpin->value();
    m_cfg["--freq"] = ui_cfg.freqSpin->value();
    m_cfg["--buffer-size"] = ui_cfg.bufferSizeSpin->value();
    m_cfg["--ring-buffer-size"] = ui_cfg.ringBufferSpin->value();
    m_cfg["--use-jack"] = ui_cfg.jackEdit->text();

    // encoding
    m_cfg["--on-the-fly-encoding"] = ui_cfg.onTheFlyCheck->isChecked();
    m_cfg["--v_quality"] = ui_cfg.videoQualitySpin->value();
    m_cfg["--s_quality"] = ui_cfg.audioQualitySpin->value();
    m_cfg["--v_bitrate"] = ui_cfg.bitrateSpin->value();

    // misc
    m_cfg["--overwrite"] = ui_cfg.overwriteCheck->isChecked();
    m_cfg["--no-wm-check"] = ui_cfg.wmCheck->isChecked();
    m_cfg["--compress-cache"] = ui_cfg.compressCacheCheck->isChecked();
    m_cfg["--workdir"] = ui_cfg.tmpRequester->text();

    QHashIterator<QString, QVariant> it(m_cfg);
    while (it.hasNext()) {
        it.next();
        group.writeEntry(it.key(), it.value());
    }
    group.sync();

}


void RecordMyDesktopRecorder::loadConfig(KSharedConfig::Ptr cfg)
{

    kDebug() << "load cfg";
    KConfigGroup group(cfg, "RecordMyDesktop");
    QHashIterator<QString, QVariant> it(m_cfg);
    while (it.hasNext()) {
        it.next();
        m_cfg[it.key()] = group.readEntry(it.key(), it.value()); // i love QHash :)
    }

}


void RecordMyDesktopRecorder::newRecorderOutput()
{

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
            emit status(i18n("Output file: %1", line.remove(0, 13)));
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
        }
    }

}


void RecordMyDesktopRecorder::recorderFinished(int)
{

    if (m_recorder->exitStatus() == KProcess::CrashExit) {
        emit finished(Crash);
    } else {
        emit finished(Normal);
    }

    m_recorder->deleteLater();
    m_recorder = 0;

}


#include "recordmydesktoprecorder.moc"
