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

#ifndef RECORDITNOW_CONFIGSCRIPTADAPTOR_H
#define RECORDITNOW_CONFIGSCRIPTADAPTOR_H


// Qt
#include <QtCore/QObject>
#include <QtCore/QVariant>


class KConfigGroup;
namespace RecordItNow {


class ConfigScriptAdaptor : public QObject
{
    Q_OBJECT


public:
    explicit ConfigScriptAdaptor(const QString &script, QObject *parent = 0);
    ~ConfigScriptAdaptor();


public slots:
    QVariant readConfig(const QString &key, const QVariant &defaultValue) const;
    QString readConfig(const QString &key, const QString &defaultValue) const;

    void writeConfig(const QString &key, const QVariant &value);
    void writeConfig(const QString &key, const QString &value);

    void deleteConfig(const QString &key);


private:
    KConfigGroup *m_config;


};


} // namespace RecordItNow


#endif // RECORDITNOW_CONFIGSCRIPTADAPTOR_H
