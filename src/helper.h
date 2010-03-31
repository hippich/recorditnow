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

#ifndef HELPER_H
#define HELPER_H

// KDE
#include <kglobal.h>

// Qt
#include <QtCore/QList>
#include <QtCore/QDataStream>


namespace RecordItNow {


class Helper
{


public:
    template <typename T>
    static QByteArray listToArray(const QList<T> &list)
    {

        QByteArray array;
        QDataStream stream(&array, QIODevice::WriteOnly);

        stream << list;

        return array;

    }
    template <typename T>
    static QList<T> listFromArray(const QByteArray &array)
    {

        QDataStream stream(array);

        QList<T> list;
        stream >> list;

        return list;

    }

    static Helper *self();

    bool firstStart() const;


private:
    friend class HelperSingleton;
    Helper();

    bool m_firstStart;


};


} // namespace RecordItNow


#endif // HELPER_H
