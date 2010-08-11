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
#include "configscriptadaptor.h"
#include <recorditnow.h>

// KDE
#include <kconfiggroup.h>


namespace RecordItNow {


ConfigScriptAdaptor::ConfigScriptAdaptor(const QString &script, QObject *parent)
    : QObject(parent)
{

    m_config = new KConfigGroup(Settings::self()->config(), "RecordItNow-Script-"+script);

}


ConfigScriptAdaptor::~ConfigScriptAdaptor()
{

    delete m_config;

}


QVariant ConfigScriptAdaptor::readConfig(const QString &key, const QVariant &defaultValue) const
{

    return m_config->readEntry(key, defaultValue);

}


QString ConfigScriptAdaptor::readConfig(const QString &key, const QString &defaultValue) const
{

    return m_config->readEntry(key, defaultValue);

}


void ConfigScriptAdaptor::writeConfig(const QString &key, const QVariant &value)
{

    m_config->writeEntry(key, value);

}


void ConfigScriptAdaptor::writeConfig(const QString &key, const QString &value)
{

    m_config->writeEntry(key, value);

}


void ConfigScriptAdaptor::deleteConfig(const QString &key)
{

    m_config->deleteEntry(key);

}


} // namespace RecordItNow


#include "configscriptadaptor.moc"
