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

#ifndef MOUSEBUTTON_H
#define MOUSEBUTTON_H


// Qt
#include <QtGui/QColor>


class MouseButton
{


public:
    MouseButton(const int &code = -1, const QColor &color = Qt::black);


    bool isValid() const;
    int code() const;
    QColor color() const;

    void setCode(const int &code);
    void setColor(const QColor &color);

    bool operator==(const MouseButton &other) const;


private:
    int m_code;
    QColor m_color;


};


QDataStream &operator<<(QDataStream &stream, const MouseButton &data);
QDataStream &operator>>(QDataStream &stream, MouseButton &data);


#endif // MOUSEBUTTON_H
