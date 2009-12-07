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
#include <kdeversion.h> // krazy:exclude=includes
#if (KDE_VERSION >= KDE_MAKE_VERSION(4,3,64))
    #include "mainwindow.h"
#else
    #include "mainwindow_4_3.h" // moc workaround
#endif
#include "framebox.h"
#include <recorditnow.h>
#include "recorditnowpluginmanager.h"
#include "configdialog.h"
#include "libs/upload/abstractuploader.h"
#include "recordermanager.h"
#include "encodermanager.h"
#include "uploadmanager.h"
#include "cursorwidget.h"
#include "application.h"
#include "mouseconfig.h"
#include "zoomview.h"
#include "timeline.h"

// Qt
#include <QtGui/QX11Info>
#include <QtGui/QMouseEvent>
#include <QtGui/QDesktopWidget>
#include <QtCore/QTimer>
#include <QtGui/QPainter>
#include <QtGui/QStackedLayout>
#include <QtGui/QDockWidget>

// KDE
#include <kicon.h>
#include <kactioncollection.h>
#include <klocalizedstring.h>
#include <kdebug.h>
#include <kstatusbar.h>
#include <kmenubar.h>
#include <kmessagebox.h>
#include <kservicetypetrader.h>
#include <kmenu.h>
#include <krun.h>
#include <kmimetype.h>
#include <kapplication.h>
#include <kactionmenu.h>
#include <ksqueezedtextlabel.h>
#include <kactioncategory.h>
#include <knotification.h>

// X11
#include <X11/Xlib.h>



MainWindow::MainWindow(QWidget *parent)
    : KXmlGuiWindow(parent),
    m_grabber(0)
{

    setupActions();

    QWidget *toolWidget = new QWidget(this);
    setupUi(toolWidget);
    setCentralWidget(toolWidget);

    timeUpButton->setIcon(KIcon("arrow-up"));
    timeDownButton->setIcon(KIcon("arrow-down"));
    soundCheck->setIcon(KIcon("preferences-desktop-sound"));
    deleteButton->setIcon(KIcon("list-remove"));
    playButton->setIcon(KIcon("media-playback-start"));

    connect(timeUpButton, SIGNAL(clicked()), this, SLOT(lcdUp()));
    connect(timeDownButton, SIGNAL(clicked()), this, SLOT(lcdDown()));
    connect(backendCombo, SIGNAL(currentIndexChanged(QString)), this,
            SLOT(backendChanged(QString)));
    connect(kapp, SIGNAL(aboutToQuit()), this, SLOT(aboutToQuit()));
    connect(playButton, SIGNAL(clicked()), this, SLOT(playFile()));
    connect(deleteButton, SIGNAL(clicked()), this, SLOT(removeFile()));

    connect(outputRequester, SIGNAL(textChanged(QString)), this, SLOT(outputFileChanged(QString)));

    m_box = new FrameBox(this, Settings::currentFrame());

    m_tray = 0;
    m_zoom = 0;
    m_timeLine = 0;
    m_timelineDock = 0;

    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(tick()));


    m_pluginManager = new RecordItNowPluginManager(this);
    connect(m_pluginManager, SIGNAL(pluginsChanged()), this, SLOT(pluginsChanged()));

    m_recorderManager = new RecorderManager(this, m_pluginManager);
    connect(m_recorderManager, SIGNAL(status(QString)), this, SLOT(pluginStatus(QString)));
    connect(m_recorderManager, SIGNAL(fileChanged(QString)), outputRequester, SLOT(setText(QString)));
    connect(m_recorderManager, SIGNAL(finished(QString,bool)), this,
            SLOT(recorderFinished(QString,bool)));
    connect(m_recorderManager, SIGNAL(stateChanged(AbstractRecorder::State)), this,
            SLOT(recorderStateChanged(AbstractRecorder::State)));

    m_encoderManager = new EncoderManager(this, m_pluginManager);
    connect(m_encoderManager, SIGNAL(status(QString)), this, SLOT(pluginStatus(QString)));
    connect(m_encoderManager, SIGNAL(fileChanged(QString)), outputRequester, SLOT(setText(QString)));
    connect(m_encoderManager, SIGNAL(finished(QString)), this,
            SLOT(encoderFinished(QString)));

    m_uploadManager = new UploadManager(this, m_pluginManager);
    connect(m_uploadManager, SIGNAL(finished(QString)), this, SLOT(uploaderFinished(QString)));
    connect(m_uploadManager, SIGNAL(status(QString)), this, SLOT(pluginStatus(QString)));


    m_statusLabel = new KSqueezedTextLabel(this);
    connect(m_statusLabel, SIGNAL(linkActivated(QString)), this, SLOT(linkActivated(QString)));
    statusBar()->addPermanentWidget(m_statusLabel, 1);

    setupTimeLine();
    setupTray();
    setupGUI();

    //setMenuBar(0); // crash on saveNewToolbarConfig()... KDE bug?
    menuBar()->clear(); // see saveNewToolbarConfig()

    setState(Idle);
    m_pluginManager->init();

    backendCombo->setCurrentItem(Settings::currentBackend(), false);
    soundCheck->setChecked(Settings::soundEnabled());
    fpsSpinBox->setValue(Settings::currentFps());
    timerLcd->display(Settings::currentTime());
    m_timer->setProperty("Time", timerLcd->value());

}


MainWindow::~MainWindow()
{

    Settings::self()->setCurrentBackend(backendCombo->currentText());
    Settings::self()->setSoundEnabled(soundCheck->isChecked());
    Settings::self()->setCurrentFps(fpsSpinBox->value());
    Settings::self()->setCurrentFrame(m_box->boxGeometry());
    Settings::self()->setCurrentTime(timerLcd->value());
    Settings::self()->writeConfig();

    if (m_timeLine) {
        KConfigGroup cfg(Settings::self()->config(), "Timeline");
        m_timeLine->saveTopics(&cfg);
    }

    if (m_grabber) {
        delete m_grabber;
    }

    delete m_box;

    if (m_tray) {
        delete m_tray;
    }
    delete m_timer;
    delete m_recorderManager;
    delete m_encoderManager;
    delete m_uploadManager;
    delete m_pluginManager;

    if (m_cursor) {
        delete m_cursor;
    }

    if (m_zoom) {
        delete m_zoom;
    }

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
        outputRequester->setText(file);
    }

    initRecorder(&m_recordData);
    m_recordData.geometry = geometry;

    timerLcd->display(time);

    startTimer();

}


KAction *MainWindow::getAction(const QString &name)
{

    KAction *action = static_cast<KAction*>(actionCollection()->action(name));
    if (!action) {
        if (name == "upload") {
            action = new KActionMenu(this);
            static_cast<KActionMenu*>(action)->setDelayed(false);
        } else {
            action = new KAction(this);
        }
        action->setShortcutConfigurable(true);
        actionCollection()->addAction(name, action);
    }
    return action;

}


void MainWindow::setupActions()
{

    KAction *recordAction = getAction("record");
    recordAction->setText(i18n("Record"));
    recordAction->setIcon(KIcon("media-record"));
    recordAction->setShortcut(Qt::CTRL+Qt::Key_R, KAction::DefaultShortcut);
    connect(recordAction, SIGNAL(triggered()), this, SLOT(recordTriggred()));

    
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

    KAction *boxAction = getAction("box");
    boxAction->setText(i18n("Show Frame"));
    boxAction->setIcon(KIcon("draw-rectangle"));
    boxAction->setShortcut(Qt::CTRL+Qt::Key_F, KAction::DefaultShortcut);
    boxAction->setCheckable(true);
    connect(boxAction, SIGNAL(triggered(bool)), this, SLOT(triggerFrame(bool)));

    KAction *fullAction = getAction("recordFullScreen");
    fullAction->setText(i18n("Record the entire Screen"));
    fullAction->setIcon(KIcon("view-fullscreen"));
    fullAction->setShortcut(Qt::CTRL+Qt::Key_A, KAction::DefaultShortcut);
    connect(fullAction, SIGNAL(triggered()), this, SLOT(recordFullScreen()));


    KAction *uploadAction = getAction("upload");
    uploadAction->setIcon(KIcon("recorditnow-upload-media"));
    uploadAction->setText(i18n("Upload"));
    uploadAction->setShortcut(Qt::CTRL+Qt::Key_U, KAction::DefaultShortcut);


    KAction *zoomAction = getAction("zoom");
    zoomAction->setObjectName("RecordItNow_zoom");
    zoomAction->setIcon(KIcon("page-zoom"));
    zoomAction->setText(i18n("Zoom"));
    zoomAction->setGlobalShortcut(KShortcut(Qt::META+Qt::CTRL+Qt::Key_Z));
    connect(zoomAction, SIGNAL(triggered()), this, SLOT(triggerZoom()));

    KAction *zoomInAction = getAction("zoom-in");
    zoomInAction->setObjectName("RecordItNow_zoom_in");
    zoomInAction->setIcon(KIcon("zoom-in"));
    zoomInAction->setText(i18n("Zoom in"));
    zoomInAction->setGlobalShortcut(KShortcut(Qt::META+Qt::CTRL+Qt::Key_Plus));
    connect(zoomInAction, SIGNAL(triggered()), this, SLOT(zoomIn()));

    KAction *zoomOutAction = getAction("zoom-out");
    zoomOutAction->setObjectName("RecordItNow_zoom_out");
    zoomOutAction->setIcon(KIcon("zoom-out"));
    zoomOutAction->setText(i18n("Zoom out"));
    zoomOutAction->setGlobalShortcut(KShortcut(Qt::META+Qt::CTRL+Qt::Key_Minus));
    connect(zoomOutAction, SIGNAL(triggered()), this, SLOT(zoomOut()));


    KStandardAction::preferences(this, SLOT(configure()), actionCollection());

}


void MainWindow::startRecord()
{

    setState(Recording);   

    if (Settings::showActivity()) {
        m_cursor = static_cast<Application*>(kapp)->getCursorWidget(this);
        connect(m_cursor, SIGNAL(error(QString)), this, SLOT(cursorError(QString)));

        m_cursor->setButtons(MouseConfig::getButtons());
        m_cursor->setSize(QSize(Settings::cursorWidgetSize(), Settings::cursorWidgetSize()));
        m_cursor->setNormalColor(Settings::defaultColor());
        m_cursor->setUseSNoop(Settings::useSNoop(), Settings::snoopDevice());

        m_cursor->start();
    }
    m_recorderManager->startRecord(backendCombo->currentText(), m_recordData);

    if (m_timeLine && m_recorderManager->hasFeature("TimelineEnabled", backendCombo->currentText())) {
        m_timeLine->start();
    }

}


void MainWindow::pauseRecord()
{

    if (state() == Timer) {
        m_timer->stop();
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
        if (m_timeLine) {
            m_timeLine->pauseOrContinue();
        }
    }

}


void MainWindow::stopRecord()
{

    if (state() == Timer || state() == TimerPaused) {
        m_timer->stop();
        setState(Idle);
    } else {
        m_recorderManager->stop();
        m_encoderManager->stop();
        m_uploadManager->stop();
        if (m_timeLine) {
            m_timeLine->stop();
        }
    }

}


void MainWindow::recordTriggred()
{

    if (!m_box->isEnabled()) {
        KMessageBox::sorry(this, i18n("No screen area selected."));
        return;
    }

    initRecorder(&m_recordData);
    m_recordData.geometry = m_box->boxGeometry();

    startTimer();
    
}


void MainWindow::recordWindow()
{

    m_grabber = new QWidget(0, Qt::X11BypassWindowManagerHint);
    m_grabber->move(-1000, -1000);
    m_grabber->installEventFilter(this);
    m_grabber->show();
    m_grabber->grabMouse(Qt::CrossCursor);

}


void MainWindow::recordCurrentWindow()
{

    Window root;
    Window child;
    uint mask;
    int rootX, rootY, winX, winY;

    XGrabServer(QX11Info::display());
    XQueryPointer(QX11Info::display(), QX11Info::appRootWindow(), &root, &child,
                  &rootX, &rootY, &winX, &winY, &mask);

    if (child == None) {
        child = QX11Info::appRootWindow();
    }

    XUngrabServer(QX11Info::display());

    m_grabber->deleteLater();
    m_grabber = 0;

    if (child == None) {
        return;
    }

    kDebug() << "WinId:" << child;

    initRecorder(&m_recordData);
    m_recordData.winId = child;
    startTimer();

}


void MainWindow::triggerFrame(const bool &checked)
{

    if (!isVisible() && !checked) {
        m_box->setEnabled(false);
    } else if (!isVisible() && checked) {
        m_box->setEnabled(false);
    } else {
        m_box->setEnabled(!m_box->isEnabled());
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
        m_recordData.geometry = kapp->desktop()->screenGeometry(screen);
    }

    startTimer();

}


bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{

    if (m_grabber
        && watched == m_grabber
        && QWidget::mouseGrabber() == m_grabber
        && event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        m_grabber->releaseMouse();
        m_grabber->hide();
        if (mouseEvent->button() == Qt::LeftButton) {
            recordCurrentWindow();
        } else {
            m_grabber->deleteLater();
            m_grabber = 0;
            setState(Idle);
        }
    }
    return KXmlGuiWindow::eventFilter(watched, event);

}


void MainWindow::initRecorder(AbstractRecorder::Data *d)
{

    if (m_box->isEnabled()) {
        m_box->setEnabled(false);
    }

    d->geometry = QRect(-1, -1, -1, -1);
    d->winId = -1;

    d->fps = fpsSpinBox->value();
    d->sound = soundCheck->isChecked();
    d->outputFile = outputRequester->text();
    d->overwrite = Settings::overwrite();
    d->workDir = Settings::workDir().path();

    pluginStatus("");

    if (Settings::hideOnRecord()) {
        if (Settings::tray()) {
            close();
        } else {
            hide();
        }
    }

}


void MainWindow::initEncoder(AbstractEncoder::Data *d)
{

    d->overwrite = Settings::overwrite();
    d->file = outputRequester->text();
    d->workDir = Settings::workDir().path();

}


void MainWindow::setupTray()
{

    if (Settings::tray()) {
        if (!m_tray) {
#if (KDE_VERSION >= KDE_MAKE_VERSION(4,3,64))
            m_tray = new KStatusNotifierItem(this);
            m_tray->setStatus(KStatusNotifierItem::Active);
            m_tray->setCategory(KStatusNotifierItem::ApplicationStatus);
            m_tray->setIconByName("recorditnow");
            m_tray->setToolTip("recorditnow", i18n("RecordItNow"), "");
            connect(m_tray, SIGNAL(activateRequested(bool,QPoint)), this,
                    SLOT(trayActivated(bool,QPoint)));
#else
            m_tray = new KSystemTrayIcon(this);
            m_tray->setIcon(KIcon("recorditnow"));
            connect(m_tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this,
            SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));
            m_tray->show();
#endif


            KMenu *context = new KMenu(this);
            context->addAction(getAction("record"));
            context->addAction(getAction("pause"));
            context->addAction(getAction("stop"));
            context->addSeparator();
            context->addAction(getAction("box"));
            context->addAction(getAction("recordWindow"));
            context->addAction(getAction("recordFullScreen"));
#if (KDE_VERSION >= KDE_MAKE_VERSION(4,3,64))
#else
            context->addSeparator();
            context->addAction((QAction*)KStandardAction::quit(kapp, SLOT(quit()), actionCollection()));
#endif
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
#if (KDE_VERSION >= KDE_MAKE_VERSION(4,3,64))
        m_tray->setOverlayIconByName(name);
#else
        QPixmap icon = KIcon("recorditnow").pixmap(KIconLoader::SizeSmallMedium,
                                                     KIconLoader::SizeSmallMedium);
        if (!name.isEmpty()) {
            QPixmap overlay = KIcon(name).pixmap(KIconLoader::SizeSmallMedium/2,
                                                 KIconLoader::SizeSmallMedium/2);
            QPainter p(&icon);
            p.drawPixmap(icon.width()-overlay.width(), icon.height()-overlay.height(), overlay);
            p.end();
        }
        m_tray->setIcon(icon);
#endif
    }

}


void MainWindow::setState(const State &newState)
{

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
            getAction("box")->setChecked(m_box->isEnabled());
            getAction("options_configure")->setEnabled(true);
            getAction("upload")->setEnabled(true);
            fpsSpinBox->setEnabled(m_recorderManager->hasFeature("FPS", recorder));
            soundCheck->setEnabled(m_recorderManager->hasFeature("Sound", recorder));
            centralWidget()->setEnabled(true);
            timerLcd->display(m_timer->property("Time").toInt());
            if (Settings::stayOnTop()) {
                const QPoint p = pos();
                setWindowFlags(windowFlags()&~Qt::WindowStaysOnTopHint);
                show(); // necessary to apply window flags
                move(p);
            }
            break;
        }
    case Timer: {
            setTrayOverlay("recorditnow-timeline");
            getAction("pause")->setIcon(KIcon("media-playback-pause"));
            getAction("record")->setEnabled(false);
            getAction("pause")->setEnabled(true);
            getAction("stop")->setEnabled(true);
            getAction("recordWindow")->setEnabled(false);
            getAction("recordFullScreen")->setEnabled(false);
            getAction("box")->setEnabled(false);
            getAction("options_configure")->setEnabled(false);
            getAction("upload")->setEnabled(false);
            centralWidget()->setEnabled(false);
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
            centralWidget()->setEnabled(false);
            timerLcd->display(0);
            if (Settings::stayOnTop()) {
                getAction("box")->setChecked(false);
                const QPoint p = pos();
                setWindowFlags(windowFlags()|Qt::WindowStaysOnTopHint);
                show(); // necessary to apply window flags
                move(p);
            }
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
            centralWidget()->setEnabled(false);
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
            centralWidget()->setEnabled(false);
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
            centralWidget()->setEnabled(false);
            break;
        }
    case Upload: {
            setTrayOverlay("arrow-up-double");
            getAction("pause")->setIcon(KIcon("media-playback-pause"));
            getAction("record")->setEnabled(false);
            getAction("pause")->setEnabled(false);
            getAction("stop")->setEnabled(true);
            getAction("recordWindow")->setEnabled(false);
            getAction("recordFullScreen")->setEnabled(false);
            getAction("box")->setEnabled(false);
            getAction("options_configure")->setEnabled(false);
            getAction("upload")->setEnabled(false);
            centralWidget()->setEnabled(false);
            break;
        }
    }
    m_state = newState;

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

    if (m_cursor) {
        m_cursor->stop();
        m_cursor = 0;
    }

    if (m_zoom) {
        triggerZoom();
    }

    if (m_timeLine) {
        m_timeLine->stop();
    }

    if (!error.isEmpty()) {
        KMessageBox::error(this, error);
        pluginStatus(error);
        setState(Idle);
    } else if (Settings::encoderName().isEmpty() || !Settings::encode() || !isVideo) {
        setState(Idle);
        playFile(false);
        pluginStatus(i18nc("Recording finished", "Finished!"));
    } else {
        AbstractEncoder::Data d;
        initEncoder(&d);
        setState(Encode);
        m_encoderManager->startEncode(Settings::encoderName() ,d);
    }
    outputFileChanged(outputRequester->text());

}


void MainWindow::encoderFinished(const QString &error)
{

    if (!error.isEmpty()) {
        KMessageBox::error(this, error);
        pluginStatus(error);
    } else {
        playFile(false);
        pluginStatus(i18nc("Encoding finished", "Finished!"));
    }
    setState(Idle);
    outputFileChanged(outputRequester->text());

}


void MainWindow::configure()
{

    ConfigDialog *dialog = new ConfigDialog(this, actionCollection(), m_pluginManager);
    connect(dialog, SIGNAL(dialogFinished()), this, SLOT(dialogFinished()));
    dialog->show();

}


void MainWindow::dialogFinished()
{

    setupTimeLine();
    setupTray();
    updateRecorderCombo();
    updateUploaderMenu();

}


void MainWindow::updateRecorderCombo()
{

    const QString oldBackend = backendCombo->currentText();
    backendCombo->clear();

    foreach (const RecorderData &data, m_recorderManager->getRecorder()) {
        backendCombo->addItem(data.second, data.first);
    }

    if (backendCombo->contains(oldBackend)) {
        backendCombo->setCurrentItem(oldBackend, false);
    }

}


void MainWindow::playFile(const bool &force)
{

    if (Settings::showVideo() || force) {
        const KUrl url(outputRequester->text());
        if (!QFile(url.path()).exists()) {
            KMessageBox::sorry(this, i18nc("%1 = file", "%1 no such file!", url.path()));
        } else {
            KMimeType::Ptr type = KMimeType::findByUrl(url);
            KRun::runUrl(url, type->name(), this);
        }
    }

}


void MainWindow::removeFile()
{

    QFile file(outputRequester->text());
    if (!file.exists()) {
        KMessageBox::sorry(this, i18nc("%1 = file", "%1 no such file!", file.fileName()));
        return;
    }

    if (!file.remove()) {
        KMessageBox::error(this, i18nc("%1 = file, %2 = error string", "Remove failed: %1.\n"
                                       "Reason: %2", file.fileName(), file.errorString()));
    } else {
        backendChanged(backendCombo->currentText()); // reset output file name
    }

}


void MainWindow::outputFileChanged(const QString &newFile)
{

    if (QFile::exists(newFile)) {
        playButton->setEnabled(true);
        deleteButton->setEnabled(true);
    } else {
        playButton->setEnabled(false);
        deleteButton->setEnabled(false);
    }

}


void MainWindow::recorderStateChanged(const AbstractRecorder::State &newState)
{

    switch (newState) {
    case AbstractRecorder::Idle: setTrayOverlay(""); break;
    case AbstractRecorder::Record: setTrayOverlay("media-record"); break;
    case AbstractRecorder::Encode: setTrayOverlay("system-run"); break;
    }

}


void MainWindow::uploaderFinished(const QString &error)
{

    if (!error.isEmpty()) {
        KMessageBox::error(this, error);
        pluginStatus(error);
    }
    setState(Idle);

}


void MainWindow::startTimer()
{

    if (state() == Idle) {
        m_timer->setProperty("Time", timerLcd->value());
    }

    setState(Timer);
    if (timerLcd->value() == 0) {
        m_timer->start(0);
    } else {
        m_timer->start(1000);
    }

}


void MainWindow::tick()
{

    lcdDown();
    if (timerLcd->value() < 1) {
        m_timer->stop();
        startRecord();
    }

}


void MainWindow::lcdUp()
{

    if (timerLcd->value() == 60) {
        return;
    }
    timerLcd->display(timerLcd->value()+1);

    if (sender() == timeUpButton) {
        m_timer->setProperty("Time", timerLcd->value());
    }

}


void MainWindow::lcdDown()
{

    if (timerLcd->value() == 0) {
        return;
    }
    timerLcd->display(timerLcd->value()-1);

    if (sender() == timeDownButton) {
        m_timer->setProperty("Time", timerLcd->value());
    }

}

#if (KDE_VERSION >= KDE_MAKE_VERSION(4,3,64))
void MainWindow::trayActivated(const bool &active, const QPoint &pos)
{

    Q_UNUSED(pos);
    if (active && (state() == Recording || state() == Paused) &&
        m_recorderManager->currentState() == AbstractRecorder::Record) {
        stopRecord();
    }

}
#else
void MainWindow::trayActivated(const QSystemTrayIcon::ActivationReason &reason)
{

    if (reason == QSystemTrayIcon::Trigger &&
        isVisible() &&
        (state() == Recording || state() == Paused) &&
        m_recorderManager->currentState() == AbstractRecorder::Record) {
        stopRecord();
    }

}
#endif

void MainWindow::backendChanged(const QString &newBackend)
{

    outputRequester->clear(); // call textChanged()
    outputRequester->setText(m_recorderManager->getDefaultFile(newBackend));
    setState(Idle); // update actions/widgets

}


void MainWindow::aboutToQuit()
{

    deleteLater(); // bug???

}


void MainWindow::pluginsChanged()
{

    if (state() != Idle) {
        return; // play save
    }
    // recorder
    updateRecorderCombo();
    // upload
    updateUploaderMenu();

}


void MainWindow::updateUploaderMenu()
{

    KActionMenu *action = static_cast<KActionMenu*>(getAction("upload"));
    KMenu *menu = action->menu();
    if (!menu) {
        menu = new KMenu(this);
        action->setMenu(menu);
    }
    menu->clear();

    foreach (const UploadData &d, m_uploadManager->getUploader()) {
        KAction *uploadAction = new KAction(this);
        uploadAction->setText(d.first);
        uploadAction->setIcon(d.second);
        uploadAction->setData(d.first);
        connect(uploadAction, SIGNAL(triggered()), this, SLOT(upload()));
        menu->addAction(uploadAction);
    }

}


void MainWindow::upload()
{

    if (state() == Upload) {
        return;
    }

    setState(Upload);
    QAction *uploadAction = static_cast<QAction*>(sender());
    m_uploadManager->startUpload(uploadAction->data().toString(), outputRequester->text(), this);

}


void MainWindow::linkActivated(const QString &link)
{

    KRun::runUrl(KUrl(link), "text/html", this);

}


void MainWindow::hideEvent(QHideEvent *event)
{

    KXmlGuiWindow::hideEvent(event);
    if (m_box->isEnabled()) {
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


void MainWindow::triggerZoom()
{

    if (m_zoom) {
        delete m_zoom;
        m_zoom = 0;
    } else {
        if (state() == Recording) {
            m_zoom = new ZoomView(this);
            m_zoom->setSize(QSize(Settings::zoomWidth(), Settings::zoomHeight()));
            m_zoom->setFactor(Settings::zoomFactor());
            m_zoom->setFollowMouse(Settings::zoomFollow());
            if (Settings::zoomHighQuality()) {
                m_zoom->setQuality(ZoomView::High);
            }
            m_zoom->show();
        }
    }

}


void MainWindow::zoomIn()
{

    if (!m_zoom) {
        triggerZoom();
    } else {
        m_zoom->setFactor(m_zoom->factor()+1);
    }

}


void MainWindow::zoomOut()
{

    if (m_zoom) {
        m_zoom->setFactor(m_zoom->factor()-1);
    }

}


void MainWindow::setupTimeLine()
{

    if (Settings::showTimeLine()) {
        if (!m_timelineDock) {
            m_timeLine = new TimeLine(this);
            connect(m_timeLine, SIGNAL(finished()), this, SLOT(timeLineFinsihed()));

            m_timelineDock = new QDockWidget(i18n("Timeline"), this);
            m_timelineDock->setWidget(m_timeLine);
            m_timelineDock->setObjectName("TimelineDockWidget");
            m_timelineDock->setAllowedAreas(Qt::AllDockWidgetAreas);
            m_timelineDock->setFeatures(QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetFloatable);
            addDockWidget(Qt::BottomDockWidgetArea, m_timelineDock);

            KConfigGroup cfg(Settings::self()->config(), "Timeline");
            m_timeLine->loadTopics(&cfg);
        }
        m_timeLine->enableNotifications(Settings::timelineNotifications());
    } else {
        if (m_timelineDock) {
            KConfigGroup cfg(Settings::self()->config(), "Timeline");
            m_timeLine->saveTopics(&cfg);

            removeDockWidget(m_timelineDock);
            delete m_timelineDock;
            m_timelineDock = 0;
            m_timeLine = 0;
        }
    }

}


void MainWindow::timeLineFinsihed()
{

    if (Settings::timeLineStop() && state() == Recording) {
        stopRecord();
    }

}


void MainWindow::cursorError(const QString &message)
{

    KNotification *notification = new KNotification("error", this, KNotification::CloseOnTimeout);
    notification->setText(message);
    notification->setPixmap(KIcon("dialog-error").pixmap(KIconLoader::SizeMedium, KIconLoader::SizeMedium));
    notification->sendEvent();

}


#if (KDE_VERSION >= KDE_MAKE_VERSION(4,3,64))
    #include "mainwindow.moc"
#else
    #include "mainwindow_4_3.moc"
#endif


