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

#ifndef KEYLOGGEREVENT_H
#define KEYLOGGEREVENT_H


// KDE
#include <kdemacros.h>

// Qt
#include <QtCore/QExplicitlySharedDataPointer>
#include <QtCore/QMetaType>


namespace RecordItNow {


class KeyloggerEventPrivate;
class KDE_EXPORT KeyloggerEvent
{


public:
    enum EventType {
        MouseEvent = 0,
        KeyboardEvent = 1
    };
    enum MouseButton {
        LeftButton = 0,
        RightButton = 1,
        MiddleButton = 3,
        SpecialButton1 = 4,
        SpecialButton2 = 5,
        WheelUp = 6,
        WheelDown = 7,
        NoButton = -1
    };

    KeyloggerEvent();
    KeyloggerEvent(const RecordItNow::KeyloggerEvent &copy);

    ~KeyloggerEvent();

    QString text() const;
    int id() const;
    bool pressed() const;
    KeyloggerEvent::EventType type() const;
    RecordItNow::KeyloggerEvent::MouseButton idToMouseButton() const;


    void setText(const QString &text);
    void setId(const int &id);
    void setPressed(const bool &pressed);
    void setType(const KeyloggerEvent::EventType &type);

    static int keyToXButton(const RecordItNow::KeyloggerEvent::MouseButton &key);


private:
    KeyloggerEventPrivate *d;


};


} // namespace RecordItNow


Q_DECLARE_METATYPE(RecordItNow::KeyloggerEvent)


#endif // KEYLOGGEREVENT_H
