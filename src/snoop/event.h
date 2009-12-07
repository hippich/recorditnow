/***************************************************************************
 *   Copyright (C) 2009 by Kai Dombrowe <just89@gmx.de>                    *
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

#ifndef EVENT_H
#define EVENT_H


// KDE
#include <kdemacros.h>

// Qt
#include <QtCore/QString>


namespace SNoop {


class KDE_EXPORT Event
{


public:
    enum Key {
        LeftButton = 0,
        RightButton = 1,
        MiddleButton = 3,
        SpecialButton1 = 4,
        SpecialButton2 = 5,
        WheelUp = 6,
        WheelDown = 7,
        NoButton = -1
    };
    Event();
    Event(const SNoop::Event &other);
    ~Event();

    Key key;
    bool pressed;


    static QString name(const SNoop::Event::Key &key);
    static SNoop::Event::Key keyFromName(const QString &name);
    static SNoop::Event::Key xButtonToKey(const int &button);
    static int keyToXButton(const SNoop::Event::Key &key);


};


}; // Namespace SNoop


#endif // EVENT_H
