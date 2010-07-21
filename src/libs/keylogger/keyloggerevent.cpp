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

// X
#include <X11/Xlib.h>


namespace RecordItNow {


KeyloggerEventPrivate::KeyloggerEventPrivate(KeyloggerEvent *parent)
    : q(parent)
{

    qRegisterMetaType<RecordItNow::KeyloggerEvent>("RecordItNow::KeyloggerEvent");
    id = -1;

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


RecordItNow::KeyloggerEvent::MouseButton KeyloggerEvent::idToMouseButton() const
{

    switch (id()) {
    case 0: return LeftButton;
    case 1: return RightButton;
    case 3: return MiddleButton;
    case 4: return SpecialButton1;
    case 5: return SpecialButton2;
    case 6: return WheelUp;
    case 7: return WheelDown;
    default: return NoButton;
    }

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


int KeyloggerEvent::keyToXButton(const RecordItNow::KeyloggerEvent::MouseButton &key)
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




} // namespace RecordItNow
