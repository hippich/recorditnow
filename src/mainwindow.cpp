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
#include <kdeversion.h>
#if (KDE_VERSION >= KDE_MAKE_VERSION(4,3,64))
    #include "mainwindow.h"
#else
    #include "mainwindow_4_3.h" // moc workaround
#endif
#include "framebox.h"
#include <recorditnow.h>
#include <recorditnowpluginmanager.h>
#include "configdialog.h"
#include "libs/upload/abstractuploader.h"

// Qt
#include <QtGui/QAction>
#include <QtGui/QX11Info>
#include <QtCore/QTimer>
#include <QtGui/QMouseEvent>
#include <QtGui/QDesktopWidget>

// KDE
#include <kicon.h>
#include <kactioncollection.h>
#include <klocalizedstring.h>
#include <kdebug.h>
#include <kstatusbar.h>
#include <kmenubar.h>
#include <kmessagebox.h>
#include <kservice.h>
#include <kservicetypetrader.h>
#include <kplugininfo.h>
#include <kconfigdialog.h>
#include <kmenu.h>
#include <krun.h>
#include <kmimetype.h>
#include <kapplication.h>
#include <kactionmenu.h>

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

    QRect pos = Settings::currentFrame();
    m_box = new FrameBox(this, pos);

    m_recorderPlugin = 0;
    m_encoderPlugin = 0;
    m_tray = 0;

    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(tick()));

    m_pluginManager = new RecordItNowPluginManager(this);
    connect(m_pluginManager, SIGNAL(pluginsChanged()), this, SLOT(pluginsChanged()));

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

}


MainWindow::~MainWindow()
{

    Settings::self()->setCurrentBackend(backendCombo->currentText());
    Settings::self()->setSoundEnabled(soundCheck->isChecked());
    Settings::self()->setCurrentFps(fpsSpinBox->value());
    Settings::self()->setCurrentFrame(m_box->boxGeometry());
    Settings::self()->setCurrentTime(timerLcd->value());
    Settings::self()->writeConfig();

    if (m_grabber) {
        delete m_grabber;
    }

    delete m_box;

    if (m_tray) {
        delete m_tray;
    }
    delete m_timer;
    delete m_pluginManager;

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


QAction *MainWindow::getAction(const QString &name)
{

    QAction *action = actionCollection()->action(name);
    if (!action) {
        action = new KActionMenu(this);
        static_cast<KActionMenu*>(action)->setDelayed(false);
        actionCollection()->addAction(name, action);
    }
    return action;

}


void MainWindow::setupActions()
{

    QAction *recordAction = getAction("record");
    recordAction->setText(i18n("Record"));
    recordAction->setIcon(KIcon("media-record"));
    connect(recordAction, SIGNAL(triggered()), this, SLOT(recordTriggred()));

    QAction *pauseAction = getAction("pause");
    pauseAction->setObjectName("pause");
    pauseAction->setText(i18n("Pause"));
    pauseAction->setIcon(KIcon("media-playback-pause"));
    pauseAction->setEnabled(false);
    connect(pauseAction, SIGNAL(triggered()), this, SLOT(pauseRecord()));

    QAction *stopAction = getAction("stop");
    stopAction->setText(i18n("Stop"));
    stopAction->setIcon(KIcon("media-playback-stop"));
    stopAction->setEnabled(false);
    connect(stopAction, SIGNAL(triggered()), this, SLOT(stopRecord()));


    QAction *recordWindowAction = getAction("recordWindow");
    recordWindowAction->setText(i18n("Record a Window"));
    recordWindowAction->setIcon(KIcon("edit-select"));
    connect(recordWindowAction, SIGNAL(triggered()), this, SLOT(recordWindow()));

    QAction *boxAction = getAction("box");
    boxAction->setText(i18n("Show Frame"));
    boxAction->setIcon(KIcon("draw-rectangle"));
    boxAction->setCheckable(true);
    connect(boxAction, SIGNAL(triggered(bool)), this, SLOT(triggerFrame(bool)));

    QAction *fullAction = getAction("recordFullScreen");
    fullAction->setText(i18n("Record the entire Screen"));
    fullAction->setIcon(KIcon("view-fullscreen"));
    connect(fullAction, SIGNAL(triggered()), this, SLOT(recordFullScreen()));


    QAction *uploadAction = getAction("upload");
    uploadAction->setIcon(KIcon("upload-media"));
    uploadAction->setText(i18n("Upload"));


    KStandardAction::preferences(this, SLOT(configure()), actionCollection());

}


void MainWindow::startRecord()
{

    if (m_recorderPlugin) {
        setState(Recording);
        m_recorderPlugin->record(m_recordData);
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
        return;
    }

    if (m_recorderPlugin) {
        m_recorderPlugin->pause();
    } else if (m_encoderPlugin) {
        m_encoderPlugin->pause();
    }

    if (state() == Recording) {
        setState(Paused);
    } else {
        setState(Recording);
    }

}


void MainWindow::stopRecord()
{

    if (state() == Timer || state() == TimerPaused) {
        m_timer->stop();
        m_pluginManager->unloadPlugin(m_recorderPlugin);
        m_recorderPlugin = 0;
        setState(Idle);
        return;
    }

    if (m_recorderPlugin) {
        m_recorderPlugin->stop();
    } else if (m_encoderPlugin) {
        m_encoderPlugin->stop();
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


static Window findRealWindow(Window w, int depth = 0)
{

    if (depth > 5) {
        return None;
    }

    static Atom wm_state = XInternAtom(QX11Info::display(), "WM_STATE", False);
    Atom type;
    int format;
    unsigned long nitems, after;
    unsigned char* prop;

    if (XGetWindowProperty(QX11Info::display(), w, wm_state, 0, 0, False, AnyPropertyType,
                           &type, &format, &nitems, &after, &prop ) == Success) {
        if (prop != NULL) {
            XFree( prop );
        }
        if (type != None) {
            return w;
        }
    }

    Window root, parent;
    Window* children;
    unsigned int nchildren;
    Window ret = None;

    if (XQueryTree(QX11Info::display(), w, &root, &parent, &children, &nchildren ) != 0) {
        for( unsigned int i = 0; i < nchildren && ret == None; ++i) {
            ret = findRealWindow(children[i], depth+1);
        }
        if (children != NULL) {
            XFree(children);
        }
    }

    return ret;

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

    Window real_child = findRealWindow(child);
    if (real_child != None) {// test just in case
        child = real_child;
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
    m_recordData.geometry = QApplication::desktop()->screenGeometry();
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
    d->workDir = Settings::workDir().pathOrUrl();
    if (m_recorderPlugin) {
        m_pluginManager->unloadPlugin(m_recorderPlugin);
        m_recorderPlugin = 0;
    }

    const QString name = backendCombo->currentText();
    m_recorderPlugin = static_cast<AbstractRecorder*>(m_pluginManager->loadPlugin(name));
    if (!m_recorderPlugin || d->outputFile.isEmpty()) {
        if (!m_recorderPlugin) {
            KMessageBox::sorry(this, i18n("Cannot load Recorder %1", name));
        } else {
            KMessageBox::sorry(this, i18n("No output file specified."));
        }
        setState(Idle);
        return;
    }
    connect(m_recorderPlugin, SIGNAL(error(QString)), this, SLOT(recorderError(QString)));
    connect(m_recorderPlugin, SIGNAL(finished(AbstractRecorder::ExitStatus)), this,
            SLOT(recorderFinished(AbstractRecorder::ExitStatus)));
    connect(m_recorderPlugin, SIGNAL(status(QString)), this, SLOT(recorderStatus(QString)));
    connect(m_recorderPlugin, SIGNAL(outputFileChanged(QString)), outputRequester,
            SLOT(setText(QString)));

    recorderStatus("");

    if (Settings::hideOnRecord()) {
        if (Settings::tray()) {
            close();
        } else {
            hide();
        }
    }

}


void MainWindow::setupTray()
{

    if (Settings::tray()) {
        if (!m_tray) {
#if (KDE_VERSION >= KDE_MAKE_VERSION(4,3,64))
            m_tray = new KStatusNotifierItem(this);
            m_tray->setStatus(KStatusNotifierItem::Active);
            m_tray->setCategory(KStatusNotifierItem::ApplicationStatus);
            m_tray->setIconByName("video-display");
            connect(m_tray, SIGNAL(activateRequested(bool,QPoint)), this,
                    SLOT(trayActivated(bool,QPoint)));
#else
            m_tray = new KSystemTrayIcon(this);
            m_tray->setIcon(KIcon("video-display"));
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
        Q_UNUSED(name);
#warning "KDE-4.3 TrayOverlay disabled"
#endif
    }

}


void MainWindow::setState(const State &newState)
{

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
            fpsSpinBox->setEnabled(m_currentFeatures[AbstractRecorder::Fps]);
            soundCheck->setEnabled(m_currentFeatures[AbstractRecorder::Sound]);
            centralWidget()->setEnabled(true);
            break;
        }
    case Timer:
    case Recording: {
            setTrayOverlay("media-record");
            getAction("pause")->setIcon(KIcon("media-playback-pause"));
            getAction("record")->setEnabled(false);
            getAction("pause")->setEnabled(m_currentFeatures[AbstractRecorder::Pause]);
            getAction("stop")->setEnabled(m_currentFeatures[AbstractRecorder::Stop]);
            getAction("recordWindow")->setEnabled(false);
            getAction("recordFullScreen")->setEnabled(false);
            getAction("box")->setEnabled(false);
            getAction("options_configure")->setEnabled(false);
            getAction("upload")->setEnabled(false);
            centralWidget()->setEnabled(false);
            break;
        }
    case TimerPaused:
    case Paused: {
            setTrayOverlay("media-playback-pause");
            getAction("pause")->setText(i18n("Continue"));
            getAction("pause")->setIcon(KIcon("media-playback-start"));
            getAction("record")->setEnabled(false);
            getAction("pause")->setEnabled(m_currentFeatures[AbstractRecorder::Pause]);
            getAction("stop")->setEnabled(m_currentFeatures[AbstractRecorder::Stop]);
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


void MainWindow::recorderStatus(const QString &text)
{

    statusBar()->showMessage(text);

}


void MainWindow::recorderError(const QString &error)
{

    KMessageBox::error(this, error);
    m_pluginManager->unloadPlugin(m_recorderPlugin);
    m_recorderPlugin = 0;
    recorderStatus(i18n("Error: %1", error));
    setState(Idle);

}


void MainWindow::recorderFinished(const AbstractRecorder::ExitStatus &status)
{

    kDebug() << "recorder finished";

    if (status == AbstractRecorder::Crash) {
        KMessageBox::error(this, i18n("recorder crashed"));
    }


    if (!Settings::encode() || !m_recorderPlugin->isVideoRecorder()) {
        m_pluginManager->unloadPlugin(m_recorderPlugin);
        m_recorderPlugin = 0;
        setState(Idle);
        playFile(false);
        recorderStatus(i18n("Finished!"));
        return;
    }
    m_pluginManager->unloadPlugin(m_recorderPlugin);
    m_recorderPlugin = 0;

    QList<KPluginInfo> list = m_pluginManager->getEncoderList();
    if (m_encoderPlugin) {
        m_encoderPlugin->disconnect(this);
        m_encoderPlugin->deleteLater();
    }
    m_encoderPlugin = static_cast<AbstractEncoder*>(
            m_pluginManager->loadPlugin(Settings::encoderName()));

    if (!m_encoderPlugin) {
        setState(Idle);
        playFile(false);
        recorderStatus(i18n("Finished!"));
        return;
    }

    connect(m_encoderPlugin, SIGNAL(finished()), this, SLOT(encoderFinished()));
    connect(m_encoderPlugin, SIGNAL(status(QString)), this, SLOT(recorderStatus(QString)));
    connect(m_encoderPlugin, SIGNAL(error(QString)), this, SLOT(encoderError(QString)));
    connect(m_encoderPlugin, SIGNAL(outputFileChanged(QString)), outputRequester,
            SLOT(setText(QString)));

    AbstractEncoder::Data d;
    d.overwrite = Settings::overwrite();
    d.file = outputRequester->text();
    d.workDir = Settings::workDir().pathOrUrl();
    m_encoderPlugin->encode(d);

}


void MainWindow::encoderFinished()
{

    m_pluginManager->unloadPlugin(m_encoderPlugin);
    m_encoderPlugin = 0;

    setState(Idle);
    playFile(false);
    recorderStatus(i18n("Finished!"));

}


void MainWindow::encoderError(const QString &error)
{

    KMessageBox::error(this, error);
    m_pluginManager->unloadPlugin(m_encoderPlugin);
    m_encoderPlugin = 0;
    recorderStatus(i18n("Error: %1", error));
    setState(Idle);

}


void MainWindow::configure()
{

    ConfigDialog *dialog = new ConfigDialog(this, m_pluginManager);
    connect(dialog, SIGNAL(dialogFinished()), this, SLOT(dialogFinished()));
    dialog->show();

}


void MainWindow::dialogFinished()
{

    setupTray();
    updateRecorderCombo();
    updateUploaderMenu();

}


void MainWindow::updateRecorderCombo()
{

    const QString oldBackend = backendCombo->currentText();
    backendCombo->clear();
    foreach (const KPluginInfo &info, m_pluginManager->getRecorderList()) {
        if (info.isPluginEnabled()) {
            backendCombo->addItem(KIcon(info.icon()), info.name());
        }
    }
    if (backendCombo->contains(oldBackend)) {
        backendCombo->setCurrentItem(oldBackend, false);
    }

}


void MainWindow::playFile(const bool &force)
{

    if (Settings::showVideo() || force) {
        KUrl url(outputRequester->text());

        if (!QFile(url.path()).exists()) {
            const QStringList files = QDir(url.directory()).entryList(QStringList() << url.fileName()+".*");
            if (!files.isEmpty()) {
                url = files.first();
            }
        }

        KMimeType::Ptr type = KMimeType::findByUrl(url);
        KRun::runUrl(url, type->name(), this);
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
        outputRequester->clear();
    }

}


void MainWindow::startTimer()
{

    setState(Timer);
    if (timerLcd->value() == 0) {
        m_timer->start(0);
    } else {
        m_timer->start(1000);
    }

}


void MainWindow::tick()
{

    if (timerLcd->value() < 1) {
        m_timer->stop();
        startRecord();
    }
    lcdDown();

}


void MainWindow::lcdUp()
{

    if (timerLcd->value() == 60) {
        return;
    }
    timerLcd->display(timerLcd->value()+1);

}


void MainWindow::lcdDown()
{

    if (timerLcd->value() == 0) {
        return;
    }
    timerLcd->display(timerLcd->value()-1);

}

#if (KDE_VERSION >= KDE_MAKE_VERSION(4,3,64))
void MainWindow::trayActivated(const bool &active, const QPoint &pos)
{

    Q_UNUSED(pos);
    if (active && (state() == Recording || state() == Paused)) {
        stopRecord();
    }

}
#else
void MainWindow::trayActivated(const QSystemTrayIcon::ActivationReason &reason)
{

    if (reason == QSystemTrayIcon::Trigger &&
        isVisible() &&
        (state() == Recording || state() == Paused)) {
        stopRecord();
    }

}
#endif

void MainWindow::backendChanged(const QString &newBackend)
{

    AbstractRecorder *recorder = static_cast<AbstractRecorder*>(
            m_pluginManager->loadPlugin(newBackend));

    if (!recorder) {
        m_currentFeatures[AbstractRecorder::Sound] = false;
        m_currentFeatures[AbstractRecorder::Fps] = false;
        m_currentFeatures[AbstractRecorder::Pause] = false;
        m_currentFeatures[AbstractRecorder::Stop] = false;
        outputRequester->setText("");
    } else {
        m_currentFeatures[AbstractRecorder::Sound] = recorder->hasFeature(AbstractRecorder::Sound);
        m_currentFeatures[AbstractRecorder::Fps] = recorder->hasFeature(AbstractRecorder::Fps);
        m_currentFeatures[AbstractRecorder::Pause] = recorder->hasFeature(AbstractRecorder::Pause);
        m_currentFeatures[AbstractRecorder::Stop] = recorder->hasFeature(AbstractRecorder::Stop);
        outputRequester->setText(recorder->getDefaultOutputFile());

        m_pluginManager->unloadPlugin(recorder);
    }
    setState(Idle); // update actions/widgets

}


void MainWindow::aboutToQuit()
{

    kDebug() << "quit...";
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

    foreach (const KPluginInfo &info, m_pluginManager->getUploaderList()) {
        if (info.isPluginEnabled()) {
            KAction *uploadAction = new KAction(this);
            uploadAction->setText(info.name());
            uploadAction->setIcon(KIcon(info.icon()));
            uploadAction->setData(info.name());
            connect(uploadAction, SIGNAL(triggered()), this, SLOT(upload()));
            menu->addAction(uploadAction);
        }
    }

}


void MainWindow::upload()
{

    QAction *uploadAction = static_cast<QAction*>(sender());
    AbstractUploader *uploader = static_cast<AbstractUploader*>(
            m_pluginManager->loadPlugin(uploadAction->data().toString()));
    if (uploader) {
        uploader->show(outputRequester->text(), this);
    }

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


#if (KDE_VERSION >= KDE_MAKE_VERSION(4,3,64))
    #include "mainwindow.moc"
#else
    #include "mainwindow_4_3.moc"
#endif


