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
#include "keyloggerevent.h"
#include "keyloggerevent_p.h"

// KDE
#include <kdebug.h>

// X
#include <X11/Xlib.h>



namespace RecordItNow {


KeyloggerEventPrivate::KeyloggerEventPrivate(KeyloggerEvent *parent)
    : q(parent)
{

    qRegisterMetaType<RecordItNow::KeyloggerEvent>("RecordItNow::KeyloggerEvent");
    id = -1;
    modifiers = Qt::NoModifier;
    keyCount = 1;

}


KeyloggerEventPrivate::~KeyloggerEventPrivate()
{


}



KeyloggerEvent::KeyloggerEvent()
    : d(new KeyloggerEventPrivate(this))
{


}


KeyloggerEvent::KeyloggerEvent(const RecordItNow::KeyloggerEvent &copy)
    : d(copy.d)
{


}


KeyloggerEvent::~KeyloggerEvent()
{


}


QString KeyloggerEvent::text() const
{

    return d->text;

}


int KeyloggerEvent::id() const
{

    return d->id;

}


bool KeyloggerEvent::pressed() const
{

    return d->pressed;

}


KeyloggerEvent::EventType KeyloggerEvent::type() const
{

    return d->type;

}


RecordItNow::KeyloggerEvent::MouseButton KeyloggerEvent::idToButton() const
{

    switch (id()) {
    case Button1: return LeftButton;
    case Button3: return RightButton;
    case Button2: return MiddleButton;
    case 8: return SpecialButton1;
    case 9: return SpecialButton2;
    case Button4: return WheelUp;
    case Button5: return WheelDown;
    default: return NoButton;
    }

}


Qt::Key KeyloggerEvent::key() const
{

    return d->qKey;

}


bool KeyloggerEvent::operator==(const RecordItNow::KeyloggerEvent &other) const
{

    return d->id == other.id();

}


bool KeyloggerEvent::operator!=(const RecordItNow::KeyloggerEvent &other) const
{

    return d->id != other.id();

}


Qt::KeyboardModifiers KeyloggerEvent::modifiers() const
{

    return d->modifiers;

}


int KeyloggerEvent::count() const
{

    return d->keyCount;

}


void KeyloggerEvent::setText(const QString &text)
{

    d->text = text;

}


void KeyloggerEvent::setId(const int &id)
{

    d->id = id;

}


void KeyloggerEvent::setPressed(const bool &pressed)
{

    d->pressed = pressed;

}


void KeyloggerEvent::setType(const KeyloggerEvent::EventType &type)
{

    d->type = type;

}


int KeyloggerEvent::buttonToXButton(const RecordItNow::KeyloggerEvent::MouseButton &key)
{

    switch (key) {
    case LeftButton: return Button1;
    case RightButton: return Button3;
    case MiddleButton: return Button2;
    case SpecialButton1: return 8;
    case SpecialButton2: return 9;
    case WheelUp: return Button4;
    case WheelDown: return Button5;
    default: return -1;
    }

}


void KeyloggerEvent::setKey(const Qt::Key &key)
{

    d->qKey = key;

}


void KeyloggerEvent::setModifiers(const Qt::KeyboardModifiers &modifiers)
{

    d->modifiers = modifiers;

}


void KeyloggerEvent::setCount(const int &count)
{

    d->keyCount = count;

}


} // namespace RecordItNow
