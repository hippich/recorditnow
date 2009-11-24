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

// KDE
#include <kdebug.h>
#include <kapplication.h>
#include <kwindowsystem.h>
#include <kxerrorhandler.h>

// Qt
#include <QtGui/QCursor>
#include <QtCore/QTimer>
#include <QtGui/QPainter>
#include <QtCore/QEvent>
#include <QtCore/QDateTime>
#include <QtCore/QThread>
#include <QtCore/QAbstractEventDispatcher>

// X11
#include <X11/Xlib.h>



CursorWidget::CursorWidget(QWidget *parent)
    : QWidget(parent, Qt::X11BypassWindowManagerHint|Qt::FramelessWindowHint|Qt::Tool)
{

    setAttribute(Qt::WA_TranslucentBackground);
    KWindowSystem::setState(winId(), NET::KeepAbove);

    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(updatePos()));

    m_resetTimer = new QTimer(this);
    connect(m_resetTimer, SIGNAL(timeout()), this, SLOT(resetColor()));
    m_resetTimer->setSingleShot(true);

    m_timer->start(1000/30);

    setContentsMargins(2, 2, 2, 2);

    setSize(QSize(40, 40));

    show();

    m_normalColor = Qt::black;
    m_currentColor = m_normalColor;

    updateGrab(true);

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

    updateGrab(false); // ungrab old buttons
    m_buttons = buttons;
    updateGrab(true);

}



void CursorWidget::click(const int &button)
{

    kDebug() << "click";
    if (m_resetTimer->isActive()) {
        m_resetTimer->stop();
    }

    m_currentColor = m_buttons[button];
    XAllowEvents(x11Info().display(), ReplayPointer, CurrentTime);
    update();

    m_resetTimer->start(250);

}



WId CursorWidget::getWindow() const
{

    return static_cast<QWidget*>(parent())->winId();

}


void CursorWidget::updatePos()
{

    QRect geo = geometry();
    geo.moveTopLeft(QCursor::pos()+QPoint(20, 20));
    setGeometry(geo);

}


void CursorWidget::resetColor()
{

    m_currentColor = m_normalColor;
    update();

}


void CursorWidget::updateGrab(const bool &grab)
{

    QHashIterator<int, QColor> it(m_buttons);
    if (grab) {
        while (it.hasNext()) {
            it.next();
            XGrabButton(x11Info().display(),
                        it.key(),
                        AnyModifier,
                        x11Info().appRootWindow(),
                        True,
                        ButtonPressMask,
                        GrabModeSync,
                        GrabModeAsync,
                        x11Info().appRootWindow(),
                        None);
        }

    } else {
        while (it.hasNext()) {
            it.next();
            XUngrabButton(x11Info().display(),
                          it.key(),
                          AnyModifier,
                          x11Info().appRootWindow());
        }
    }

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

