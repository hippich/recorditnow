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
#include "keywidget.h"

// KDE
#include <kdebug.h>
#include <kicon.h>

// Qt
#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>
#include <QtCore/QTimer>
#include <QtGui/QSizePolicy>


KeyWidget::KeyWidget(const QString &image, const int &code, QWidget *parent)
    : QWidget(parent), m_image(image), m_pressed(false)
{

    m_keyCode = code;

    setContentsMargins(2, 2, 2, 2);
    setMinimumSize(50, 50);

    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(timeout()));

}


KeyWidget::~KeyWidget()
{

    delete m_timer;

}


int KeyWidget::keyCode() const
{

    return m_keyCode;

}


void KeyWidget::setPressed(const bool &pressed)
{

    if (m_timer->isActive()) {
        m_timer->stop();
    }

    if (!pressed) {
        m_timer->start(400);
    } else {
        m_pressed = true;
        update();
    }

}


void KeyWidget::timeout()
{

    m_pressed = !m_pressed;
    update();

}


void KeyWidget::paintEvent(QPaintEvent *event)
{

    QPainter painter(this);
    painter.setClipRegion(event->region());
    painter.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform);

    if (!m_pressed) {
        painter.setOpacity(0.3);
    }

    QRect rect = contentsRect();
    rect.setWidth(rect.height());
    rect.moveCenter(contentsRect().center());

    painter.drawPixmap(rect, m_image);

}


void KeyWidget::resizeEvent(QResizeEvent *event)
{

    const int w = event->size().width();
    resize(w, w);

    QWidget::resizeEvent(event);

}


#include "keywidget.moc"

