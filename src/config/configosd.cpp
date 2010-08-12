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
#include "configosd.h"

// KDE
#include <kdebug.h>
#include <klocalizedstring.h>
#include <kicon.h>

// Qt
#include <QtGui/QMouseEvent>
#include <QtGui/QApplication>
#include <QtGui/QDesktopWidget>
#include <QtGui/QGridLayout>
#include <QtGui/QToolButton>
#include <QtGui/QSpinBox>
#include <QtGui/QLabel>


namespace RecordItNow {


ConfigOSD::ConfigOSD(QWidget *parent)
    : RecordItNow::OSD(parent)
{

    setBackgroundImage("widgets/tooltip");
    setBlurEnabled(true);

    QGridLayout *layout = new QGridLayout;

    m_moveButton = new QToolButton(this);
    m_moveButton->setIcon(KIcon("transform-move"));

    m_hSpin = new QSpinBox(this);
    m_wSpin = new QSpinBox(this);

    m_wSpin->setMaximum(9000);
    m_wSpin->setSingleStep(20);

    m_hSpin->setMaximum(9000);
    m_hSpin->setSingleStep(20);

    QLabel *hLabel = new QLabel(i18n("Height:"), this);
    QLabel *wLabel = new QLabel(i18n("Width:"), this);
    QLabel *mLabel = new QLabel(i18n("Move:"), this);

    layout->addWidget(wLabel, 0, 0);
    layout->addWidget(m_wSpin, 0, 1);
    layout->addItem(new QSpacerItem(10, 5, QSizePolicy::Expanding, QSizePolicy::Preferred), 0, 2);
    layout->addWidget(hLabel, 0, 3);
    layout->addWidget(m_hSpin, 0, 4);

    QHBoxLayout *mLayout = new QHBoxLayout;
    mLayout->addWidget(mLabel);
    mLayout->addWidget(m_moveButton);
    layout->addLayout(mLayout, 1, 0, 1, 4, Qt::AlignCenter);

    setLayout(layout);

    connect(m_moveButton, SIGNAL(pressed()), this, SLOT(movePressed()));
    m_moveButton->installEventFilter(this);

    connect(m_wSpin, SIGNAL(valueChanged(int)), this, SLOT(widthValueChanged(int)));
    connect(m_hSpin, SIGNAL(valueChanged(int)), this, SLOT(heightValueChanged(int)));

}


ConfigOSD::~ConfigOSD()
{


}


void ConfigOSD::checkGeometry()
{

    const QRect screenGeometry = qApp->desktop()->screenGeometry(this);
    QRect geometry = RecordItNow::OSD::geometry();

    if (geometry.left() < screenGeometry.left()) {
        geometry.moveLeft(screenGeometry.left());
    }
    if (geometry.top() < screenGeometry.top()) {
        geometry.moveTop(screenGeometry.top());
    }
    if (geometry.right() > screenGeometry.right()) {
        geometry.moveRight(screenGeometry.right());
    }
    if (geometry.bottom() > screenGeometry.bottom()) {
        geometry.moveBottom(screenGeometry.bottom());
    }

    if (geometry != RecordItNow::OSD::geometry()) {
        setGeometry(geometry);
    }

}


void ConfigOSD::movePressed()
{

    grabMouse(Qt::SizeAllCursor);

}


void ConfigOSD::widthValueChanged(const int &value)
{

    const QRect screenGeometry = qApp->desktop()->screenGeometry(this);
    int left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);

    if (value > screenGeometry.width()) {
        m_wSpin->setValue(screenGeometry.width());
    } else if (value < layout()->minimumSize().width()+left+right) {
        m_wSpin->setValue(layout()->minimumSize().width()+left+right);
    } else {
        resize(value, height());
    }
    emit OSDChanged();

}


void ConfigOSD::heightValueChanged(const int &value)
{

    const QRect screenGeometry = qApp->desktop()->screenGeometry(this);
    int left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);

    if (value > screenGeometry.height()) {
        m_hSpin->setValue(screenGeometry.height());
    } else if (value < layout()->minimumSize().height()+top+bottom) {
        m_hSpin->setValue(layout()->minimumSize().height()+top+bottom);
    } else {
        resize(width(), value);
    }
    emit OSDChanged();

}


bool ConfigOSD::eventFilter(QObject *watched, QEvent *event)
{

    if (watched != m_moveButton) {
        return RecordItNow::OSD::eventFilter(watched, event);
    }

    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouse = static_cast<QMouseEvent*>(event);
        m_lastPos = mouse->globalPos();
    }
    return false;

}


void ConfigOSD::resizeEvent(QResizeEvent *event)
{

    RecordItNow::OSD::resizeEvent(event);

    m_wSpin->setValue(width());
    m_hSpin->setValue(height());
    checkGeometry();

}


void ConfigOSD::mousePressEvent(QMouseEvent *event)
{

    RecordItNow::OSD::mousePressEvent(event);
    m_lastPos = event->globalPos();

}


void ConfigOSD::mouseMoveEvent(QMouseEvent *event)
{

    RecordItNow::OSD::mouseMoveEvent(event);

    if (!mouseGrabber()) {
        return;
    }

    QRect geometry = RecordItNow::OSD::geometry();
    geometry.moveTopLeft(geometry.topLeft()-(m_lastPos-event->globalPos()));
    setGeometry(geometry);

    m_lastPos = event->globalPos();
    checkGeometry();

    emit OSDChanged();

}


void ConfigOSD::mouseReleaseEvent(QMouseEvent *event)
{

    RecordItNow::OSD::mouseReleaseEvent(event);
    if (mouseGrabber()) {
        releaseMouse();

        qApp->sendEvent(m_moveButton, event); // release
    }
    m_lastPos = QPoint(-1, -1);

}


} // namespace RecordItNow


#include "configosd.moc"
