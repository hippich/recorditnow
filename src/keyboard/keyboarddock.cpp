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
#include "keyboarddock.h"
#include "keywidget.h"
#include "../keymonmanager.h"
#include "flowlayout.h"
#include <recorditnow.h>

// KDE
#include <kauth.h>
#include <kdebug.h>

// Qt
#include <QtCore/QFile>
#include <QtCore/QTimer>
#include <QtGui/QMainWindow>
#include <QtGui/QSlider>


KeyboardDock::KeyboardDock(QWidget *parent)
    : RecordItNow::DockWidget(parent)
{

    setObjectName("Keyboard");
    setDockTitle(i18n("Keyboard-Monitor"));
    setWindowIcon(KIcon("input-keyboard"));
    setAllowedAreas(Qt::AllDockWidgetAreas);
    setFeatures(QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetFloatable);

    QWidget *content = new QWidget;
    setupUi(content);

    connect(KeyMonManager::self(), SIGNAL(keyEvent(RecordItNow::KeyloggerEvent)), this,
            SLOT(keyPressed(RecordItNow::KeyloggerEvent)));

    m_layout = new FlowLayout;
    mainLayout->addLayout(m_layout);

    QWidget *title = new QWidget(this);
    ui_title.setupUi(title);

    ui_title.sizeSlider->setValue(Settings::keySize());
    connect(ui_title.sizeSlider, SIGNAL(valueChanged(int)), this, SLOT(sizeChanged(int)));

    setTitleBarWidget(title);

    sizeChanged(Settings::keySize());


    setWidget(content);

}


KeyboardDock::~KeyboardDock()
{

    
}


void KeyboardDock::init(const QList<KeyboardKey> &map)
{

    m_layout->clear();
    m_keyList.clear();

    foreach (const KeyboardKey &key, map) {
        KeyWidget *widget = new KeyWidget(key, this);
        m_keyList.append(widget);
        m_layout->addWidget(widget);
    }

}


void KeyboardDock::keyPressed(const RecordItNow::KeyloggerEvent &event)
{

    if (event.type() != RecordItNow::KeyloggerEvent::KeyboardEvent) {
        return;
    }

    foreach (KeyWidget * widget, m_keyList) {
        if (widget->keyCode() == event.id()) {
            kDebug() << "Event:" << "Code:" << event.id() << "Pressed?" << event.pressed();
            widget->setPressed(event.pressed());
            break;
        }
    }
   // QKeyEvent k(event.pressed() ? (QEvent::Type)6 : (QEvent::Type)7, event.key(), event.modifiers(), event.text(), false, event.count());
  //  qApp->sendEvent(m_edit, &k);

}


void KeyboardDock::sizeChanged(const int &value)
{

    m_layout->setItemHeight(value);
    Settings::self()->setKeySize(value);
    scrollAreaWidgetContents->adjustSize();

}


#include "keyboarddock.moc"
