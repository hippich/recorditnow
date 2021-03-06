/***************************************************************************
 *   Copyright (C) 2010 by Kai Dombrowe <just89@gmx.de>                    *
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

#ifndef RECORDITNOW_MAINWINDOW_H
#define RECORDITNOW_MAINWINDOW_H

// own
#include "ui_toolbarwidget.h"
#include "libs/recorder/abstractrecorder.h"
#include "libs/encoder/abstractencoder.h"
#include "frame/framesize.h"

// KDE
#include <kxmlguiwindow.h>
#include <kstatusnotifieritem.h>

// Qt
#include <QtCore/QPointer>


class KSqueezedTextLabel;
class KAction;
class Frame;
namespace RecordItNow {


class PlayerDock;
class DockWidget;
class KeyloggerOSD;
class ZoomDock;
class TimelineDock;
class CursorWidget;
class RecorderManager;
class EncoderManager;
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
        Config = 7
    };
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QMenu *createPopupMenu();

    void startWithArgs(const QString &backend, const QString &file, const int &time,
                       const QRect &geometry);


private:
    Frame *m_frame;
    KStatusNotifierItem *m_tray;
    AbstractRecorder::Data m_recordData;
    State m_state;
    RecordItNow::RecorderManager *m_recorderManager;
    RecordItNow::EncoderManager *m_encoderManager;
    KSqueezedTextLabel *m_statusLabel;
    QPointer<RecordItNow::CursorWidget> m_cursor;
    RecordItNow::TimelineDock *m_timelineDock;
    RecordItNow::ZoomDock *m_zoomDock;
    RecordItNow::PlayerDock *m_playerDock;
    RecordItNow::DockWidget *m_mainDock;
    RecordItNow::KeyloggerOSD *m_keyloggerOSD;

    KAction *getAction(const QString &name);
    bool isDockEnabled(QDockWidget *dock) const;

    inline void setupActions();
    inline void initRecorder(RecordItNow::AbstractRecorder::Data *d);
    inline void initEncoder(RecordItNow::AbstractEncoder::Data *d);
    inline void setupTray();
    void setTrayOverlay(const QString &name);
    void setState(const State &newState);
    inline MainWindow::State state() const;
    inline void updateRecorderCombo();
    inline void updateWindowFlags();
    void reloadPopAction();


private slots:
    void startRecord();
    void pauseRecord();
    void stopRecord();
    void recordTriggered();
    void recordWindow();
    void recordCurrentWindow(const WId &id);
    void windowGrabberCanceled();
    void triggerFrame(const bool &checked);
    void recordFullScreen();
    void pluginStatus(const QString &text);
    void recorderFinished(const QString &error, const bool &isVideo);
    void encoderFinished(const QString &error);
    void configure();
    void configDialogFinished();
    void applyConfig();
    void startTimer();
    void trayActivated(const bool &active, const QPoint &pos);
    void backendChanged(const QString &newBackend);
    void aboutToQuit();
    void pluginsChanged();
    void upload();
    void recorderStateChanged(const RecordItNow::AbstractRecorder::State &newState);
    void linkActivated(const QString &link);
    void zoomIn();
    void zoomOut();
    void resetZoomFactor();
    void setupDocks();
    void timeLineFinsihed();
    void resolutionActionTriggered();
    void frameSizesChanged(const QList<FrameSize> &sizes);
    void initRecordWidgets(const bool &start);
    void initKeyMon(const bool &start);
    void keyMonStopped();
    void errorNotification(const QString &error);
    void playRequested();
    void lockLayout(const bool &lock);


protected:
    void hideEvent(QHideEvent *event);
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);
    void saveNewToolbarConfig();


signals:
    void recordStateChanged(const RecordItNow::MainWindow::State &state, const RecordItNow::MainWindow::State &oldState);


};


} // anmespace RecordItNow


#endif // RECORDITNOW_MAINWINDOW_H
