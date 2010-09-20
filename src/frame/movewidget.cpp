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
#include "movewidget.h"

// KDE
#include <kdebug.h>
#include <kicon.h>
#include <kiconloader.h>

// Qt
#include <QtGui/QMouseEvent>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>


MoveWidget::MoveWidget(QWidget *parent)
    : QWidget(parent, Qt::FramelessWindowHint|Qt::Tool|Qt::X11BypassWindowManagerHint),
    m_move(false)
{

    Q_ASSERT(parent);
    parent->installEventFilter(this);
    setCursor(Qt::SizeAllCursor);

    int size, junk;
    parentWidget()->getContentsMargins(&junk, &size, &junk, &junk);
    setMaximumSize(size*2, size*2);
    setMinimumSize(maximumSize());

    setMouseTracking(true);

    setAttribute(Qt::WA_NoSystemBackground, true);
    setVisible(parentWidget()->isVisible());

}


MoveWidget::~MoveWidget()
{



}


void MoveWidget::moveToParent()
{

    const QRect parentGeometry = parentWidget()->geometry();

    int size, junk;
    parentWidget()->getContentsMargins(&junk, &size, &junk, &junk);
    size = size/2;

    QPoint center;
    center.setX(parentGeometry.center().x());
    center.setY(parentGeometry.y()+size);
    QRect newGeometry = geometry();
    newGeometry.moveCenter(center);
    setGeometry(newGeometry);

}


bool MoveWidget::eventFilter(QObject *watched, QEvent *event)
{

    if (!m_move) {
        switch (event->type()) {
        case QEvent::Show: show();
        case QEvent::Move:
        case QEvent::Resize: moveToParent(); break;
        case QEvent::Hide: hide(); break;
        default: break;
        }
    }
    return QWidget::eventFilter(watched, event);

}


void MoveWidget::mouseMoveEvent(QMouseEvent *event)
{

    if (!m_move) {
        return;
    }

    QRect geometry = this->geometry();
    const QPoint way = m_pos-event->globalPos();

    geometry.moveTopLeft(geometry.topLeft()-way);
    m_pos = event->globalPos();

    setGeometry(geometry);

    geometry = parentWidget()->geometry();
    QPoint topLeft = this->geometry().center();

    int size, junk;
    parentWidget()->getContentsMargins(&junk, &size, &junk, &junk);
    size = size/2;

    topLeft.setX(topLeft.x()-(geometry.width()/2));
    topLeft.setY(topLeft.y()-size);

    geometry.moveTopLeft(topLeft);
    parentWidget()->setGeometry(geometry);

}


void MoveWidget::mousePressEvent(QMouseEvent *event)
{

    event->accept();

    m_move = true;
    m_pos = event->globalPos();
    grabMouse(Qt::ClosedHandCursor);

}


void MoveWidget::mouseReleaseEvent(QMouseEvent *event)
{

    event->accept();

    m_move = false;
    releaseMouse();

}


void MoveWidget::showEvent(QShowEvent *event)
{

    QWidget::showEvent(event);
    moveToParent();

}


void MoveWidget::paintEvent(QPaintEvent *event)
{

    QPainter painter(this);
    painter.setClipRect(event->rect());

    QPen pen;
    pen.setColor(Qt::lightGray);
    pen.setWidth(2);

    painter.setPen(pen);

    painter.fillRect(rect(), Qt::black);

    QRect frame = rect();
    frame.setHeight(frame.height()-pen.width());
    frame.setWidth(frame.width()-pen.width());
    frame.moveTopLeft(frame.topLeft()+QPoint(pen.width()/2, pen.width()/2));

    painter.drawRect(frame);

}


#include "movewidget.moc"
