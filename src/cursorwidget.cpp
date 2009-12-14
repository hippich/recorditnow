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
#include "cursorwidget.h"
#include "mousebutton.h"
#include "keymon/device.h"
#include "keymon/manager.h"

// KDE
#include <kdeversion.h>
#if KDE_IS_VERSION(4,3,80)
    #include <kauth.h>
#endif
#include <kdebug.h>
#include <kapplication.h>
#include <kwindowsystem.h>
#include <kxerrorhandler.h>
#include <klocalizedstring.h>

// Qt
#include <QtGui/QCursor>
#include <QtCore/QTimer>
#include <QtGui/QPainter>
#include <QtCore/QEvent>
#include <QtCore/QDateTime>
#include <QtCore/QThread>
#include <QtGui/QDesktopWidget>

// X11
#include <X11/Xlib.h>
#include <X11/Xcursor/Xcursor.h>


CursorWidget::CursorWidget(QWidget *parent)
    : QWidget(parent, Qt::X11BypassWindowManagerHint|Qt::FramelessWindowHint|Qt::Tool)
{

    setAttribute(Qt::WA_TranslucentBackground);
    // setAttribute(Qt::WA_TransparentForMouseEvents);

    m_useKeyMon = false;
    m_grab = false;
    m_device = 0;

    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(updatePos()));

    m_resetTimer = new QTimer(this);
    connect(m_resetTimer, SIGNAL(timeout()), this, SLOT(resetColor()));
    m_resetTimer->setSingleShot(true);

    m_timer->start(1000/25);

    setContentsMargins(2, 2, 2, 2);

    setSize(QSize(40, 40));

    show();

    m_normalColor = Qt::black;
    m_currentColor = m_normalColor;

}


CursorWidget::~CursorWidget()
{

    updateGrab(false);

    delete m_timer;
    delete m_resetTimer;

}


void CursorWidget::setSize(const QSize &size)
{

    setMinimumSize(size);
    setMaximumSize(size);

}


void CursorWidget::setNormalColor(const QColor &color)
{

    m_normalColor = color;
    resetColor();

}


void CursorWidget::setButtons(const QHash<int, QColor> &buttons)
{

    m_buttons = buttons;

}


void CursorWidget::setUseKeyMon(const bool &use, const QString &deviceName)
{

    m_useKeyMon = use;
    m_deviceName = deviceName;

}


void CursorWidget::start()
{

    updateGrab(false); // ungrab old buttons
    updateGrab(true);

}


void CursorWidget::stop()
{

    hide();
    updateGrab(false);
    deleteLater();

}



void CursorWidget::click(const int &button)
{

    if (m_resetTimer->isActive()) {
        m_resetTimer->stop();
    }

    m_currentColor = m_buttons[button];
    XAllowEvents(x11Info().display(), ReplayPointer, CurrentTime);
    update();

    m_resetTimer->start(350);

}



WId CursorWidget::getWindow() const
{

    return static_cast<QWidget*>(parent())->winId();

}


void CursorWidget::updatePos()
{

    QRect geo = geometry();

    const int size = XcursorGetDefaultSize(x11Info().display());
    geo.moveTopLeft(QCursor::pos()+QPoint(size, size));

    const QRect desktop = kapp->desktop()->screenGeometry(x11Info().appScreen());
    if (!desktop.contains(geo)) {
        if (geo.x()+geo.width() > desktop.width()) {
            geo.moveRight(desktop.right());
        }
        if (geo.y()+geo.height() > desktop.height()) {
            geo.moveBottom(desktop.bottom());
        }

        if (geo.contains(QCursor::pos())) {
            geo.moveBottomRight(QCursor::pos()-QPoint(size/2, size/2));
        }
    }

    setGeometry(geo);
    raise();

}


void CursorWidget::resetColor()
{

    m_currentColor = m_normalColor;
    update();

}


void CursorWidget::updateGrab(const bool &grab)
{

    QHashIterator<int, QColor> it(m_buttons);
    const int screen = x11Info().appScreen();
    if (grab) {
        if (!m_useKeyMon) {
            while (it.hasNext()) {
                it.next();
                KXErrorHandler handler;
                XGrabButton(x11Info().display(),
                            it.key(),
                            AnyModifier,
                            x11Info().appRootWindow(screen),
                            True,
                            ButtonPressMask,
                            GrabModeSync,
                            GrabModeAsync,
                            x11Info().appRootWindow(screen),
                            None);
                if (handler.error(true)) {
                    XErrorEvent event = handler.errorEvent();
                    if (event.error_code == 10) { // BadAccess
                        MouseButton::Button button = MouseButton::getButtonFromXButton(it.key());
                        const QString buttonName = MouseButton::getName(button);
                        emit error(i18n("Grab failed!\n"
                                        "Perhaps the button \"%1\" is already grabbed by another"
                                        " application.", buttonName));
                    }
                }
            }
        } else {
#if KDE_IS_VERSION(4,3,80)
            m_grab = true;
            KAuth::Action action("org.kde.recorditnow.helper.watch");
            action.setHelperID("org.kde.recorditnow.helper");
            connect(action.watcher(), SIGNAL(progressStep(QVariantMap)), this,
                    SLOT(progressStep(QVariantMap)));

            QVariantMap args;
            args["Device"] = m_deviceName;
            action.setArguments(args);
            action.setExecutesAsync(true);

            KAuth::ActionReply reply = action.execute("org.kde.recorditnow.helper");
            if (reply.type() != 2) {
                hide();
                emit error(i18n("Grab failed!"));
            }
#else
            m_device = KeyMon::Manager::watch(m_deviceName, this);
            if (m_device) {
                connect(m_device, SIGNAL(buttonPressed(KeyMon::Event)), this,
                        SLOT(buttonPressed(KeyMon::Event)));
            } else {
                emit error(i18n("Grab failed!"));
            }
#endif
        }
    } else {
        while (it.hasNext()) {
            it.next();
            XUngrabButton(x11Info().display(),
                          it.key(),
                          AnyModifier,
                          x11Info().appRootWindow(screen));
        }
#if KDE_IS_VERSION(4,3,80)
        if (m_grab) {
            KAuth::Action action;
            action.setName("wattch");
            action.setHelperID("org.kde.recorditnow.helper");
            action.stop();
            m_grab = false;
        }
#else
        if (m_device) {
            m_device->disconnect(this);
            m_device->deleteLater();
            m_device = 0;
        }
#endif
    }

}


void CursorWidget::buttonPressed(const KeyMon::Event &event)
{

    if (m_resetTimer->isActive()) {
        m_resetTimer->stop();
    }

    switch (event.key) {
    case KeyMon::Event::WheelUp:
    case KeyMon::Event::WheelDown: {
            m_currentColor = m_buttons[event.keyToXButton(event.key)];
            m_resetTimer->start(350);
            break;
        }
    default: {
            if (event.pressed) {
                m_currentColor = m_buttons[event.keyToXButton(event.key)];
            } else {
                m_currentColor = m_normalColor;
            }
            break;
        }
    }
    update();

}


void CursorWidget::progressStep(const QVariantMap &data)
{

#if KDE_IS_VERSION(4,3,80)
    KeyMon::Event event;
    event.key = static_cast<KeyMon::Event::Key>(data["Key"].toInt());
    event.pressed = data["Pressed"].toBool();
    buttonPressed(event);
#else
    Q_UNUSED(data);
#endif

}


void CursorWidget::paintEvent(QPaintEvent *event)
{

    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // base
    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(m_currentColor);
    painter.setBrush(brush);
    painter.drawEllipse(contentsRect());

    // spot
    QRadialGradient grad(contentsRect().center(), size().height()/2);
    grad.setColorAt(0, Qt::white);
    grad.setColorAt(1, Qt::transparent);
    grad.setFocalPoint(contentsRect().center()-QPoint(size().height()/4, size().height()/4));
    painter.setBrush(QBrush(grad));
    painter.drawEllipse(contentsRect());

    // border
    QPen pen;
    pen.setWidth(2);
    pen.setColor(Qt::black);
    painter.setPen(pen);
    painter.drawEllipse(contentsRect());

}


#include "cursorwidget.moc"

