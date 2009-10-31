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
#include "ui_settings.h"
#include "ui_recorderplugins.h"

// KDE
#include <kxmlguiwindow.h>
#include <kdeversion.h>
#if (KDE_VERSION >= KDE_MAKE_VERSION(4,3,64))
#include <knotificationitem.h>
#else
#include <knotificationitem-1/knotificationitem.h>
#endif


class Settings;
class RecordItNowPluginManager;
class FrameBox;
class MainWindow : public KXmlGuiWindow, public Ui::ToolBarWidget
{
    Q_OBJECT


public:
    enum State {
        Idle = 0,
        Recording = 1,
        Paused = 2
    };
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void startWithArgs(const QString &backend, const QString &file, const int &time,
                       const QRect &geometry);


private:
    QWidget *m_grabber;
    Ui::Settings ui_settings;
    Ui::RecorderPlugins ui_recorder;
    FrameBox *m_box;
    AbstractRecorder *m_recorderPlugin;
#if (KDE_VERSION >= KDE_MAKE_VERSION(4,3,64))
    KNotificationItem *m_tray;
#else
    Experimental::KNotificationItem *m_tray;
#endif
    QTimer *m_timer;
    AbstractRecorder::Data m_recordData;
    State m_state;
    RecordItNowPluginManager *m_pluginManager;
    QHash<AbstractRecorder::Feature, bool> m_currentFeatures;

    void setupActions();
    void initRecorder(AbstractRecorder::Data *d);
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
    void recorderStatus(const QString &text);
    void recorderError(const QString &error);
    void recorderFinished(const AbstractRecorder::ExitStatus &status);
    void configure();
    void saveConfig(int code);
    void startTimer();
    void tick();
    void lcdUp();
    void lcdDown();
    void trayActivated(const bool &active, const QPoint &pos);
    void backendChanged(const QString &newBackend);
    void aboutToQuit();
    void pluginsChanged();


protected:
    bool eventFilter(QObject *watched, QEvent *event);
    void hideEvent(QHideEvent *event);
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);


};


#endif // MAINWINDOW_H
