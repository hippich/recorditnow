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
#include "keyloggerosd.h"
#include "keyloggerlabel.h"
#include "src/keymonmanager.h"

// KDE
#include <plasma/theme.h>
#include <kwindowsystem.h>

// Qt
#include <QtGui/QKeyEvent>
#include <QtCore/QTimer>
#include <QtCore/QPropertyAnimation>
#include <QtGui/QLineEdit>
#include <QtGui/QHBoxLayout>
#include <QtGui/QX11Info>
#include <QtGui/QClipboard>
#include <QtGui/QApplication>


namespace RecordItNow {



KeyloggerOSD::KeyloggerOSD(QWidget *parent)
    : RecordItNow::OSD(parent)
{

    m_inactive = true;

    m_animation = new QPropertyAnimation(this);
    m_animation->setPropertyName("windowOpacity");
    m_animation->setTargetObject(this);
    m_animation->setEasingCurve(QEasingCurve::Linear);
    m_animation->setDuration(300);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);

    m_edit = new KeyloggerLabel(this);
    m_edit->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(m_edit);
    setLayout(layout);

    m_timer = new QTimer(this);
    m_timer->setInterval(1000/2);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(updateMousePos()));

    m_hideTimer = new QTimer(this);
    m_hideTimer->setSingleShot(true);
    m_hideTimer->setInterval(5*1000); // hide after 5 sec
    connect(m_hideTimer, SIGNAL(timeout()), this, SLOT(inactive()));

    m_timer->start();

    connect(KeyMonManager::self(), SIGNAL(keyEvent(RecordItNow::KeyloggerEvent)), this,
            SLOT(keyloggerEvent(RecordItNow::KeyloggerEvent)));

    connect(qApp->clipboard(), SIGNAL(dataChanged()), this, SLOT(clipboardDataChanged()));

    setBackgroundImage("widgets/tooltip");
    setBlurEnabled(true);
    setTransparentForMouseEvents(true);

}


KeyloggerOSD::~KeyloggerOSD()
{

    delete m_hideTimer;
    delete m_timer;
    delete m_animation;

}


void KeyloggerOSD::init(const int &timeout, const bool &shortcuts, const bool &clipboard)
{

    m_hideTimer->setInterval(timeout*1000);
    m_edit->setShowShortcuts(shortcuts);
    m_edit->setShowClipbaord(clipboard);

}


void KeyloggerOSD::updateMousePos()
{

    if (m_inactive) {
        return;
    }

    if (geometry().contains(QCursor::pos())) {
        if (!KWindowSystem::self()->compositingActive()) {
            hide();
        } else {
            if (m_animation->state() == QPropertyAnimation::Running) {
                if (m_animation->endValue().toInt() == 1) {
                    m_animation->stop();
                } else {
                    return;
                }
            }
            m_animation->setStartValue(windowOpacity());
            m_animation->setEndValue(0.1);

            m_animation->start();
        }
    } else {
        if (!KWindowSystem::self()->compositingActive()) {
            show();
        } else {
            if (m_animation->state() == QPropertyAnimation::Running) {
                if (m_animation->endValue().toInt() != 1) {
                    m_animation->stop();
                } else {
                    return;
                }
            }
            m_animation->setStartValue(windowOpacity());
            m_animation->setEndValue(1.0);

            m_animation->start();
        }
    }

}


void KeyloggerOSD::inactive()
{

    m_inactive = true;
    hide();
    m_edit->clear();

}


void KeyloggerOSD::keyloggerEvent(const RecordItNow::KeyloggerEvent &event)
{

    if (event.type() != RecordItNow::KeyloggerEvent::KeyboardEvent) {
        return;
    }

    QKeyEvent qKeyEvent(event.pressed() ? (QEvent::Type) 6 : (QEvent::Type) 7,
                        event.key(),
                        event.modifiers(),
                        event.text(),
                        false,
                        event.count());

    qApp->sendEvent(this, &qKeyEvent);

}


void KeyloggerOSD::clipboardDataChanged()
{

    QClipboard *clip = static_cast<QClipboard*>(sender());
    QString txt = clip->text();

    if (!txt.isEmpty()) {
        m_edit->setClipboardText(txt);

        m_inactive = false;

        if (m_hideTimer->isActive()) {
            m_hideTimer->stop();
        }

        if (!isVisible()) {
            show();
            updateMousePos();
        }

        m_hideTimer->start();
    }

}


void KeyloggerOSD::keyPressEvent(QKeyEvent *event)
{

    m_inactive = false;

    if (m_hideTimer->isActive()) {
        m_hideTimer->stop();
    }

    if (!isVisible()) {
        show();
        updateMousePos();
    }
    m_edit->pressEvent(event);

}


void KeyloggerOSD::keyReleaseEvent(QKeyEvent *event)
{

    m_inactive = false;

    m_edit->releaseEvent(event);
    if (m_hideTimer->isActive()) {
        m_hideTimer->stop();
    }
    m_hideTimer->start();

}


} // namespace RecordItNow


#include "keyloggerosd.moc"
