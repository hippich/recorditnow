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
#include "configpage.h"

// KDE
#include <kconfig.h>



namespace RecordItNow {


ConfigPage::ConfigPage(KConfig *cfg, QWidget *parent)
    : QWidget(parent), m_config(cfg)
{

    m_settingsChanged = false;
    Q_ASSERT(m_config);

}


ConfigPage::~ConfigPage()
{



}


bool ConfigPage::hasChanged()
{

    return m_settingsChanged;

}


void ConfigPage::save()
{

    if (hasChanged()) {
        saveConfig();
        m_settingsChanged = false;
    }

}


void ConfigPage::defaults()
{

    setDefaults();
    m_settingsChanged = true;

}


void ConfigPage::load()
{

    loadConfig();
    m_settingsChanged = false;

}


KConfig *ConfigPage::config() const
{

    return m_config;

}


void ConfigPage::setConfigChanged(const bool &changed)
{

    m_settingsChanged = changed;
    emit settingsChanged();

}


} // namespace RecordItNow


#include "configpage.moc"
