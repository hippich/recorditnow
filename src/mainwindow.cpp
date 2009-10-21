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
#include "mainwindow.h"
#include "framebox.h"
#include <recorditnow.h>

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
#include <knotificationitem.h>
#include <kmenu.h>
#include <krun.h>
#include <kmimetype.h>

// X11
#include <X11/Xlib.h>



MainWindow::MainWindow(QWidget *parent)
    : KXmlGuiWindow(parent),
    m_grabber(0)
{

    setupActions();
    menuBar()->hide();

    QWidget *toolWidget = new QWidget;
    ui_toolBarWidget.setupUi(toolWidget);
    setCentralWidget(toolWidget);

    ui_toolBarWidget.timeUpButton->setIcon(KIcon("arrow-up"));
    ui_toolBarWidget.timeDownButton->setIcon(KIcon("arrow-down"));
    connect(ui_toolBarWidget.timeUpButton, SIGNAL(clicked()), this, SLOT(lcdUp()));
    connect(ui_toolBarWidget.timeDownButton, SIGNAL(clicked()), this, SLOT(lcdDown()));

    m_box = new FrameBox(this);
    m_recorderPlugin = 0;
    ui_toolBarWidget.backendCombo->addItems(recorder().keys());
    m_tray = 0;
    setupTray();

    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(tick()));

    setupGUI();
    setState(Idle);

}


MainWindow::~MainWindow()
{

    if (m_grabber) {
        delete m_grabber;
    }

    delete m_box;

    if (m_recorderPlugin) {
        delete m_recorderPlugin;
    }
    if (m_tray) {
        delete m_tray;
    }
    delete m_timer;

}


void MainWindow::setupActions()
{

    QAction *recordAction = new QAction(this);
    recordAction->setText("Record");
    recordAction->setIcon(KIcon("media-record"));
    connect(recordAction, SIGNAL(triggered()), this, SLOT(recordTriggred()));

    QAction *pauseAction = new QAction(this);
    pauseAction->setObjectName("pause");
    pauseAction->setText("Pause");
    pauseAction->setIcon(KIcon("media-playback-pause"));
    pauseAction->setEnabled(false);
    connect(pauseAction, SIGNAL(triggered()), this, SLOT(pauseRecord()));

    QAction *stopAction = new QAction(this);
    stopAction->setText("Stop");
    stopAction->setIcon(KIcon("media-playback-stop"));
    stopAction->setEnabled(false);
    connect(stopAction, SIGNAL(triggered()), this, SLOT(stopRecord()));


    QAction *recordWindowAction = new QAction(this);
    recordWindowAction->setText(i18n("Record a Window"));
    recordWindowAction->setIcon(KIcon("window-new")); // TODO
    connect(recordWindowAction, SIGNAL(triggered()), this, SLOT(recordWindow()));

    QAction *boxAction = new QAction(this);
    boxAction->setText(i18n("Show Frame"));
    boxAction->setIcon(KIcon("draw-rectangle"));
    boxAction->setCheckable(true);
    connect(boxAction, SIGNAL(triggered()), this, SLOT(boxWindow()));

    QAction *fullAction = new QAction(this);
    fullAction->setText(i18n("Record the entire Screen"));
    fullAction->setIcon(KIcon("view-fullscreen"));
    connect(fullAction, SIGNAL(triggered()), this, SLOT(recordFullScreen()));


    actionCollection()->addAction("record", recordAction);
    actionCollection()->addAction("pause", pauseAction);
    actionCollection()->addAction("stop", stopAction);

    actionCollection()->addAction("recordWindow", recordWindowAction);
    actionCollection()->addAction("box", boxAction);
    actionCollection()->addAction("recordFullScreen", fullAction);


    KStandardAction::preferences(this, SLOT(configure()), actionCollection());

}


void MainWindow::startRecord()
{

    m_recorderPlugin->record(m_recordData);

}


void MainWindow::pauseRecord()
{

    if (state() == Recording) {
        setState(Paused);
        m_recorderPlugin->pause();
    } else {
        setState(Recording);
        m_recorderPlugin->pause();
    }

}


void MainWindow::stopRecord()
{

    if (m_recorderPlugin) {
        m_recorderPlugin->stop();
    }
    setState(Idle);

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


void MainWindow::boxWindow()
{

    m_box->setEnabled(!m_box->isEnabled());
    actionCollection()->action("box")->setChecked(m_box->isEnabled());

}


void MainWindow::recordFullScreen()
{

    kDebug() << "record fullscreen";
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
            actionCollection()->action("pause")->setEnabled(false);
            actionCollection()->action("record")->setEnabled(true);
            actionCollection()->action("stop")->setEnabled(false);
            actionCollection()->action("recordWindow")->setEnabled(true);
        }
    }
    return KXmlGuiWindow::eventFilter(watched, event);

}


void MainWindow::loadRecorder(const QString &name)
{

    if (m_recorderPlugin) {
        delete m_recorderPlugin;
        m_recorderPlugin = 0;
    }

    QHash<QString, QString> rec = recorder();

    if (!rec.contains(name)) {
        kDebug() << "recorder:" << name << "not found";
        return;
    }

    kDebug() << "load recorder:" << rec[name];
    KService::Ptr service = KService::serviceByStorageId(rec[name]);

    KPluginLoader loader(service->library());
    KPluginFactory *factory = loader.factory();
    if (!factory) {
        kError(5001) << "KPluginFactory could not load the plugin:" << service->library() <<
        "Reason:" << loader.errorString();
    }

    m_recorderPlugin = factory->create<AbstractRecorder>(this);
    if (!m_recorderPlugin) {
        kError(5001) << "factory::create<>() failed " << service->library();
    }

    connect(m_recorderPlugin, SIGNAL(error(QString)), this, SLOT(recorderError(QString)));
    connect(m_recorderPlugin, SIGNAL(status(QString)), this, SLOT(recorderStatus(QString)));
    connect(m_recorderPlugin, SIGNAL(finished(AbstractRecorder::ExitStatus)), this,
            SLOT(recorderFinished(AbstractRecorder::ExitStatus)));

    KSharedConfig::Ptr ptr = KSharedConfig::openConfig("recorditnowrc");
    m_recorderPlugin->loadConfig(ptr);

}


QHash<QString, QString> MainWindow::recorder()
{

    kDebug() << "search...";

    QHash<QString, QString> rec;    
    KService::List offers = KServiceTypeTrader::self()->query("RecordItNowRecorder");
    KService::List::const_iterator iter;
    for (iter = offers.begin(); iter < offers.end(); ++iter) {
        KService::Ptr service = *iter;
        rec[service->name()] = service->entryPath();
    }
    kDebug() << rec.size() << "recorder found.";

    return rec;

}


void MainWindow::initRecorder(Data *d)
{

    setState(Recording);
    if (m_box->isEnabled()) {
        m_box->setEnabled(false);
    }

    QString path = ui_toolBarWidget.outputRequester->text();
    path.replace("~/", QDir::homePath()+'/');

    d->geometry = QRect(-1, -1, -1, -1);
    d->winId = -1;

    d->fps = ui_toolBarWidget.fpsSpinBox->value();
    d->sound = ui_toolBarWidget.soundCheck->isChecked();
    d->outputFile = path;

    loadRecorder(ui_toolBarWidget.backendCombo->currentText());

    if (Settings::hideOnRecord()) {
        hide();
    }

}


void MainWindow::setupTray()
{

    if (Settings::tray()) {
        if (!m_tray) {
            m_tray = new KNotificationItem(this);
            m_tray->setIconByName("video-display");
            m_tray->setStatus(KNotificationItem::Active);
            connect(m_tray, SIGNAL(activateRequested(bool,QPoint)), this,
                    SLOT(trayActivated(bool,QPoint)));

            KMenu *context = new KMenu(this);
            context->addAction(actionCollection()->action("record"));
            context->addAction(actionCollection()->action("pause"));
            context->addAction(actionCollection()->action("stop"));
            context->addSeparator();
            context->addAction(actionCollection()->action("box"));
            context->addAction(actionCollection()->action("recordWindow"));
            context->addAction(actionCollection()->action("recordFullScreen"));

            m_tray->setContextMenu(context);
            setAttribute(Qt::WA_DeleteOnClose, false);
        }
    } else {
        if (m_tray) {
            delete m_tray;
            m_tray = 0;
            setAttribute(Qt::WA_DeleteOnClose, true);
        }
    }

}


void MainWindow::setTrayOverlay(const QString &name)
{

    if (m_tray) {
        m_tray->setOverlayIconByName(name);
    }

}


void MainWindow::setState(const State &newState)
{

    switch (newState) {
    case Idle: {
            setTrayOverlay("");
            actionCollection()->action("pause")->setIcon(KIcon("media-playback-pause"));
            actionCollection()->action("record")->setEnabled(true);
            actionCollection()->action("pause")->setEnabled(false);
            actionCollection()->action("stop")->setEnabled(false);
            actionCollection()->action("recordWindow")->setEnabled(true);
            actionCollection()->action("recordFullScreen")->setEnabled(true);
            actionCollection()->action("box")->setEnabled(true);
            actionCollection()->action("box")->setChecked(m_box->isEnabled());
            actionCollection()->action("options_configure")->setEnabled(true);
            break;
        }
    case Recording: {
            setTrayOverlay("media-record");
            actionCollection()->action("pause")->setIcon(KIcon("media-playback-pause"));
            actionCollection()->action("record")->setEnabled(false);
            actionCollection()->action("pause")->setEnabled(true);
            actionCollection()->action("stop")->setEnabled(true);
            actionCollection()->action("recordWindow")->setEnabled(false);
            actionCollection()->action("recordFullScreen")->setEnabled(false);
            actionCollection()->action("box")->setEnabled(false);
            actionCollection()->action("options_configure")->setEnabled(false);
            break;
        }
    case Paused: {
            setTrayOverlay("media-playback-pause");
            actionCollection()->action("pause")->setText(i18n("Continue"));
            actionCollection()->action("pause")->setIcon(KIcon("media-playback-start"));
            actionCollection()->action("record")->setEnabled(false);
            actionCollection()->action("pause")->setEnabled(true);
            actionCollection()->action("stop")->setEnabled(true);
            actionCollection()->action("recordWindow")->setEnabled(false);
            actionCollection()->action("recordFullScreen")->setEnabled(false);
            actionCollection()->action("box")->setEnabled(false);
            actionCollection()->action("options_configure")->setEnabled(false);
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
    setState(Idle);
    m_recorderPlugin->deleteLater();
    m_recorderPlugin = 0;

}


void MainWindow::recorderFinished(const AbstractRecorder::ExitStatus &status)
{

    kDebug() << "recorder finished";
    setState(Idle);

    if (status == AbstractRecorder::Crash) {
        KMessageBox::error(this, i18n("recorder crashed"));
    }

    if (Settings::showVideo()) {
        KUrl url(ui_toolBarWidget.outputRequester->text());

        if (!QFile(url.path()).exists()) {
            QStringList files = QDir(url.directory()).entryList(QStringList() << url.fileName()+".*");
            if (!files.isEmpty()) {
                url = files.first();
            }
        }

        KMimeType::Ptr type = KMimeType::findByUrl(url);
        KRun::runUrl(url, type->name(), this);
    }

    m_recorderPlugin->deleteLater();
    m_recorderPlugin = 0;

}


void MainWindow::configure()
{

    KConfigDialog *dialog = new KConfigDialog(this, "settings", Settings::self());

    QWidget *general = new QWidget;
    ui_settings.setupUi(general);
    dialog->addPage(general, i18n("RecordItNow"), "configure");


    AbstractRecorder *tmp = 0;
    if (m_recorderPlugin) {
        tmp = m_recorderPlugin;
    }

    QHash<QString, QString> rec = recorder();
    QHashIterator<QString, QString> it(rec);
    while (it.hasNext()) {
        it.next();
        loadRecorder(it.key());
        if (!m_recorderPlugin) {
            kDebug() << "load failed!";
            continue;
        }
        if (m_recorderPlugin->hasConfigPage()) {
            dialog->addPage(m_recorderPlugin->configPage(), it.key(), "configure");
            m_recorder.append(m_recorderPlugin);
        } else {
            delete m_recorderPlugin;
        }
        m_recorderPlugin = 0;
   }

    if (tmp) {
        m_recorderPlugin = tmp;
    }

    dialog->setAttribute(Qt::WA_DeleteOnClose);

    connect(dialog, SIGNAL(finished(int)), this, SLOT(saveConfig(int)));

    dialog->show();

}


void MainWindow::saveConfig(int code)
{

    KSharedConfig::Ptr ptr = KSharedConfig::openConfig("recorditnowrc");
    foreach (AbstractRecorder *rec, m_recorder) {
        if (code == QDialog::Accepted) {
            rec->saveConfig(ptr);
        }
        delete rec;
    }
    m_recorder.clear();
    setupTray();

}


void MainWindow::startTimer()
{

    if (ui_toolBarWidget.timerLcd->value() == 0) {
        m_timer->start(0);
    } else {
        m_timer->start(1000);
    }

}


void MainWindow::tick()
{

    if (ui_toolBarWidget.timerLcd->value() < 2) {
        m_timer->stop();
        startRecord();
    }
    lcdDown();

}


void MainWindow::lcdUp()
{

    if (ui_toolBarWidget.timerLcd->value() == 60) {
        return;
    }
    ui_toolBarWidget.timerLcd->display(ui_toolBarWidget.timerLcd->value()+1);

}


void MainWindow::lcdDown()
{

    if (ui_toolBarWidget.timerLcd->value() == 0) {
        return;
    }
    ui_toolBarWidget.timerLcd->display(ui_toolBarWidget.timerLcd->value()-1);

}


void MainWindow::trayActivated(const bool &active, const QPoint &pos)
{

    Q_UNUSED(pos);
    if (active && (state() == Recording || state() == Paused)) {
        stopRecord();
    }

    if (!active && m_box->isEnabled()) {
        boxWindow();
    }

}




