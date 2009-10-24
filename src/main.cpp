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
#include "mainwindow.h"

// KDE
#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>


int main(int argc, char *argv[])
{

    KAboutData about("recorditnow",
                     0,
                     ki18n("RecordItNow"),
                     "0.1",
                     ki18n("Desktop recorder GUI"),
                     KAboutData::License_GPL,
                     ki18n("(C) 2009 Kai Dombrowe"),
                     KLocalizedString(),
                     0,
                     "just89@gmx.de");
    about.setProgramIconName("video-display");

    KCmdLineArgs::init(argc, argv, &about);
    KCmdLineOptions options;
    KCmdLineArgs::addCmdLineOptions(options);

    KApplication app;

    MainWindow *window = new MainWindow;

    // see if we are starting with session management
    if (app.isSessionRestored())
    {
        RESTORE(MainWindow);
    }
    else
    {
        // no session.. just start up normally
        KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
        if (args->count() == 0)
        {
            window->show();
        }
        else
        {
            int i = 0;
            for (; i < args->count(); i++)
            {
                window->show();
            }
        }
        args->clear();
    }

    return app.exec();

}
