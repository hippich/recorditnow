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
#include "framebox.h"
#include "line.h"

// Qt
#include <QtGui/QWidget>
#include <QtGui/QResizeEvent>
#include <QtGui/QMoveEvent>
#include <QtGui/QLabel>
#include <QtGui/QHBoxLayout>
#include <QtCore/QTimer>

// KDE
#include <kwindowsystem.h>
#include <kdebug.h>



#define MIN_SIZE 100
#define SPACING 20



FrameBox::FrameBox(QWidget *parent, QRect pos)
    : QObject(parent),
    m_parent(parent)
{

    m_enabled = false;

    // hide timer for sizeFrame
    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(hideFrame()));

    // sizeFrame
    m_sizeFrame = new QFrame(0, Qt::X11BypassWindowManagerHint);
    QHBoxLayout *layout = new QHBoxLayout;
    m_sizeFrame->setLayout(layout);
    m_sizeLabel = new QLabel(m_sizeFrame);
    layout->addWidget(m_sizeLabel);
    m_sizeFrame->hide();

    // lines
    m_bottom = new Line(Line::Bottom);
    m_top = new Line(Line::Top);
    m_left = new Line(Line::Left);
    m_right = new Line(Line::Right);

    // workaround... TODO fix top and bottom dia. move
    KWindowSystem::setState(m_top->winId(), NET::KeepBelow);
    KWindowSystem::setState(m_bottom->winId(), NET::KeepBelow);

    connect(m_left, SIGNAL(geometryChanged(Line::Side, QRect)), this,
            SLOT(lineGeometryChanged(Line::Side, QRect)));
    connect(m_top, SIGNAL(geometryChanged(Line::Side, QRect)), this,
            SLOT(lineGeometryChanged(Line::Side, QRect)));
    connect(m_right, SIGNAL(geometryChanged(Line::Side, QRect)), this,
            SLOT(lineGeometryChanged(Line::Side, QRect)));
    connect(m_bottom, SIGNAL(geometryChanged(Line::Side, QRect)), this,
            SLOT(lineGeometryChanged(Line::Side, QRect)));

    // parent widget
    m_parent->installEventFilter(this);

    // restore size
    if (pos.width() < MIN_SIZE) {
        pos.setWidth(MIN_SIZE);
    }

    if (pos.height() < MIN_SIZE) {
        pos.setHeight(MIN_SIZE);
    }
    QRect geo = pos;

    geo.setHeight(m_top->height());
    m_top->setGeometry(geo);
    geo.moveBottom(pos.bottom());
    m_bottom->setGeometry(geo);

    geo = pos;
    geo.setWidth(m_left->width());
    m_left->setGeometry(geo);
    geo.moveRight(pos.right());
    m_right->setGeometry(geo);

    adjustToParent();

}


FrameBox::~FrameBox()
{

    delete m_top;
    delete m_left;
    delete m_right;
    delete m_bottom;
    delete m_sizeFrame;
    delete m_timer;

}


QRect FrameBox::boxGeometry() const
{

    QRect geo;

    geo.setX(m_left->geometry().right());
    geo.setY(m_top->geometry().bottom());
    geo.setHeight(m_bottom->geometry().top()-m_top->geometry().bottom());
    geo.setWidth(m_right->geometry().left()-m_left->geometry().right());

    return geo;

}


bool FrameBox::isEnabled() const
{

    return m_enabled;

}


void FrameBox::setEnabled(const bool &enabled)
{

    m_enabled = enabled;
    m_left->setVisible(enabled);
    m_top->setVisible(enabled);
    m_right->setVisible(enabled);
    m_bottom->setVisible(enabled);

    if (enabled) {
        adjustToParent();
    }

}


void FrameBox::adjustToParent()
{

    QRect geometry = m_top->geometry();
    geometry.moveLeft(m_parent->geometry().left());
    geometry.moveTop(m_parent->geometry().bottomLeft().y()+SPACING);
    m_top->setGeometry(geometry);
    adjustLines();

}


void FrameBox::adjustLines()
{

    // adjust to top
    const QRect topGeometry = m_top->geometry();
    QRect leftGeometry = m_left->geometry();
    QRect rightGeometry = m_right->geometry();
    QRect bottomGeometry = m_bottom->geometry();

    leftGeometry.moveTopLeft(topGeometry.topLeft());
    m_left->setGeometry(leftGeometry);

    rightGeometry.moveTopRight(topGeometry.topRight());
    m_right->setGeometry(rightGeometry);

    bottomGeometry.moveBottomLeft(leftGeometry.bottomLeft());
    m_bottom->setGeometry(bottomGeometry);

    adjustSizeFrame(boxGeometry());

}


void FrameBox::adjustSizeFrame(const QRect &boxGeo)
{

    m_sizeFrame->resize(m_sizeLabel->sizeHint());
    QRect frameGeo = m_sizeFrame->geometry();
    frameGeo.moveCenter(boxGeo.center());
    m_sizeFrame->setGeometry(frameGeo);

}


void FrameBox::lineGeometryChanged(const Line::Side &side, const QRect &geometry)
{

    if (m_timer->isActive()) {
        m_timer->stop();
    }

    switch (side) {
    case Line::Left: {
            QRect topGeometry = m_top->geometry();
            const QRect leftGeometry = geometry;
            QRect rightGeometry = m_right->geometry();
            QRect bottomGeometry = m_bottom->geometry();

            if (leftGeometry.x()+MIN_SIZE > rightGeometry.x() || leftGeometry.height() < MIN_SIZE) {
                break;
            } else {
                m_left->setGeometry(geometry);
            }

            topGeometry.setTopLeft(leftGeometry.topLeft());
            topGeometry.setHeight(m_top->height());
            m_top->setGeometry(topGeometry);

            rightGeometry.setTopRight(topGeometry.topRight());
            rightGeometry.setHeight(leftGeometry.height());
            m_right->setGeometry(rightGeometry);

            bottomGeometry.moveBottomLeft(leftGeometry.bottomLeft());
            bottomGeometry.setWidth(m_top->width());
            m_bottom->setGeometry(bottomGeometry);
            break;
        }
    case Line::Top: {
            const QRect topGeometry = geometry;
            QRect leftGeometry = m_left->geometry();
            QRect rightGeometry = m_right->geometry();
            QRect bottomGeometry = m_bottom->geometry();

            if (topGeometry.y()+MIN_SIZE > bottomGeometry.y()) {
                break;
            } else {
                m_top->setGeometry(geometry);
            }

            leftGeometry.setTopLeft(topGeometry.topLeft());
            leftGeometry.setWidth(m_left->width());
            m_left->setGeometry(leftGeometry);

            rightGeometry.setTopRight(topGeometry.topRight());
            rightGeometry.setBottom(leftGeometry.bottom());
            rightGeometry.setWidth(m_right->width());
            m_right->setGeometry(rightGeometry);

            bottomGeometry.setBottomLeft(leftGeometry.bottomLeft());
            bottomGeometry.setHeight(m_bottom->height());
            m_bottom->setGeometry(bottomGeometry);
            break;
        }
    case Line::Right: {
            QRect topGeometry = m_top->geometry();
            QRect leftGeometry = m_left->geometry();
            const QRect rightGeometry = geometry;
            QRect bottomGeometry = m_bottom->geometry();

            if (rightGeometry.x()-MIN_SIZE < leftGeometry.x() || rightGeometry.height() < MIN_SIZE) {
                break;
            } else {
                m_right->setGeometry(geometry);
            }

            topGeometry.setTopRight(rightGeometry.topRight());
            topGeometry.setHeight(m_top->height());
            m_top->setGeometry(topGeometry);

            leftGeometry.setTopLeft(topGeometry.topLeft());
            leftGeometry.setBottom(rightGeometry.bottom());
            leftGeometry.setWidth(m_left->width());
            m_left->setGeometry(leftGeometry);

            bottomGeometry.setWidth(m_top->width());
            bottomGeometry.moveBottom(leftGeometry.bottom());
            m_bottom->setGeometry(bottomGeometry);
            break;
        }
    case Line::Bottom: {
            QRect topGeometry = m_top->geometry();
            QRect leftGeometry = m_left->geometry();
            QRect rightGeometry = m_right->geometry();
            const QRect bottomGeometry = geometry;

            if (bottomGeometry.y()-MIN_SIZE < topGeometry.y()) {
                break;
            } else {
                m_bottom->setGeometry(geometry);
            }

            leftGeometry.setBottomLeft(bottomGeometry.bottomLeft());
            leftGeometry.setWidth(m_left->width());
            m_left->setGeometry(leftGeometry);

            topGeometry.setTopLeft(leftGeometry.topLeft());
            topGeometry.setHeight(m_top->height());
            m_top->setGeometry(topGeometry);

            rightGeometry.setTopRight(topGeometry.topRight());
            rightGeometry.setBottomRight(bottomGeometry.bottomRight());
            rightGeometry.setWidth(m_right->width());
            m_right->setGeometry(rightGeometry);
            break;
        }
    }

    QRect parentGeometry = m_parent->geometry();
    parentGeometry.moveLeft(m_top->geometry().left());

    if (parentGeometry.bottom() > m_top->geometry().top()) {
        parentGeometry.moveBottom(m_top->geometry().top()-SPACING);
    } else if (parentGeometry.bottom()+SPACING != m_top->geometry().top()) {
        parentGeometry.moveBottom(m_top->geometry().top()-SPACING);
    }

    m_parent->setGeometry(parentGeometry);

    const QRect boxGeo = boxGeometry();
    QString text;
    text = QString::number(boxGeo.width());
    text += " x ";
    text += QString::number(boxGeo.height());
    m_sizeLabel->setText(text);

    adjustSizeFrame(boxGeo);

    m_sizeFrame->show();

    m_timer->start(1000);

}


void FrameBox::hideFrame()
{

    m_sizeFrame->hide();

}


bool FrameBox::eventFilter(QObject *watched, QEvent *event)
{

    switch (event->type()) {
    case QEvent::Move: adjustToParent(); break;
    default: break;
    }
    return QObject::eventFilter(watched, event);

}


