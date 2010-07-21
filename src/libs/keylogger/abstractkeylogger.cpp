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
#include "abstractkeylogger.h"
#include "abstractkeylogger_p.h"

// KDE
#include <kdebug.h>


namespace RecordItNow {


AbstractKeyloggerPrivate::AbstractKeyloggerPrivate(AbstractKeylogger *parent)
    : QObject(parent), q(parent)
{

    connect(q, SIGNAL(keyEvent(RecordItNow::KeyloggerEvent)), this,
            SLOT(logKeys(RecordItNow::KeyloggerEvent)));

}


AbstractKeyloggerPrivate::~AbstractKeyloggerPrivate()
{


}


void AbstractKeyloggerPrivate::logKeys(const RecordItNow::KeyloggerEvent &event)
{


    if (event.type() != KeyloggerEvent::KeyboardEvent) {
        return;
    }

    if ((!event.isKeyboardModifier() && m_keys.isEmpty()) ||
        (!m_keys.isEmpty() && !m_keys.at(0).isKeyboardModifier())) {
        // TODO: log text
        return;
    }

    if (event.pressed()) {
        if (!m_keys.contains(event)) {
            m_keys.append(event);
        }
    } else {
        m_keys.removeAll(event);
    }

    QString string;
    foreach (const RecordItNow::KeyloggerEvent &key, m_keys) {
        if (!string.isEmpty()) {
            string.append("+");
        }
        string.append(key.text());
    }
    kDebug() << string;

    emit pressedKeysChanged(m_keys);

}



AbstractKeylogger::AbstractKeylogger(QObject *parent)
    : QObject(parent), d(new AbstractKeyloggerPrivate(this))
{

    connect(d, SIGNAL(pressedKeysChanged(QList<RecordItNow::KeyloggerEvent>)), this,
            SIGNAL(pressedKeysChanged(QList<RecordItNow::KeyloggerEvent>)));

}


AbstractKeylogger::~AbstractKeylogger()
{

    delete d;

}


QWidget *AbstractKeylogger::getKeyboardConfigWidget(QWidget *parent, const KConfig *)
{

    Q_UNUSED(parent);

    return 0;

}


QWidget *AbstractKeylogger::getMouseConfigWidget(QWidget *parent, const KConfig *)
{

    Q_UNUSED(parent);

    return 0;

}


bool AbstractKeylogger::hasConfigChanged(const KConfig *cfg)
{

    Q_UNUSED(cfg);
    return false;

}


} // namespace RecordItNow


#include "abstractkeylogger.moc"
#include "abstractkeylogger_p.moc"
