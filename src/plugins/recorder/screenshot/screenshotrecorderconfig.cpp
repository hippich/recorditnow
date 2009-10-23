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


// own
#include "screenshotrecorderconfig.h"
#include <screenshot.h>

// KDE
#include <kpluginfactory.h>
#include <kaboutdata.h>


K_PLUGIN_FACTORY(ConfigFactory, registerPlugin<ScreenshotRecorderConfig>();)
K_EXPORT_PLUGIN(ConfigFactory("recorditnow_screenshot_config"))
ScreenshotRecorderConfig::ScreenshotRecorderConfig(QWidget *parent, const QVariantList &args)
    : KCModule( ConfigFactory::componentData(), parent, args)
{

    KAboutData *about = new KAboutData("ScreenshotConfig",
                                       0,
                                       ki18n("Screenshot Configuration"),
                                       KDE_VERSION_STRING,
                                       KLocalizedString(),
                                       KAboutData::License_GPL,
                                       ki18n( "Copyright 2009 Kai Dombrowe" ) );
    setAboutData(about);

    ui_cfg.setupUi(this);
    addConfig(Settings::self(), this);

}


ScreenshotRecorderConfig::~ScreenshotRecorderConfig()
{


}