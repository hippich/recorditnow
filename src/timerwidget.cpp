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
#include "timerwidget.h"

// KDE
#include <kicon.h>

// Qt
#include <QtCore/QTimer>


namespace RecordItNow {


TimerWidget::TimerWidget(QWidget *parent)
    : QFrame(parent)
{

    setupUi(this);

    m_timer = new QTimer(this);
    m_timer->setInterval(1000);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(tick()));

    upButton->setIcon(KIcon("arrow-up"));
    downButton->setIcon(KIcon("arrow-down"));

    connect(upButton, SIGNAL(clicked()), this, SLOT(upClicked()));
    connect(downButton, SIGNAL(clicked()), this, SLOT(downClicked()));

}


TimerWidget::~TimerWidget()
{

    delete m_timer;

}


int TimerWidget::value() const
{

    return lcd->value();

}


void TimerWidget::setValue(const int &value)
{

    lcd->display(value);
    m_time = value;

}


void TimerWidget::start()
{

    if (value() == 0) {
        emit timeout();
    } else {
        m_timer->start();
    }

}


void TimerWidget::pause()
{

    m_timer->stop();

}


void TimerWidget::reset()
{

    setValue(m_time);

}


void TimerWidget::upClicked()
{

    if (value() < 61) {
        setValue(value()+1);
    }

}


void TimerWidget::downClicked()
{

    if (value() > 0) {
        setValue(value()-1);
    }

}


void TimerWidget::tick()
{

    if (value() > 0) {
        lcd->display(value()-1);

        emit tick(value());
    } else {
        m_timer->stop();

        emit timeout();
    }

}


} // namespace RecordItNow


#include "timerwidget.moc"
