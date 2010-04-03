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
#include "configitem.h"

// KDE
#include <kdebug.h>


namespace RecordItNow {


class ConfigItemPrivate : public QSharedData
{


public:
    ConfigItemPrivate() {}
    ~ConfigItemPrivate() {}

    QHash<QString, QVariant> data;


};



ConfigItem::ConfigItem()
    : d(new ConfigItemPrivate)

{


}


ConfigItem::ConfigItem(const ConfigItem &copy)
    : d(copy.d)
{


}


ConfigItem::~ConfigItem()
{



}


ConfigItem &ConfigItem::operator=(const ConfigItem &rhs)
{

    d = rhs.d;
    return *this;

}


bool ConfigItem::operator==(const ConfigItem &rhs) const
{

    return d.data()->data == rhs.d.data()->data;

}


bool ConfigItem::operator!=(const ConfigItem &rhs) const
{

    return d.data()->data != rhs.d.data()->data;

}


bool ConfigItem::operator<(const ConfigItem &rhs) const
{

    return d < rhs.d;

}


bool ConfigItem::operator>(const ConfigItem &rhs) const
{

    return d > rhs.d;

}


QDataStream &ConfigItem::operator>>(QDataStream &stream)
{

    stream >> d.data()->data;
    return stream;

}


QDataStream &ConfigItem::operator<<(QDataStream &stream) const
{

    stream << d.data()->data;
    return stream;

}


QVariant ConfigItem::data(const QString &key) const
{

    return d.data()->data.value(key);

}


void ConfigItem::setData(const QString &key, const QVariant &value)
{

    d.data()->data.insert(key, value);

}


} // namespace RecordItNow


QDataStream &operator<<(QDataStream &stream, const RecordItNow::ConfigItem &data)
{

    data << stream;
    return stream;

}


QDataStream &operator>>(QDataStream &stream, RecordItNow::ConfigItem &data)
{

    data >> stream;
    return stream;

}


