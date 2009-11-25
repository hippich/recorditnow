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
#include "zoomview.h"

// KDE
#include <kdebug.h>

// Qt
#include <QtGui/QX11Info>
#include <QtGui/QPainter>
#include <QtCore/QTimer>
#include <QtGui/QWheelEvent>

// X
#ifdef XFIXES_FOUND
    #include <X11/extensions/Xfixes.h>
    #include <X11/Xlib.h>
#endif


ZoomView::ZoomView(QWidget *parent)
    : QWidget(parent, Qt::X11BypassWindowManagerHint|Qt::FramelessWindowHint|Qt::Tool)
{

    m_factor = 2;
    m_followMouse = true;
    setMinimumSize(350, 300);
    setMaximumSize(350, 300);

    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(updateView()));

    m_timer->start(1000/18); // 18 FPS

}



ZoomView::~ZoomView()
{

    delete m_timer;

}


void ZoomView::setFactor(const qreal &factor)
{

    m_factor = factor;
    updateView();

}



void ZoomView::setSize(const QSize &size)
{

    setMinimumSize(size);
    setMaximumSize(size);
    updateView();

}


void ZoomView::setFollowMouse(const bool &follow)
{

    if (follow) {
        setWindowFlags(Qt::X11BypassWindowManagerHint|Qt::FramelessWindowHint|Qt::Tool);
    } else {
        setWindowFlags(Qt::Window);
    }
    m_followMouse = follow;

}


void ZoomView::updateView()
{

    QRect target(0, 0, size().width()/m_factor, size().height()/m_factor);
    target.moveCenter(QCursor::pos());

    m_pixmap = QPixmap::grabWindow(x11Info().appRootWindow(),
                                   target.x(),
                                   target.y(),
                                   target.width(),
                                   target.height());

#ifdef XFIXES_FOUND
    // cursor
    XFixesCursorImage *xcursor = XFixesGetCursorImage(QX11Info::display());
    unsigned char *pixels = (unsigned char*) malloc(xcursor->width*xcursor->height*4);
    for (int i = 0; i < xcursor->width*xcursor->height; i++) {
        unsigned long pix = xcursor->pixels[i];
        pixels[i * 4] = pix & 0xff;
        pixels[(i * 4) + 1] = (pix >> 8) & 0xff;
        pixels[(i * 4) + 2] = (pix >> 16) & 0xff;
        pixels[(i * 4) + 3] = (pix >> 24) & 0xff;
    }
    QImage qcursor(pixels, xcursor->width, xcursor->height, QImage::Format_ARGB32);
    QPainter painter(&m_pixmap);

    painter.drawImage(xcursor->x-(target.x()+xcursor->xhot), xcursor->y-(target.y()+xcursor->xhot),
                      qcursor);

    painter.end();

    free(pixels);
    XFree(xcursor);
#endif
    
    m_pixmap = m_pixmap.scaled(size(), Qt::KeepAspectRatio, Qt::FastTransformation);

    if (m_followMouse) {
        QRect geo = geometry();
        geo.moveTopLeft(QCursor::pos()+(QCursor::pos()-target.topLeft()));
        setGeometry(geo);
    }

    update();

}


void ZoomView::paintEvent(QPaintEvent *event)
{

    Q_UNUSED(event);

    QPainter painter(this);
    painter.drawPixmap(contentsRect(), m_pixmap);

    QPen pen;
    pen.setWidth(4);
    pen.setColor(Qt::gray);

    painter.setPen(pen);
    painter.drawRect(rect());

}


void ZoomView::wheelEvent(QWheelEvent *event)
{

    if (event->delta() > 0) {
        m_factor++;
        m_factor = qMin((int)m_factor, 10);
    } else {
        m_factor--;
        m_factor = qMax((int)m_factor, 1);
    }
    updateView();

}

