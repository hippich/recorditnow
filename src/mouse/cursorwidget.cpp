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
#include "config/mousebuttonwidget.h"
#include "keymonmanager.h"
#include "helper.h"

// KDE
#include <kdebug.h>
#include <kapplication.h>
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
#include <QtGui/QPaintEvent>
#include <QtGui/QBitmap>

// X11
#include <X11/Xlib.h>
#include <X11/Xcursor/Xcursor.h>
#include <X11/extensions/shape.h>


#define RESET_TIME 600
CursorWidget::CursorWidget(QWidget *parent)
    : QWidget(parent, Qt::X11BypassWindowManagerHint|Qt::FramelessWindowHint|Qt::Tool)
{

    setAttribute(Qt::WA_TranslucentBackground);
    setAutoFillBackground(false);

    // _UGLY_ HACK: Prevents KWin to draw a shadow
    setAttribute(Qt::WA_X11NetWmWindowTypeDock);

    m_grab = false;
    m_mode = LEDMode;
    m_opacity = 0.4;
    m_show = true;

    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(updatePos()));

    m_resetTimer = new QTimer(this);
    connect(m_resetTimer, SIGNAL(timeout()), this, SLOT(resetColor()));
    m_resetTimer->setSingleShot(true);

    setContentsMargins(2, 2, 2, 2);

    setSize(QSize(40, 40));

    m_defaultButton = MouseButton(-1, Qt::black);
    m_currentButton = m_defaultButton;

    connect(KeyMonManager::self(), SIGNAL(keyEvent(KeyMon::Event)), this,
            SLOT(buttonPressed(KeyMon::Event)));
    connect(RecordItNow::Helper::self(), SIGNAL(compositingChanged(bool)), this, SLOT(updateMask()));

}


CursorWidget::~CursorWidget()
{

    delete m_timer;
    delete m_resetTimer;

}


void CursorWidget::setSize(const QSize &size)
{

    setMinimumSize(size);
    setMaximumSize(size);

}


void CursorWidget::setButtons(const QList<MouseButton> &buttons)
{

    m_buttons = buttons;

}


void CursorWidget::setDevice(const QString &deviceName)
{

    m_deviceName = deviceName;

}


void CursorWidget::setMode(const CursorWidget::WidgetMode &mode)
{

    m_mode = mode;

    if (m_mode == CircleMode || m_mode == TargetMode) {
        // click-through
        int junk;
        if (XQueryExtension(x11Info().display(), "SHAPE", &junk, &junk, &junk)) {
            Pixmap mask = XCreatePixmap(x11Info().display(), winId(), 1, 1, 1);
            XShapeCombineMask(x11Info().display(), winId(), ShapeInput, 0, 0, mask, ShapeSet);
            XFreePixmap(x11Info().display(), mask);
        } else {
            kWarning() << "SHAPE extension is _NOT_ present!";
            m_mode = LEDMode;
        }
    }

    updateMask();
    update();

}


void CursorWidget::setOpacity(const qreal &opacity)
{

    if (m_opacity == opacity) {
        return;
    }

    const double max = 1.0;
    const double onePercent = max/100;
    const double value = opacity*onePercent;

    m_opacity = value;
    update();

}


void CursorWidget::setShowAlways(const bool &show)
{

    m_show = show;

}


void CursorWidget::switchToPreviewMode()
{

    setWindowFlags(windowFlags()&~(Qt::X11BypassWindowManagerHint|Qt::FramelessWindowHint|Qt::Tool));
    show(); // necessary to apply window flags
    m_timer->disconnect(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(previewColors()));
    m_timer->start(2000);

}


void CursorWidget::start()
{

    m_timer->start(1000/50);

    if (m_show) {
        show();
    }

}


void CursorWidget::stop()
{

    hide();
    deleteLater();

}


void CursorWidget::updatePos()
{

    QRect geo = geometry();

    switch (m_mode) {
    case LEDMode: {
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
            break;
        }
    case CircleMode:
    case TargetMode: geo.moveCenter(QCursor::pos()); break;
    }

    setGeometry(geo);
    raise();

}


void CursorWidget::resetColor()
{

    m_currentButton = m_defaultButton;

    if (m_show) {
        update();
    } else {
        hide();
    }

}


MouseButton CursorWidget::getButton(const int &code) const
{

    foreach (const MouseButton &button, m_buttons) {
        if (button.code() == code) {
            return button;
        }
    }
    return MouseButton();

}


void CursorWidget::buttonPressed(const KeyMon::Event &event)
{

    if (!event.mouseEvent) {
        return;
    }

    if (m_resetTimer->isActive()) {
        m_resetTimer->stop();
    }

    if (!isVisible()) {
        show();
    }

    const MouseButton button = getButton(event.keyToXButton(event.key));
    if (!button.sound().isEmpty() && event.pressed) {
        RecordItNow::Helper::self()->playSound(button.sound());
    }
    
    if (!button.isValid()) {
        return;
    }
    
    switch (event.key) {
    case KeyMon::Event::WheelUp:
    case KeyMon::Event::WheelDown: {
            m_currentButton = button;
            m_resetTimer->start(RESET_TIME);
            break;
        }
    default: {
            if (event.pressed) {
                m_currentButton = button;
            } else {
                m_resetTimer->start(RESET_TIME);
            }
            break;
        }
    }
    update();

}



void CursorWidget::previewColors()
{

    int index = m_buttons.indexOf(m_currentButton);
    if (index == -1) {
        if (!m_buttons.isEmpty()) {
            m_currentButton = m_buttons.at(0);
        } else {
            m_currentButton = m_defaultButton;
        }
    } else {
        index++;
        if (index >= m_buttons.size()) {
            if (!m_buttons.isEmpty()) {
                m_currentButton = m_buttons.first();
            } else {
                m_currentButton = m_defaultButton;
            }
        } else {
            m_currentButton = m_buttons.at(index);
        }
    }
    update();

}


void CursorWidget::paintEvent(QPaintEvent *event)
{

    QPainter painter(this);
    painter.setClipRegion(event->region());

    switch (m_mode) {
    case LEDMode: paintLED(&painter); break;
    case CircleMode: paintCircle(&painter); break;
    case TargetMode: paintTarget(&painter); break;
    }

}


void CursorWidget::paintLED(QPainter *painter)
{

    painter->setRenderHint(QPainter::Antialiasing);
    const bool compositing = RecordItNow::Helper::self()->compositingActive();


    if (!compositing) {
        QBrush brush;
        brush.setColor(Qt::gray);
        brush.setStyle(Qt::SolidPattern);
        painter->setBrush(brush);
        painter->drawRect(rect());
    }

    // base
    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(m_currentButton.color());
    painter->setBrush(brush);
    painter->drawEllipse(contentsRect());

    // spot
    QRadialGradient grad(contentsRect().center(), size().height()/2);
    grad.setColorAt(0, Qt::white);
    grad.setColorAt(1, Qt::transparent);
    grad.setFocalPoint(contentsRect().center()-QPoint(size().height()/4, size().height()/4));
    painter->setBrush(QBrush(grad));
    painter->drawEllipse(contentsRect());

    // border
    if (compositing) {
        QPen pen;
        pen.setWidth(2);
        pen.setColor(Qt::black);
        painter->setPen(pen);
        painter->drawEllipse(contentsRect());
    }

}


void CursorWidget::paintCircle(QPainter *painter)
{

    painter->setRenderHints(QPainter::Antialiasing|QPainter::HighQualityAntialiasing);

    if (RecordItNow::Helper::self()->compositingActive()) {
        QRadialGradient grad(contentsRect().center(), size().height());
        grad.setColorAt(0, m_currentButton.color());
        grad.setColorAt(1, Qt::transparent);
        painter->setBrush(QBrush(grad));
        painter->setOpacity(m_opacity);
        painter->drawEllipse(contentsRect());
    } else {
        painter->fillRect(rect(), m_currentButton.color());
    }

}


void CursorWidget::paintTarget(QPainter *painter)
{

    if (!RecordItNow::Helper::self()->compositingActive()) {
        paintCircle(painter);
        return;
    }

    painter->setRenderHints(QPainter::Antialiasing|QPainter::HighQualityAntialiasing);

    QPen pen;
    pen.setWidth(2);
    pen.setColor(m_currentButton.color());
    painter->setPen(pen);
    QRect rect = contentsRect();

    QRect ellipse = rect;
    ellipse.setWidth(ellipse.width()/1.5);
    ellipse.setHeight(ellipse.height()/1.5);
    ellipse.moveCenter(rect.center());

    QLine hLine1;
    hLine1.setP1(QPoint(rect.left(), rect.center().y()));
    hLine1.setP2(QPoint(rect.width()/3, rect.center().y()));

    QLine hLine2;
    hLine2.setP1(QPoint(rect.right()-(rect.width()/3), rect.center().y()));
    hLine2.setP2(QPoint(rect.right(), rect.center().y()));

    QLine vLine1;
    vLine1.setP1(QPoint(rect.center().x(), rect.top()));
    vLine1.setP2(QPoint(rect.center().x(), rect.height()/3));

    QLine vLine2;
    vLine2.setP1(QPoint(rect.center().x(), rect.bottom()));
    vLine2.setP2(QPoint(rect.center().x(), rect.bottom()-(rect.height()/3)));

    painter->drawLine(hLine1);
    painter->drawLine(hLine2);
    painter->drawLine(vLine1);
    painter->drawLine(vLine2);

    painter->drawEllipse(ellipse);

}


void CursorWidget::resizeEvent(QResizeEvent *event)
{

    QWidget::resizeEvent(event);
    updateMask();

}


void CursorWidget::updateMask()
{

    switch (m_mode) {
    case TargetMode: clearMask(); break;
    case CircleMode: {
            if (!RecordItNow::Helper::self()->compositingActive()) {
                QRect maskRect = rect();
                maskRect.setHeight(maskRect.height()-10);
                maskRect.setWidth(maskRect.width()-10);
                maskRect.moveCenter(rect().center());
                setMask(QRegion(maskRect).xored(QRegion(rect())));
            } else {
                clearMask();
            }
            break;
        }
    case LEDMode: clearMask(); break;
    }

}


#include "cursorwidget.moc"

