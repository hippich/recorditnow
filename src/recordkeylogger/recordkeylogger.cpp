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
#include "recordkeylogger.h"
#include "recordkeylogger_p.h"

// KDE
#include <klocalizedstring.h>

// Qt
#include <QtGui/QX11Info>
#include <QtCore/QDebug>
#include <QtCore/QtConcurrentRun>
#include <QtCore/QMutex>
#include <QtGui/QApplication>
#include <QtCore/QTimer>

// C
#include <unistd.h>

// X
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysymdef.h>
#include <X11/keysym.h>
#include <X11/extensions/record.h>
#include <X11/Xlibint.h>



// for this struct, refer to libxnee
typedef union {
  unsigned char    type;
  xEvent           event;
  xResourceReq     req;
  xGenericReply    reply;
  xError           error;
  xConnSetupPrefix setup;
} XRecordDatum;


namespace RecordItNow {


struct WatchData {
    Display *data_display;
    Display *ctrl_display;
    XRecordRange *ranges[2];
    XRecordClientSpec rcs;
    XRecordContext rc;

    RecordKeyloggerPrivate *d;
    bool stop;
    QMutex mutex;
};


RecordKeyloggerPrivate::RecordKeyloggerPrivate(RecordKeylogger *parent)
    : QObject(parent), q(parent)
{

    data = new WatchData();
    data->d = this;
    data->stop = false;
    running = false;

}


RecordKeyloggerPrivate::~RecordKeyloggerPrivate()
{

    delete data;

}


void RecordKeyloggerPrivate::emitKey(const RecordItNow::KeyloggerEvent &event)
{

    emit keyEvent(event);

}


void RecordKeyloggerPrivate::emitError(const QString &errorString)
{

    running = false;
    emit error(errorString);

}



RecordKeylogger::RecordKeylogger(QObject *parent)
    : RecordItNow::AbstractKeylogger(parent), d(new RecordKeyloggerPrivate(this))
{

    connect(d, SIGNAL(keyEvent(RecordItNow::KeyloggerEvent)), this, SIGNAL(keyEvent(RecordItNow::KeyloggerEvent)));
    connect(d, SIGNAL(error(QString)), this, SIGNAL(error(QString)));

}


RecordKeylogger::~RecordKeylogger()
{

    stop();
    delete d;

}


static void event_callback(XPointer priv, XRecordInterceptData *hook)
{

    if (hook->category != XRecordFromServer) {
      XRecordFreeData (hook);
      return;
    }


    WatchData *watch = (WatchData*)priv;
    XRecordDatum *data = (XRecordDatum*) hook->data;

    int buttonCode, keyCode;
    buttonCode = keyCode = data->event.u.u.detail;
    char *string = 0;
    RecordItNow::KeyloggerEvent event;

    watch->mutex.lock();
    if (watch->stop) {
        XRecordFreeData (hook);
        watch->mutex.unlock();
        return;
    }

    switch (data->type) {
    case KeyPress: {
            event.setType(RecordItNow::KeyloggerEvent::KeyboardEvent);
            event.setPressed(true);
            event.setId(keyCode);

            string = XKeysymToString(XKeycodeToKeysym(watch->ctrl_display, keyCode, 0));
            event.setText(QString(string));

          //  qDebug() << "key press" << string;
            break;
        }
    case KeyRelease: {
            event.setType(RecordItNow::KeyloggerEvent::KeyboardEvent);
            event.setPressed(false);
            event.setId(keyCode);

            string = XKeysymToString(XKeycodeToKeysym(watch->ctrl_display, keyCode, 0));
            event.setText(QString(string));

            //qDebug() << "key release" << string;
            break;
        }
    case ButtonPress: {
            event.setType(RecordItNow::KeyloggerEvent::MouseEvent);
            event.setPressed(true);
            event.setId(buttonCode);

           // qDebug() << "button press";
            break;
        }
    case ButtonRelease: {
            event.setType(RecordItNow::KeyloggerEvent::MouseEvent);
            event.setPressed(false);
            event.setId(buttonCode);

           // qDebug() << "button release";
            break;
        }
    default: break;
    }

    XRecordFreeData(hook);

    watch->d->emitKey(event);
    watch->mutex.unlock();

}


static void recordLoop(RecordItNow::WatchData *data)
{

    XSynchronize(data->data_display, True);

    data->ranges[0] = XRecordAllocRange();
    data->ranges[1] = XRecordAllocRange();
    if (!data->ranges[0] || !data->ranges[1]) {
        qDebug() << "Could not alloc record range object!";
        data->d->emitError(i18n("Internal Error"));
        return;
    }


    data->ranges[0]->device_events.first = KeyPress;
    data->ranges[0]->device_events.last = KeyRelease;
    data->ranges[1]->device_events.first = ButtonPress;
    data->ranges[1]->device_events.last = ButtonRelease;
    data->rcs = XRecordAllClients;

    data->rc = XRecordCreateContext(data->data_display, 0, &data->rcs, 1, data->ranges, 2);
    if (!data->rc) {
        qDebug() << "Could not create a record context!";
        data->d->emitError(i18n("Internal Error"));
        return;
    }

    if (XRecordEnableContextAsync(data->data_display, data->rc, event_callback, (XPointer)data) != 1) {
        qDebug() << "Cound not enable the record context!";
        data->d->emitError(i18n("Internal Error"));
        return;
    }

    while (!data->stop) {
        XRecordProcessReplies(data->data_display);
        usleep(500);
    }

    if (!XRecordDisableContext(data->ctrl_display, data->rc)) {
        qFatal("XRecordDisableContext) failed!");
    }
    XRecordFreeContext(data->ctrl_display, data->rc);
    XFree(data->ranges[0]);
    XFree(data->ranges[1]);
    XCloseDisplay(data->ctrl_display);
    XCloseDisplay(data->data_display);

}


bool RecordKeylogger::start(const KConfig *)
{

    QMutexLocker(&d->data->mutex);

    if (d->running) {
        qDebug() << "already started";
        return true;
    }

    const QString displayString = XDisplayString(QX11Info::display());
    qDebug() << "start:" << displayString;

    d->data->stop = false;    

    XLockDisplay(QX11Info::display());
    qApp->syncX(); // Prevent freezing by QWidget::grabMouse()
    d->data->data_display = XOpenDisplay(displayString.toLatin1().constData());
    d->data->ctrl_display = XOpenDisplay(displayString.toLatin1().constData());
    XUnlockDisplay(QX11Info::display());

    if (!d->data->data_display) {
        qDebug() << "Could not open display:" << displayString;
        return false;
    } else if (!d->data->ctrl_display) {
        XCloseDisplay(d->data->data_display);
        qDebug() << "Could not open display2:" << displayString;
        return false;
    }



    int junk;
    if (!XQueryExtension(d->data->data_display, "RECORD", &junk, &junk, &junk)) {
        qDebug() << "RECORD extension not found.";
        return false;
    }

    d->running = true;
    d->future = QtConcurrent::run(recordLoop, d->data);

    QTimer::singleShot(100, this, SIGNAL(started()));

    return true;

}



void RecordKeylogger::stop()
{

    QMutexLocker(&d->data->mutex);
    if (!d->running) {
        return;
    }

    qDebug() << "stopping rec";

    d->data->stop = true;

    if (d->future.isRunning()) {
        d->future.waitForFinished();
    }
    d->running = false;

    emit stopped();

    qDebug() << "rec stopped";

}


bool RecordKeylogger::waitForStarted()
{

    return isRunning();

}


bool RecordKeylogger::isRunning()
{

    return d->running;

}



} // namespace RecordItNow


#include "recordkeylogger.moc"
#include "recordkeylogger_p.moc"

