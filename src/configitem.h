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

#ifndef CONFIGITEM_H
#define CONFIGITEM_H


// Qt
#include <QtCore/QDataStream>
#include <QtCore/QExplicitlySharedDataPointer>
#include <QtCore/QHash>
#include <QtCore/QVariant>
#include <QtCore/QMetaType>


namespace RecordItNow {


class ConfigItemPrivate;
class ConfigItem
{


public:
    explicit ConfigItem();
    ConfigItem(const ConfigItem &copy);
    ~ConfigItem();

    ConfigItem &operator=(const ConfigItem &rhs);
    bool operator==(const ConfigItem &rhs) const;
    bool operator!=(const ConfigItem &rhs) const;
    bool operator<(const ConfigItem &rhs) const;
    bool operator>(const ConfigItem &rhs) const;
    QDataStream &operator>>(QDataStream &stream);
    QDataStream &operator<<(QDataStream &stream) const;


private:
     QExplicitlySharedDataPointer<ConfigItemPrivate> d;

    QVariant data(const QString &key) const;
    void setData(const QString &key, const QVariant &value);


protected:
    template <typename T>
    T data(const QString &key) const
    {

        return data(key).value<T>();

    }
    template <typename T>
    void setData(const QString &key, const T &value)
    {

        setData(key, QVariant::fromValue<T>(value));

    }


};


} // namespace RecordItNow


Q_DECLARE_METATYPE(RecordItNow::ConfigItem)


QDataStream &operator<<(QDataStream &stream, const RecordItNow::ConfigItem &data);
QDataStream &operator>>(QDataStream &stream, RecordItNow::ConfigItem &data);


#endif // CONFIGITEM_H
