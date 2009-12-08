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
#include "device.h"

// KDE
#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <kdebug.h>



int main(int argc, char *argv[])
{

    KAboutData about("recorditnow_fix_permissions",
                     0,
                     ki18n("recorditnow_fix_permissions"),
                     "0.1",
                     ki18n(""),
                     KAboutData::License_GPL,
                     ki18n("(C) 2009 Kai Dombrowe"),
                     KLocalizedString(),
                     0,
                     "just89@gmx.de");
    about.setProgramIconName("recorditnow");
    about.setHomepage("http://recorditnow.sourceforge.net/index.html");

    KCmdLineArgs::init(argc, argv, &about);

    KCmdLineOptions options;
    options.add("device <device>", ki18n("Path to device"));
    KCmdLineArgs::addCmdLineOptions(options);

    KApplication app;
    KCmdLineArgs *parsed = KCmdLineArgs::parsedArgs();

    QString device;
    if (parsed->isSet("device")) {
        device = parsed->getOption("device");
    }
    parsed->clear();

    if (device.isEmpty()) {
        return Device::ERR_NOSUCHFILE;
    }

    Device *dev = new Device(device);
    Q_UNUSED(dev);

    return app.exec();

}
