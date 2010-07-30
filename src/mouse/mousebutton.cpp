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


namespace RecordItNow {


MouseButton::MouseButton(const int &code, const QColor &color, const QString &sound)
    : RecordItNow::ConfigItem()
{

    setCode(code);
    setColor(color);
    setSound(sound);

}


MouseButton::MouseButton()
    : RecordItNow::ConfigItem()
{

    setCode(-1);
    setColor(Qt::color0);
    setSound(QString());

}


bool MouseButton::isValid() const
{

    return code() != -1 && color() != Qt::color0;

}


int MouseButton::code() const
{

    return data<int>("Code");

}


QColor MouseButton::color() const
{

    return data<QColor>("Color");

}


QString MouseButton::sound() const
{

    return data<QString>("Sound");    
    
}


void MouseButton::setCode(const int &code)
{

    setData("Code", code);

}


void MouseButton::setColor(const QColor &color)
{

    setData("Color", color);

}


void MouseButton::setSound(const QString& sound)
{

    setData("Sound", sound);    
    
}


} // namespace RecordItNow


