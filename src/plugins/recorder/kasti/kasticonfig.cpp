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
#include "kasticonfig.h"
#include <recorditnow_kasti.h>

// KDE
#include <kpluginfactory.h>
#include <kconfiggroup.h>


K_PLUGIN_FACTORY(ConfigFactory, registerPlugin<KastiConfig>();)
K_EXPORT_PLUGIN(ConfigFactory("recorditnow_kasti_config"))
KastiConfig::KastiConfig(QWidget *parent, const QVariantList &args)
    : KCModule( ConfigFactory::componentData(), parent, args)
{

    setupUi(this);
    addConfig(Settings::self(), this);

}


KastiConfig::~KastiConfig()
{


}


void KastiConfig::load()
{

    KCModule::load();

}


void KastiConfig::save()
{

    KCModule::save();
    Settings::self()->writeConfig();

}


void KastiConfig::defaults()
{

    KCModule::defaults();

}


#include "kasticonfig.moc"
