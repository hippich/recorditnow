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

// own
#include "mainwindow.h"
#include "frame/frame.h"
#include <recorditnow.h>
#include "pluginmanager.h"
#include "config/configdialog.h"
#include "recordermanager.h"
#include "encodermanager.h"
#include "mouse/cursorwidget.h"
#include "config/mouseconfig.h"
#include "zoom/zoomview.h"
#include "timeline/timeline.h"
#include "timeline/timelinedock.h"
#include "upload/uploadwizard.h"
#include "keyboard/keyloggerosd.h"
#include "keymonmanager.h"
#include "config/frameconfig.h"
#include "config/keyboardconfig.h"
#include "zoom/zoomdock.h"
#include "windowgrabber.h"
#include "player/playerdock.h"
#include "helper.h"
#include <config-recorditnow.h>
#ifdef HAVE_QTSCRIPT
    #include "script/mainwindowscriptadaptor.h"
    #include "script/scriptmanager.h"
#endif
#include "widgets/recordhelpwidget.h"

// Qt
#include <QtGui/QMouseEvent>
#include <QtGui/QDesktopWidget>

// KDE
#include <kdebug.h>
#include <kstatusbar.h>
#include <kmenubar.h>
#include <kmessagebox.h>
#include <kmenu.h>
#include <krun.h>
#include <kapplication.h>
#include <kactionmenu.h>
#include <ksqueezedtextlabel.h>
#include <kactioncategory.h>
#include <knotification.h>
#include <ktoolbar.h>
#include <kfiledialog.h>


namespace RecordItNow {


MainWindow::MainWindow(QWidget *parent)
    : KXmlGuiWindow(parent)
{

    m_frame = new Frame(this);
    m_frame->setFrameSize(FrameSize(Settings::currentFrame().size(), QString()));

    setupActions();

   // setCentralWidget(new QWidget(this)); // just a placeholder
   // centralWidget()->setMaximumSize(1, 1);

    setDockOptions(KXmlGuiWindow::AllowNestedDocks|
                   KXmlGuiWindow::AllowTabbedDocks|
                   KXmlGuiWindow::AnimatedDocks);

    QWidget *toolWidget = new QWidget(this);
    setupUi(toolWidget);

    m_mainDock = new RecordItNow::DockWidget(this);
    m_mainDock->setDockTitle(i18n("RecordItNow"));
    m_mainDock->setObjectName("_RecordItNow_Main_Dock_");
    m_mainDock->setFeatures(QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetFloatable);
    m_mainDock->setWidget(toolWidget);
    
    QWidget *dockTitle = new QWidget(m_mainDock);
    QHBoxLayout *titleLayout = new QHBoxLayout;
    titleLayout->setContentsMargins(0, 0, 0, 0);
    
    QLabel *titleLabel = new QLabel(dockTitle);
    titleLabel->setText(i18n("RecordItNow"));
    
    titleLayout->addWidget(titleLabel);
    m_mainDock->setTitleBarWidget(dockTitle);
    
    addDockWidget(Qt::TopDockWidgetArea, m_mainDock);

    
    soundCheck->setIcon("preferences-desktop-sound");
    mouseCheck->setIcon("input-mouse");
    keyboardCheck->setIcon("input-keyboard");

    connect(backendCombo, SIGNAL(currentIndexChanged(QString)), this,
            SLOT(backendChanged(QString)));
    connect(kapp, SIGNAL(aboutToQuit()), this, SLOT(aboutToQuit()));

    connect(outputWidget, SIGNAL(error(QString)), this, SLOT(errorNotification(QString)));
    connect(timerWidget, SIGNAL(timeout()), this, SLOT(startRecord()));
    connect(timerWidget, SIGNAL(canceled()), this, SLOT(stopRecord()));
    connect(outputWidget, SIGNAL(playRequested()), this, SLOT(playRequested()));
    connect(KeyMonManager::self(), SIGNAL(stopped()), this, SLOT(keyMonStopped()));

    m_tray = 0;
    m_timelineDock = 0;
    m_keyloggerOSD = 0;
    m_zoomDock = 0;
    m_playerDock = 0;

    connect(Helper::self()->pluginmanager(), SIGNAL(pluginsChanged()), this, SLOT(pluginsChanged()));

    m_recorderManager = new RecordItNow::RecorderManager(this);
    connect(m_recorderManager, SIGNAL(status(QString)), this, SLOT(pluginStatus(QString)));
    connect(m_recorderManager, SIGNAL(fileChanged(QString)), outputWidget, SLOT(setOutputFile(QString)));
    connect(m_recorderManager, SIGNAL(finished(QString,bool)), this,
            SLOT(recorderFinished(QString,bool)));
    connect(m_recorderManager, SIGNAL(stateChanged(RecordItNow::AbstractRecorder::State)), this,
            SLOT(recorderStateChanged(RecordItNow::AbstractRecorder::State)));

    m_encoderManager = new RecordItNow::EncoderManager(this);
    connect(m_encoderManager, SIGNAL(status(QString)), this, SLOT(pluginStatus(QString)));
    connect(m_encoderManager, SIGNAL(fileChanged(QString)), outputWidget, SLOT(setOutputFile(QString)));
    connect(m_encoderManager, SIGNAL(finished(QString)), this,
            SLOT(encoderFinished(QString)));


    m_statusLabel = new KSqueezedTextLabel(this);
    connect(m_statusLabel, SIGNAL(linkActivated(QString)), this, SLOT(linkActivated(QString)));
    statusBar()->addPermanentWidget(m_statusLabel, 1);

    setupDocks();
    setupTray();
    setupGUI();

    // default dock cfg, must be called after setupGUI()
    if (RecordItNow::Helper::self()->firstStart()) {
        m_timelineDock->close();
        m_mainDock->raise(); // the main dock should be activated
    }

    updateWindowFlags();

    menuBar()->clear();
    setState(Idle);
    Helper::self()->pluginmanager()->init();

    backendCombo->setCurrentItem(Settings::currentBackend(), false);
    soundCheck->setChecked(Settings::soundEnabled());
    fpsSpinBox->setValue(Settings::currentFps());
    timerWidget->setValue(Settings::currentTime());
    mouseCheck->setChecked(Settings::mouseMonitorEnabled());
    keyboardCheck->setChecked(Settings::keyboardMonitorEnabled());

    reloadPopAction();
    lockLayout(Settings::lockLayout());

    // don't show the toggleviewaction from our mainToolBar
    if (toolBarMenuAction()) {
        foreach (QWidget *widget, toolBarMenuAction()->associatedWidgets()) {
            widget->removeAction(toolBarMenuAction());
        }
    }

#ifdef HAVE_QTSCRIPT
    RecordItNow::Helper::self()->setMainWindow(this);
    connect(Helper::self()->scriptManager(), SIGNAL(scriptError(QString)), this, SLOT(errorNotification(QString)));
    RecordItNow::Helper::self()->scriptManager()->reloadScripts();
#endif

}


MainWindow::~MainWindow()
{

#ifdef HAVE_QTSCRIPT
    RecordItNow::Helper::self()->unloadScriptManager();
#endif

    Settings::self()->setCurrentBackend(backendCombo->currentText());
    Settings::self()->setSoundEnabled(soundCheck->isChecked());
    Settings::self()->setCurrentFps(fpsSpinBox->value());
    Settings::self()->setCurrentFrame(m_frame->getFrameGeometry());
    Settings::self()->setCurrentTime(timerWidget->value());
    Settings::self()->setLockLayout(getAction("lockLayout")->isChecked());
    Settings::self()->setMouseMonitorEnabled(mouseCheck->isChecked());
    Settings::self()->setKeyboardMonitorEnabled(keyboardCheck->isChecked());
    Settings::self()->writeConfig();

    delete m_frame;

    if (m_tray) {
        delete m_tray;
    }
    delete m_recorderManager;
    delete m_encoderManager;

    if (m_cursor) {
        delete m_cursor;
    }

    if (m_keyloggerOSD) {
        delete m_keyloggerOSD;
    }

}


QMenu *MainWindow::createPopupMenu()
{

    QMenu *menu = KXmlGuiWindow::createPopupMenu();
    if (!menu) {
        menu = new QMenu(this);
    }

    menu->removeAction(toolBar()->toggleViewAction());

    KAction *lockAction = getAction("lockLayout");
    if (menu->actions().isEmpty()) {
        menu->addAction(lockAction);
        menu->addAction(getAction("options_configure_toolbars"));
        menu->addSeparator();
    } else {
        menu->insertAction(menu->actions().first(), lockAction);
        menu->insertAction(lockAction, getAction("options_configure_toolbars"));
        menu->insertSeparator(menu->actions().at(2));
    }

    return menu;

}


void MainWindow::startWithArgs(const QString &backend, const QString &file, const int &time,
                               const QRect &geometry)
{

    if (!backend.isEmpty() && !backendCombo->contains(backend)) {
        kWarning() << "Backend:" << backend << "not found...";
    } else if (!backend.isEmpty()) {
        backendCombo->setCurrentItem(backend, false);
    }

    if (!file.isEmpty()) {
        outputWidget->setOutputFile(file);
    }

    QRect rect = geometry;
    if (rect == QRect(-1, -1, -1, -1)) {
        rect = QApplication::desktop()->screenGeometry(x11Info().appScreen());
    }

    initRecorder(&m_recordData);
    m_recordData.geometry = rect;

    timerWidget->setValue(time);

    startTimer();

}


KAction *MainWindow::getAction(const QString &name)
{

    KAction *action = static_cast<KAction*>(actionCollection()->action(name));
    if (!action) {
        if (name == "box" || name == "popupAction") {
            action = new KActionMenu(this);
            if (name == "popupAction") {
                static_cast<KActionMenu*>(action)->setDelayed(false);
            }
        } else {
            action = new KAction(this);
        }
        action->setShortcutConfigurable(true);
        actionCollection()->addAction(name, action);
    }
    return action;

}


bool MainWindow::isDockEnabled(QDockWidget *dock) const
{

    return dock->toggleViewAction()->isChecked();

}


void MainWindow::setupActions()
{

    KAction *recordAction = getAction("record");
    recordAction->setText(i18n("Record"));
    recordAction->setIcon(KIcon("media-record"));
    recordAction->setShortcut(Qt::CTRL+Qt::Key_R, KAction::DefaultShortcut);
    connect(recordAction, SIGNAL(triggered()), this, SLOT(recordTriggered()));

    
    KAction *pauseAction = getAction("pause");
    pauseAction->setObjectName("pause");
    pauseAction->setText(i18n("Pause"));
    pauseAction->setIcon(KIcon("media-playback-pause"));
    pauseAction->setShortcut(Qt::CTRL+Qt::Key_P, KAction::DefaultShortcut);
    pauseAction->setEnabled(false);
    connect(pauseAction, SIGNAL(triggered()), this, SLOT(pauseRecord()));

    KAction *stopAction = getAction("stop");
    stopAction->setText(i18n("Stop"));
    stopAction->setIcon(KIcon("media-playback-stop"));
    stopAction->setShortcut(Qt::CTRL+Qt::Key_S, KAction::DefaultShortcut);
    stopAction->setEnabled(false);
    connect(stopAction, SIGNAL(triggered()), this, SLOT(stopRecord()));


    KAction *recordWindowAction = getAction("recordWindow");
    recordWindowAction->setText(i18n("Record a Window"));
    recordWindowAction->setIcon(KIcon("edit-select"));
    recordWindowAction->setShortcut(Qt::CTRL+Qt::Key_W, KAction::DefaultShortcut);
    connect(recordWindowAction, SIGNAL(triggered()), this, SLOT(recordWindow()));

    KActionMenu *boxAction = static_cast<KActionMenu*>(getAction("box"));
    boxAction->setText(i18n("Show Frame"));
    boxAction->setIcon(KIcon("recorditnow-frame"));
    boxAction->setShortcut(Qt::CTRL+Qt::Key_F, KAction::DefaultShortcut);
    boxAction->setCheckable(true);
    connect(boxAction, SIGNAL(triggered(bool)), this, SLOT(triggerFrame(bool)));

    KAction *frameMoveAction = getAction("frame_move");
    frameMoveAction->setCheckable(true);
    frameMoveAction->setText(i18n("Freely movable"));
    frameMoveAction->setIcon(KIcon("transform-move"));
    connect(frameMoveAction, SIGNAL(triggered(bool)), m_frame, SLOT(setMoveEnabled(bool)));

    boxAction->addAction(frameMoveAction);
    boxAction->addSeparator();

    const QList<FrameSize> sizes = FrameConfig::readSizes(Settings::self()->config());
    frameSizesChanged(sizes);

    KAction *fullAction = getAction("recordFullScreen");
    fullAction->setText(i18n("Record the entire Screen"));
    fullAction->setIcon(KIcon("view-fullscreen"));
    fullAction->setShortcut(Qt::CTRL+Qt::Key_A, KAction::DefaultShortcut);
    connect(fullAction, SIGNAL(triggered()), this, SLOT(recordFullScreen()));


    KAction *uploadAction = getAction("upload");
    uploadAction->setIcon(KIcon("recorditnow-upload-media"));
    uploadAction->setText(i18n("Upload"));
    uploadAction->setShortcut(Qt::CTRL+Qt::Key_U, KAction::DefaultShortcut);
    connect(uploadAction, SIGNAL(triggered()), this, SLOT(upload()));

    KAction *zoomInAction = getAction("zoom-in");
    zoomInAction->setIcon(KIcon("zoom-in"));
    zoomInAction->setText(i18n("Zoom in"));
    zoomInAction->setGlobalShortcut(KShortcut(Qt::META+Qt::CTRL+Qt::Key_Plus));
    connect(zoomInAction, SIGNAL(triggered()), this, SLOT(zoomIn()));

    KAction *zoomOutAction = getAction("zoom-out");
    zoomOutAction->setIcon(KIcon("zoom-out"));
    zoomOutAction->setText(i18n("Zoom out"));
    zoomOutAction->setGlobalShortcut(KShortcut(Qt::META+Qt::CTRL+Qt::Key_Minus));
    connect(zoomOutAction, SIGNAL(triggered()), this, SLOT(zoomOut()));

    KAction *zoomDefaultAction = getAction("zoom-default");
    zoomDefaultAction->setIcon(KIcon("zoom-original"));
    zoomDefaultAction->setText(i18n("Default Zoom Factor"));
    zoomDefaultAction->setGlobalShortcut(KShortcut(Qt::META+Qt::CTRL+Qt::Key_O));
    connect(zoomDefaultAction, SIGNAL(triggered()), this, SLOT(resetZoomFactor()));

    KAction *popAction = getAction("popupAction");
    popAction->setIcon(KIcon("configure-toolbars"));
    popAction->setText(i18n("Configure toolbars"));
    popAction->setShortcut(Qt::CTRL+Qt::Key_T, KAction::DefaultShortcut);


    KAction *lockLayoutAction = getAction("lockLayout");
    lockLayoutAction->setText(i18n("Lock layout"));
    lockLayoutAction->setIcon(KIcon("object-locked"));
    lockLayoutAction->setCheckable(true);
    lockLayoutAction->setShortcut(Qt::CTRL+Qt::Key_L, KAction::DefaultShortcut);
    connect(lockLayoutAction, SIGNAL(triggered(bool)), this, SLOT(lockLayout(bool)));


    KStandardAction::preferences(this, SLOT(configure()), actionCollection());
    KStandardAction::configureToolbars(this, SLOT(configureToolbars()), actionCollection());

}


void MainWindow::startRecord()
{

    setState(Recording);
    m_recorderManager->startRecord(backendCombo->currentText(), m_recordData, m_cursor);

}


void MainWindow::pauseRecord()
{

    if (state() == Timer) {
        timerWidget->pause();
        setState(TimerPaused);
        return;
    } else if (state() == TimerPaused) {
        startTimer();
    } else {
        if (state() == Recording) {
            setState(Paused);
        } else {
            setState(Recording);
        }
        m_recorderManager->pauseOrContinue();
        m_encoderManager->pauseOrContinue();
        if (isDockEnabled(m_timelineDock)) {
            m_timelineDock->timeline()->pauseOrContinue();
        }
    }

}


void MainWindow::stopRecord()
{

    if (state() == Timer || state() == TimerPaused) {
        timerWidget->pause();
        timerWidget->reset();
        setState(Idle);
    } else {
        m_recorderManager->stop();
        m_encoderManager->stop();
    }

    initRecordWidgets(false);

}


void MainWindow::recordTriggered()
{

    if (!m_frame->isVisible()) {
        KDialog *dialog = new KDialog(this);
        dialog->setAttribute(Qt::WA_DeleteOnClose, true);
        RecordItNow::RecordHelpWidget *widget = new RecordItNow::RecordHelpWidget(getAction("box"),
                                                                                  getAction("recordFullScreen"),
                                                                                  getAction("recordWindow"),
                                                                                  dialog);
        dialog->setMainWidget(widget);
        connect(widget, SIGNAL(clicked()), dialog, SLOT(close()));
        dialog->setCaption(widget->windowTitle());
        dialog->setButtons(KDialog::Cancel);

        dialog->show();
        return;
    } else if (!m_frame->validFrameGeometry()) {
        KMessageBox::error(this, i18n("The recording area is located outside of the screen."));

        return;
    }

    initRecorder(&m_recordData);
    m_recordData.geometry = m_frame->getFrameGeometry();

    startTimer();
    
}


void MainWindow::recordWindow()
{

    RecordItNow::WindowGrabber *grabber = new RecordItNow::WindowGrabber(this);
    connect(grabber, SIGNAL(grabCanceled()), this, SLOT(windowGrabberCanceled()));
    connect(grabber, SIGNAL(windowGrabbed(WId)), this, SLOT(recordCurrentWindow(WId)));

    grabber->startGrab();

}


void MainWindow::recordCurrentWindow(const WId &id)
{

    initRecorder(&m_recordData);
    m_recordData.winId = id;
    startTimer();

}


void MainWindow::windowGrabberCanceled()
{

    setState(Idle);

}


void MainWindow::triggerFrame(const bool &checked)
{

    if (!isVisible()) {
        m_frame->setVisible(false);
    } else {
        m_frame->setVisible(checked);
    }

}


void MainWindow::recordFullScreen()
{

    initRecorder(&m_recordData);

    const int screen = x11Info().appScreen();
    QWidget *desktop = kapp->desktop()->screen(screen);
    if (desktop) {
        m_recordData.winId = desktop->winId();
    } else {
        KMessageBox::error(this, i18n("An internal error occurred!"));
        return;
    }

    startTimer();

}


void MainWindow::initRecorder(RecordItNow::AbstractRecorder::Data *d)
{

    if (m_frame->isVisible()) {
        m_frame->setVisible(false);
    }

    d->geometry = QRect(-1, -1, -1, -1);
    d->winId = -1;

    d->fps = fpsSpinBox->value();
    d->sound = soundCheck->isChecked();
    d->outputFile = outputWidget->outputFile();
    d->overwrite = Settings::overwrite();
    d->workDir = Settings::workDir().path();
    d->mouseMarkSize = Settings::cursorWidgetSize();
    
    pluginStatus(""); // clear the status bar

}


void MainWindow::initEncoder(RecordItNow::AbstractEncoder::Data *d)
{

    d->overwrite = Settings::overwrite();
    d->file = outputWidget->outputFile();
    d->workDir = Settings::workDir().path();

}


void MainWindow::setupTray()
{

    if (Settings::tray()) {
        if (!m_tray) {
            m_tray = new KStatusNotifierItem(this);
            m_tray->setStatus(KStatusNotifierItem::Active);
            m_tray->setCategory(KStatusNotifierItem::ApplicationStatus);
            m_tray->setIconByName("recorditnow");
            m_tray->setToolTip("recorditnow", i18n("RecordItNow"), "");
            connect(m_tray, SIGNAL(activateRequested(bool,QPoint)), this,
                    SLOT(trayActivated(bool,QPoint)));

            KMenu *context = new KMenu(this);
            context->addAction(getAction("record"));
            context->addAction(getAction("pause"));
            context->addAction(getAction("stop"));
            context->addSeparator();
            context->addAction(getAction("box"));
            context->addAction(getAction("recordWindow"));
            context->addAction(getAction("recordFullScreen"));
            m_tray->setContextMenu(context);
        }
    } else {
        if (m_tray) {
            delete m_tray;
            m_tray = 0;
        }
    }

}


void MainWindow::setTrayOverlay(const QString &name)
{

    if (m_tray) {
        m_tray->setIconByName("recorditnow");
        m_tray->setOverlayIconByName(name);
    }

}


void MainWindow::setState(const State &newState)
{

    const RecordItNow::MainWindow::State oldState = m_state;
    m_state = newState;

    QList<QWidget*> widgets;
    widgets << soundCheck;
    widgets << keyboardCheck;
    widgets << mouseCheck;
    widgets << fpsSpinBox;
    widgets << backendCombo;
    widgets << outputWidget;
    widgets << timerWidget;

    const QString recorder = backendCombo->currentText();
    switch (newState) {
    case Idle: {
            setTrayOverlay("");
            getAction("pause")->setIcon(KIcon("media-playback-pause"));
            getAction("record")->setEnabled(true);
            getAction("pause")->setEnabled(false);
            getAction("stop")->setEnabled(false);
            getAction("recordWindow")->setEnabled(true);
            getAction("recordFullScreen")->setEnabled(true);
            getAction("box")->setEnabled(true);
            getAction("box")->setChecked(m_frame->isVisible());
            getAction("options_configure")->setEnabled(true);
            getAction("upload")->setEnabled(true);
            getAction("zoom-in")->setEnabled(true);
            getAction("zoom-out")->setEnabled(true);
            getAction("popupAction")->setEnabled(true);

            foreach (QWidget *widget, widgets) {
                widget->setEnabled(true);
            }

            fpsSpinBox->setEnabled(m_recorderManager->hasFeature("FPS", recorder));
            soundCheck->setEnabled(m_recorderManager->hasFeature("Sound", recorder));
            mouseCheck->setEnabled(m_recorderManager->hasFeature("MouseEnabled", recorder));
            keyboardCheck->setEnabled(m_recorderManager->hasFeature("KeyboardEnabled", recorder));
            timerWidget->reset();
            initRecordWidgets(false);
            initKeyMon(false);
            break;
        }
    case Timer: {
            setTrayOverlay("");
            getAction("pause")->setIcon(KIcon("media-playback-pause"));
            getAction("record")->setEnabled(false);
            getAction("pause")->setEnabled(true);
            getAction("stop")->setEnabled(true);
            getAction("recordWindow")->setEnabled(false);
            getAction("recordFullScreen")->setEnabled(false);
            getAction("box")->setEnabled(false);
            getAction("options_configure")->setEnabled(false);
            getAction("upload")->setEnabled(false);
            getAction("zoom-in")->setEnabled(true);
            getAction("zoom-out")->setEnabled(true);
            getAction("popupAction")->setEnabled(false);

            foreach (QWidget *widget, widgets) {
                widget->setEnabled(false);
            }

            initKeyMon(true);

            if (Settings::hideOnRecord()) {
                if (Settings::tray()) {
                    close();
                } else {
                    hide();
                }
            }
            break;
        }
    case Recording: {
            setTrayOverlay("media-record");
            getAction("pause")->setIcon(KIcon("media-playback-pause"));
            getAction("record")->setEnabled(false);
            getAction("pause")->setEnabled(m_recorderManager->hasFeature("Pause", recorder));
            getAction("stop")->setEnabled(m_recorderManager->hasFeature("Stop", recorder));
            getAction("recordWindow")->setEnabled(false);
            getAction("recordFullScreen")->setEnabled(false);
            getAction("box")->setEnabled(false);
            getAction("options_configure")->setEnabled(false);
            getAction("upload")->setEnabled(false);
            getAction("zoom-in")->setEnabled(true);
            getAction("zoom-out")->setEnabled(true);
            getAction("popupAction")->setEnabled(false);

            foreach (QWidget *widget, widgets) {
                widget->setEnabled(false);
            }

            initRecordWidgets(true);
            break;
        }
    case TimerPaused: {
            setTrayOverlay("media-playback-pause");
            getAction("pause")->setText(i18n("Continue"));
            getAction("pause")->setIcon(KIcon("media-playback-start"));
            getAction("record")->setEnabled(false);
            getAction("pause")->setEnabled(true);
            getAction("stop")->setEnabled(true);
            getAction("recordWindow")->setEnabled(false);
            getAction("recordFullScreen")->setEnabled(false);
            getAction("box")->setEnabled(false);
            getAction("options_configure")->setEnabled(false);
            getAction("upload")->setEnabled(false);
            getAction("zoom-in")->setEnabled(true);
            getAction("zoom-out")->setEnabled(true);
            getAction("popupAction")->setEnabled(false);

            foreach (QWidget *widget, widgets) {
                widget->setEnabled(false);
            }

            break;
        }
    case Paused: {
            setTrayOverlay("media-playback-pause");
            getAction("pause")->setText(i18n("Continue"));
            getAction("pause")->setIcon(KIcon("media-playback-start"));
            getAction("record")->setEnabled(false);
            getAction("pause")->setEnabled(m_recorderManager->hasFeature("Pause", recorder));
            getAction("stop")->setEnabled(m_recorderManager->hasFeature("Stop", recorder));
            getAction("recordWindow")->setEnabled(false);
            getAction("recordFullScreen")->setEnabled(false);
            getAction("box")->setEnabled(false);
            getAction("options_configure")->setEnabled(false);
            getAction("upload")->setEnabled(false);
            getAction("zoom-in")->setEnabled(true);
            getAction("zoom-out")->setEnabled(true);
            getAction("popupAction")->setEnabled(false);

            foreach (QWidget *widget, widgets) {
                widget->setEnabled(false);
            }

            break;
        }
    case Encode: {
            setTrayOverlay("system-run");
            getAction("pause")->setIcon(KIcon("media-playback-pause"));
            getAction("record")->setEnabled(false);
            getAction("pause")->setEnabled(true);
            getAction("stop")->setEnabled(true);
            getAction("recordWindow")->setEnabled(false);
            getAction("recordFullScreen")->setEnabled(false);
            getAction("box")->setEnabled(false);
            getAction("options_configure")->setEnabled(false);
            getAction("upload")->setEnabled(false);
            getAction("zoom-in")->setEnabled(true);
            getAction("zoom-out")->setEnabled(true);
            getAction("popupAction")->setEnabled(false);

            foreach (QWidget *widget, widgets) {
                widget->setEnabled(false);
            }

            initRecordWidgets(false);
            initKeyMon(false);
            break;
        }
        case Config: {
            setTrayOverlay("configure");
            getAction("pause")->setIcon(KIcon("media-playback-pause"));
            getAction("record")->setEnabled(false);
            getAction("pause")->setEnabled(false);
            getAction("stop")->setEnabled(false);
            getAction("recordWindow")->setEnabled(false);
            getAction("recordFullScreen")->setEnabled(false);
            getAction("box")->setEnabled(false);
            getAction("options_configure")->setEnabled(false);
            getAction("upload")->setEnabled(false);
            getAction("zoom-in")->setEnabled(false);
            getAction("zoom-out")->setEnabled(false);
            getAction("popupAction")->setEnabled(false);

            foreach (QWidget *widget, widgets) {
                widget->setEnabled(false);
            }

            break;
        }
    }

    emit recordStateChanged(m_state, oldState);

}


MainWindow::State MainWindow::state() const
{

    return m_state;

}


void MainWindow::pluginStatus(const QString &text)
{

    m_statusLabel->setText(text);

}


void MainWindow::recorderFinished(const QString &error, const bool &isVideo)
{

    if (!error.isEmpty()) {
        pluginStatus(error);
        setState(Idle);
    } else if (Settings::encoderName().isEmpty() || !Settings::encode() || !isVideo) {
        setState(Idle);
        pluginStatus(i18nc("Recording finished", "Finished!"));
        if (!isVisible()) {
            show();
        }
        raise();
        
        if (Settings::showVideo()) {
            playRequested();
        }
    } else {
        AbstractEncoder::Data d;
        initEncoder(&d);
        setState(Encode);
        m_encoderManager->startEncode(Settings::encoderName() ,d);
    }

}


void MainWindow::encoderFinished(const QString &error)
{

    if (!error.isEmpty()) {
        pluginStatus(error);
    } else {
        if (Settings::showVideo()) {
            playRequested();
        }
        pluginStatus(i18nc("Encoding finished", "Finished!"));
    }
    if (!isVisible()) {
        show();
    }
    raise();
    setState(Idle);

}


void MainWindow::configure()
{

    setState(Config);

    if(ConfigDialog::showDialog("settings")) {
        return;
    }

    ConfigDialog *dialog = new ConfigDialog(this, actionCollection());
    connect(dialog, SIGNAL(dialogFinished()), this, SLOT(configDialogFinished()));
    connect(dialog, SIGNAL(settingsSaved()), this, SLOT(applyConfig()));
    dialog->show();

}


void MainWindow::configDialogFinished()
{

    setState(Idle);

}


void MainWindow::applyConfig()
{

    setupDocks();
    setupTray();
    updateRecorderCombo();
    triggerFrame(false); // update KAction

    // update frame menu
    frameSizesChanged(FrameConfig::readSizes(Settings::self()->config()));

    // update window flags
    updateWindowFlags();

    reloadPopAction();

    // reload scripts
#ifdef HAVE_QTSCRIPT
    RecordItNow::Helper::self()->scriptManager()->reloadScripts();
#endif

}


void MainWindow::updateRecorderCombo()
{
    
    const QString oldBackend = backendCombo->currentText();
    backendCombo->clear();

    foreach (const RecordItNow::RecorderData &data, m_recorderManager->getRecorder()) {
        backendCombo->addItem(data.second, data.first);
    }

    if (backendCombo->contains(oldBackend)) {
        backendCombo->setCurrentItem(oldBackend, false);
    }

}


void MainWindow::updateWindowFlags()
{

    const QPoint p = pos();
    bool apply = false;
    if (Settings::stayOnTop()) {
        setWindowFlags(windowFlags()|Qt::WindowStaysOnTopHint);
        apply = true;
    } else if (windowFlags() & Qt::WindowStaysOnTopHint) {
        setWindowFlags(windowFlags()&~Qt::WindowStaysOnTopHint);
        apply = true;
    }

    if (apply) {
        show(); // necessary to apply window flags
        move(p);
    }

}


void MainWindow::reloadPopAction()
{

    KAction *action = static_cast<KAction*>(getAction("popupAction"));
    action->setMenu(0);

    QMenu *menu = createPopupMenu();
    if (!menu) {
        return;
    }

    action->setMenu(menu);

}


void MainWindow::recorderStateChanged(const RecordItNow::AbstractRecorder::State &newState)
{

    switch (newState) {
    case AbstractRecorder::Idle: setTrayOverlay(""); break;
    case AbstractRecorder::Record: setTrayOverlay("media-record"); break;
    case AbstractRecorder::Encode: setState(Encode); break;
    }

}


void MainWindow::startTimer()
{

    setState(Timer);
    if (state() == Timer) {
        timerWidget->start();
    }
        
}


void MainWindow::trayActivated(const bool &active, const QPoint &pos)
{

    Q_UNUSED(pos);
    if (active && (state() == Recording || state() == Paused) &&
        m_recorderManager->currentState() == AbstractRecorder::Record) {
        stopRecord();
    }

}


void MainWindow::backendChanged(const QString &newBackend)
{

    if (state() != Idle) {
        return;
    }

    //outputRequester->clear(); // call textChanged()
    outputWidget->setOutputFile(QString());

    QString file = Settings::videoDir().path();
    if (!file.endsWith(QDir::separator())) {
        file.append(QDir::separator());
    }
    file.append(m_recorderManager->getDefaultFile(newBackend));
    outputWidget->setOutputFile(file);
    setState(state()); // update actions/widgets

}


void MainWindow::aboutToQuit()
{

    KeyMonManager::self()->stop();
    deleteLater(); // bug???

}


void MainWindow::pluginsChanged()
{

    updateRecorderCombo();

}


void MainWindow::upload()
{

    QString file;
    if (outputWidget->exists()) {
        file = outputWidget->outputFile();
    } else {
        KFileDialog dialog(KGlobalSettings::videosPath(), QString::null, this);
        dialog.setMode(KFile::ExistingOnly|KFile::Files|KFile::LocalOnly);
        dialog.setOperationMode(KFileDialog::Opening);

        const int ret = dialog.exec();
        if (ret == KFileDialog::Accepted) {
            file = dialog.selectedFile();
        }
    }

    if (!file.isEmpty()) {
        UploadWizard *wizard = new UploadWizard(file, this);
        wizard->show();
    }

}


void MainWindow::linkActivated(const QString &link)
{

    KRun::runUrl(KUrl(link), "text/html", this);

}


void MainWindow::hideEvent(QHideEvent *event)
{

    KXmlGuiWindow::hideEvent(event);
    if (m_frame->isVisible()) {
        triggerFrame(false);
        getAction("box")->setChecked(true);
    }

}


void MainWindow::showEvent(QShowEvent *event)
{

    KXmlGuiWindow::showEvent(event);
    if (getAction("box")->isChecked()) {
        triggerFrame(true);
    }

}


void MainWindow::closeEvent(QCloseEvent *event)
{

    if (m_tray) {
        hide();
        event->ignore();
        return;
    }
    event->accept();
    kapp->quit();

}


void MainWindow::saveNewToolbarConfig()
{

    KXmlGuiWindow::saveNewToolbarConfig();
    menuBar()->clear();

}


void MainWindow::zoomIn()
{

    m_recorderManager->zoom(true);
    if (isDockEnabled(m_zoomDock)) {
        m_zoomDock->zoomIn();
    }

}


void MainWindow::zoomOut()
{

    m_recorderManager->zoom(false);
    if (isDockEnabled(m_zoomDock)) {
        m_zoomDock->zoomOut();
    }

}


void MainWindow::resetZoomFactor()
{

    m_recorderManager->resetZoom();
    if (isDockEnabled(m_zoomDock)) {
        m_zoomDock->resetZoomFactor();
    }

}


void MainWindow::setupDocks()
{

    // tabifyDockWidget:
    //     The window size would go crazy if a small size is restored


    // Timeline
    if (!m_timelineDock) {
        m_timelineDock = new RecordItNow::TimelineDock(this);
        connect(m_timelineDock->timeline(), SIGNAL(finished()), this, SLOT(timeLineFinsihed()));

        addDockWidget(Qt::BottomDockWidgetArea, m_timelineDock);
        tabifyDockWidget(m_mainDock, m_timelineDock);
    }
    m_timelineDock->timeline()->reload();

    // Zoom
    if (!m_zoomDock) {
        m_zoomDock = new RecordItNow::ZoomDock(this);
        addDockWidget(Qt::BottomDockWidgetArea, m_zoomDock);
        tabifyDockWidget(m_mainDock, m_zoomDock);
    }
    m_zoomDock->setFactor(Settings::zoomFactor());
    m_zoomDock->setFPS(Settings::zoomFps());
    if (Settings::zoomHighQuality()) {
        m_zoomDock->setQuality(RecordItNow::ZoomView::High);
    } else {
        m_zoomDock->setQuality(RecordItNow::ZoomView::Low);
    }

    // Player
    if (!m_playerDock) {
        m_playerDock = new RecordItNow::PlayerDock(this);
        addDockWidget(Qt::BottomDockWidgetArea, m_playerDock);
        tabifyDockWidget(m_mainDock, m_playerDock);
    }

}


void MainWindow::timeLineFinsihed()
{

    if (Settings::timeLineStop() && state() == Recording) {
        stopRecord();
    }

}


void MainWindow::resolutionActionTriggered()
{

    KAction *action = static_cast<KAction*>(sender());
    m_frame->setFrameSize(action->data().value<FrameSize>());

}


void MainWindow::frameSizesChanged(const QList<FrameSize> &sizes)
{

    KActionMenu *boxAction = static_cast<KActionMenu*>(getAction("box"));
    foreach (QAction *act, boxAction->menu()->actions()) {
        if (act->data().isNull()) {
            continue;
        }
        boxAction->removeAction(act);
    }

    for (int i = 0; i < sizes.size(); i++) {
        const FrameSize size = sizes.at(i);

        KAction *frameResAction = new KAction(this);
        frameResAction->setData(QVariant::fromValue(size));
        frameResAction->setText(size.text());
        frameResAction->setProperty("CleanText", size.text());

        if (i < 10) {
            Qt::Key key;

            switch (i) {
            case 0: key = Qt::Key_0; break;
            case 1: key = Qt::Key_1; break;
            case 2: key = Qt::Key_2; break;
            case 3: key = Qt::Key_3; break;
            case 4: key = Qt::Key_4; break;
            case 5: key = Qt::Key_5; break;
            case 6: key = Qt::Key_6; break;
            case 7: key = Qt::Key_7; break;
            case 8: key = Qt::Key_8; break;
            default: key = Qt::Key_9; break;
            }

            frameResAction->setShortcut(Qt::CTRL+key);
        }
        connect(frameResAction, SIGNAL(triggered()), this, SLOT(resolutionActionTriggered()));

        boxAction->addAction(frameResAction);
    }

}


void MainWindow::initRecordWidgets(const bool &start)
{

    const QString recorder = backendCombo->currentText();
    // mouse
    if (start) {
        if (mouseCheck->isChecked() && m_recorderManager->hasFeature("MouseEnabled", recorder)) {
            if (m_cursor) {
                return; // timer was paused
            }
            m_cursor = new RecordItNow::CursorWidget(this);
            connect(m_cursor, SIGNAL(error(QString)), this, SLOT(errorNotification(QString)));

            m_cursor->setButtons(MouseConfig::getButtons(Settings::self()->config()));
            m_cursor->setSize(QSize(Settings::cursorWidgetSize(), Settings::cursorWidgetSize()));
            m_cursor->setDevice(Settings::mouseDevice());
            m_cursor->setMode((RecordItNow::CursorWidget::WidgetMode)Settings::cursorMode());
            m_cursor->setOpacity(Settings::cursorOpacity());
            m_cursor->setShowAlways(Settings::mouseWidgetAlwaysVisible());

            m_cursor->start();
        }
    } else {
        if (m_cursor) {
            m_cursor->stop();
            m_cursor = 0;
        }
    }

    // timeline
    if (isDockEnabled(m_timelineDock)) {
        if (start) {
            if (m_recorderManager->hasFeature("TimelineEnabled", recorder)) {
                m_timelineDock->timeline()->start();
            }
        } else {
            m_timelineDock->timeline()->stop();
        }
    }

    // Keyboard
    if (start) {
        if (keyboardCheck->isChecked() && m_recorderManager->hasFeature("KeyboardEnabled", recorder)) {
            if (!m_keyloggerOSD) {
                m_keyloggerOSD = new RecordItNow::KeyloggerOSD(this);
                m_keyloggerOSD->loadGeometry(KeyboardConfig::keyloggerGeometry(Settings::self()->config()));
            }
            m_keyloggerOSD->init(Settings::keyboardOnScreenDisplayHideTime(),
                                 Settings::keyboardOnScreenDisplayShortcuts(),
                                 Settings::keyboardOnScreenDisplayClipboard());
        }
    } else {
        if (m_keyloggerOSD) {
            delete m_keyloggerOSD;
            m_keyloggerOSD = 0;
        }
    }

}


void MainWindow::initKeyMon(const bool &start)
{

    const bool mouseFeature = m_recorderManager->hasFeature("MouseEnabled", backendCombo->currentText());
    const bool keyboardFeature = m_recorderManager->hasFeature("KeyboardEnabled", backendCombo->currentText());

    if (start && (mouseFeature || keyboardFeature)) {
        if (!KeyMonManager::self()->start() && !KeyMonManager::self()->error().isEmpty()) {
            errorNotification(KeyMonManager::self()->error());
        } else {
            if (!KeyMonManager::self()->waitForStarted()) {
                if (state() == Recording) {
                    errorNotification(KeyMonManager::self()->error());
                }
            }
        }
    } else if (!start) {
        KeyMonManager::self()->stop();
    }

}


void MainWindow::keyMonStopped()
{

    if (KeyMonManager::self()->error().isEmpty() || state() == Idle) {
        kDebug() << state();
        return;
    }

    errorNotification(KeyMonManager::self()->error());

}


void MainWindow::errorNotification(const QString &error)
{

    KNotification *notification = new KNotification("error", this, KNotification::CloseOnTimeout);
    notification->setText(error);
    notification->setPixmap(KIcon("dialog-error").pixmap(KIconLoader::SizeMedium, KIconLoader::SizeMedium));
    notification->sendEvent();

}


void MainWindow::playRequested()
{

    if (!(isDockEnabled(m_playerDock) && m_playerDock->play(outputWidget->outputFile()))) {
        outputWidget->playOutputFile();
    } else {
        m_playerDock->raise();
    }

}


void MainWindow::lockLayout(const bool &lock)
{

    QFlags<QDockWidget::DockWidgetFeature> features;
    QFlags<QDockWidget::DockWidgetFeature> mainFeatures;
    if (lock) {
        features = QDockWidget::DockWidgetFloatable;
        mainFeatures = features;
    } else {
        features = QDockWidget::DockWidgetClosable|
                   QDockWidget::DockWidgetFloatable|
                   QDockWidget::DockWidgetMovable;
        mainFeatures = features &~ QDockWidget::DockWidgetClosable;
    }

    m_mainDock->setFeatures(mainFeatures);
    m_timelineDock->setFeatures(features);
    m_zoomDock->setFeatures(features);
    m_playerDock->setFeatures(features);

    KAction *action = getAction("lockLayout");
    if (action->isChecked() != lock) {
        action->setChecked(lock);
    }

}


} // namespace RecordItNow


#include "mainwindow.moc"

