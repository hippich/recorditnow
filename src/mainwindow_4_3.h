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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// own
#include "ui_toolbarwidget.h"
#include "libs/recorder/abstractrecorder.h"                                       
#include "libs/encoder/abstractencoder.h" 

// KDE
#include <kxmlguiwindow.h>
#include <kdeversion.h>
#include <ksystemtrayicon.h>


class KAction;
class RecorderManager;
class EncoderManager;
class RecordItNowPluginManager;
class FrameBox;
class UploadManager;
class MainWindow : public KXmlGuiWindow, public Ui::ToolBarWidget
{
    Q_OBJECT


public:
    enum State {
        Idle = 0,
        Timer = 1,
        TimerPaused = 2,
        Recording = 3,
        Paused = 4,
        Encode = 5,
        Upload = 6
    };
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void startWithArgs(const QString &backend, const QString &file, const int &time,
                       const QRect &geometry);


private:
    QWidget *m_grabber;
    FrameBox *m_box;
    KSystemTrayIcon *m_tray;
    QTimer *m_timer;
    AbstractRecorder::Data m_recordData;
    State m_state;
    RecordItNowPluginManager *m_pluginManager;
    RecorderManager *m_recorderManager;
    EncoderManager *m_encoderManager;
    UploadManager *m_uploadManager;

    KAction *getAction(const QString &name);

    void setupActions();
    void initRecorder(AbstractRecorder::Data *d);
    void initEncoder(AbstractEncoder::Data *d);
    void setupTray();
    void setTrayOverlay(const QString &name);
    void setState(const State &newState);
    inline MainWindow::State state() const;
    inline void updateRecorderCombo();


private slots:
    void startRecord();
    void pauseRecord();
    void stopRecord();
    void recordTriggred();
    void recordWindow();
    void recordCurrentWindow();
    void triggerFrame(const bool &checked);
    void recordFullScreen();
    void pluginStatus(const QString &text);
    void recorderFinished(const QString &error, const bool &isVideo);
    void encoderFinished(const QString &error);
    void configure();
    void dialogFinished();
    void startTimer();
    void tick();
    void lcdUp();
    void lcdDown();
    void trayActivated(const QSystemTrayIcon::ActivationReason &reason);
    void backendChanged(const QString &newBackend);
    void aboutToQuit();
    void pluginsChanged();
    void updateUploaderMenu();
    void upload();
    void playFile(const bool &force = true);
    void removeFile();
    void outputFileChanged(const QString &newFile);
    void recorderStateChanged(const AbstractRecorder::State &newState);
    void uploaderFinished(const QString &error);


protected:
    bool eventFilter(QObject *watched, QEvent *event);
    void hideEvent(QHideEvent *event);
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);
    void saveNewToolbarConfig();


};


#endif // MAINWINDOW_H
