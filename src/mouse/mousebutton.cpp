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
#include "mousebutton.h"


MouseButton::MouseButton(const int &code, const QColor &color)
    : m_code(code), m_color(color)
{


}


bool MouseButton::isValid() const
{

    return code() != -1;

}


int MouseButton::code() const
{

    return m_code;

}


QColor MouseButton::color() const
{

    return m_color;

}


QByteArray MouseButton::toArray() const
{

    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);

    stream << code();
    stream << color();

    return data;

}


void MouseButton::setCode(const int &code)
{

    m_code = code;

}


void MouseButton::setColor(const QColor &color)
{

    m_color = color;

}


MouseButton MouseButton::fromArray(const QByteArray &array)
{

    QDataStream stream(array);

    int code;
    QColor color;

    stream >> code;
    stream >> color;

    return MouseButton(code, color);

}


QList<MouseButton> MouseButton::listFromArray(const QByteArray &array)
{

    QList<MouseButton> list;
    QDataStream stream(array);

    int size;
    stream >> size;

    for (int i = 0; i < size; i++) {

        QByteArray data;
        stream >> data;

        list.append(MouseButton::fromArray(data));
    }

    return list;

}


QByteArray MouseButton::listToArray(const QList<MouseButton> &list)
{

    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);

    stream << list.size();
    foreach (const MouseButton &button, list) {
        stream << button.toArray();
    }

    return data;

}


bool MouseButton::operator==(const MouseButton &other) const
{

    return (other.code() == code() && other.color() == color());

}
