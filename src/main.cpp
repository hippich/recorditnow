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
#include "application.h"
#include "mainwindow.h"
#include <recorditnow.h>
#include "firstStart/firststartassistant.h"
#include "helper.h"

// KDE
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <kdebug.h>


#define RECORDITNOW_VERSION 0.9
static void checkVersion()
{

    if (RECORDITNOW_VERSION != Settings::version()) {
        printf("RecordItNow update:\nOld version: %f.\nNew version: %f.\n\n",
               Settings::version(), RECORDITNOW_VERSION);

        printf("\n");
        printf("Update done...\n\n");

        Settings::setVersion(RECORDITNOW_VERSION);
        Settings::self()->writeConfig();
    }

}


void startRecordItNow(KCmdLineArgs *parsed = 0)
{

    checkVersion();

    bool hasArgs = false;
    QRect geometry = QRect(-1, -1, -1, -1);
    QString backend;
    QString file;
    int time = 0;
    bool hide = false;

    if (parsed) {
        if (parsed->isSet("x")) {
            hasArgs = true;
            geometry.setX(parsed->getOption("x").toInt());
        }
        if (parsed->isSet("y")) {
            hasArgs = true;
            geometry.setY(parsed->getOption("y").toInt());
        }
        if (parsed->isSet("width")) {
            hasArgs = true;
            geometry.setWidth(parsed->getOption("width").toInt());
        }
        if (parsed->isSet("height")) {
            hasArgs = true;
            geometry.setHeight(parsed->getOption("height").toInt());
        }
        if (parsed->isSet("backend")) {
            hasArgs = true;
            backend = parsed->getOption("backend");
        }
        if (parsed->isSet("timer")) {
            hasArgs = true;
            time = parsed->getOption("timer").toInt();
        }
        if (parsed->isSet("o")) {
            hasArgs = true;
            file = parsed->getOption("o");
        }
        if (parsed->isSet("hide")) {
            hide = true;
        }
        parsed->clear();
    }

    if (!hasArgs && Settings::showFirstStartAssistant()) {
        RecordItNow::FirstStartAssistant *assistant = new RecordItNow::FirstStartAssistant;
        assistant->show();
    } else {
        RecordItNow::MainWindow *window = new RecordItNow::MainWindow;
        if (!hide) {
            window->show();
        }

        if (hasArgs) {
            window->startWithArgs(backend, file, time, geometry);
        }
    }

}


int main(int argc, char *argv[])
{

    KAboutData about("recorditnow",
                     0,
                     ki18n("RecordItNow"),
                     QString::number(RECORDITNOW_VERSION).toLatin1(),
                     ki18n("Plugin based desktop recorder"),
                     KAboutData::License_GPL,
                     ki18n("(C) 2009-2010 Kai Dombrowe"),
                     KLocalizedString(),
                     0,
                     "just89@gmx.de");
    about.setProgramIconName("recorditnow");
    about.setHomepage("http://recorditnow.sourceforge.net/index.html");

    KCmdLineArgs::init(argc, argv, &about);

    KCmdLineOptions options;
    options.add("x <offset>", ki18n("Offset in x direction."));
    options.add("y <offset>", ki18n("Offset in y direction."));
    options.add("width <width>", ki18n("Width of recorded window."));
    options.add("height <height>", ki18n("Height of recorded window."));
    options.add("backend <backend>", ki18n("The Backend to use. (Example: RecordMyDesktop/Screenshot)"));
    options.add("timer <time>", ki18n("Wait \"time\" seconds."));
    options.add("o filename", ki18n("Name of recorded video/image."));
    options.add("hide", ki18n("Start hidden."));
    KCmdLineArgs::addCmdLineOptions(options);

    Application app;

    if (app.isSessionRestored()) { // see if we are starting with session management
        RESTORE(RecordItNow::MainWindow);
    } else { // no session.. just start up normally        
        startRecordItNow(KCmdLineArgs::parsedArgs());
    }

    return app.exec();

}
