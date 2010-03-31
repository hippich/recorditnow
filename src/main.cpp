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
#include "keyboard/keyboardkey.h"
#include "config/keyboardconfig.h"
#include "config/mouseconfig.h"
#include "config/frameconfig.h"
#include "timeline/topic.h"
#include "config/timelineconfig.h"

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


        if (Settings::version() < 0.9) {
            printf("keyMonDevice >> mouseDevice\n");

            KConfigGroup cfg(Settings::self()->config(), "Mouse");
            Settings::setMouseDevice(cfg.readEntry("keyMonDevice", QString()));
            cfg.deleteEntry("keyMonDevice");


            printf("Convert old keyboard configuration...\n");

            KConfigGroup keyGroup(Settings::self()->config(), "Keyboard");
            int count = keyGroup.readEntry("Keys", 0);
            QList<KeyboardKey> keyMap;

            for (int i = 0; i < count; i++) {
                const QString codeKey(QString("Key %1 Code").arg(i));
                const QString iconKey(QString("Key %1 Icon").arg(i));
                const QString textKey(QString("Key %1 Text").arg(i));

                const int key = keyGroup.readEntry(codeKey, -1);
                const QString icon = keyGroup.readEntry(iconKey, QString());
                const QString text = keyGroup.readEntry(textKey, QString());

                printf("Save Key: %d\n", key);

                keyMap.append(KeyboardKey(key, icon, text));
            }
            KeyboardConfig::saveConfig(keyMap, Settings::self()->config());

            printf("Delete old keyboard entrys...\n");

            int index = 0;
            QString key = QString("Key %1 Code").arg(index);
            while (keyGroup.hasKey(key)) {
                keyGroup.deleteEntry(key);
                keyGroup.deleteEntry(QString("Key %1 Icon").arg(index));
                keyGroup.deleteEntry(QString("Key %1 Text").arg(index));

                index++;
                key = QString("Key %1 Code").arg(index);
            }


            printf("Convert old mouse configuration...\n");

            QList<MouseButton> buttons;
            KConfigGroup mouseGroup(Settings::self()->config(), "Mouse");

            int keys = mouseGroup.readEntry("Buttons", 0);
            for (int i = 0; i < keys; i++) {
                const int key = mouseGroup.readEntry(QString("Button %1 key").arg(i), 0);
                const QColor color = mouseGroup.readEntry(QString("Button %1 color").arg(i), QColor());

                printf("Save Button: %d\n", key);

                buttons.append(MouseButton(key, color));
            }
            MouseConfig::saveConfig(Settings::self()->config(), buttons);

            printf("Delete old mouse entrys...\n");

            index = 0;
            key = QString("Button %1 key").arg(index);
            while (keyGroup.hasKey(key)) {
                keyGroup.deleteEntry(key);
                keyGroup.deleteEntry(QString("Button %1 color").arg(index));

                index++;
                key = QString("Button %1 key").arg(index);
            }

            printf("Convert old frame configuration...\n");

            KConfigGroup frameCfg(Settings::self()->config(), "Frame");

            QList<FrameSize> frames;
            foreach (const QString &name, frameCfg.readEntry("Names", QStringList())) {
                FrameSize size;

                size.setText(name);
                size.setSize(frameCfg.readEntry(QString("Size %1").arg(name), QSize()));

                printf("Save Frame: %s\n", size.text().toLatin1().constData());

                frames.append(size);
            }
            FrameConfig::writeSizes(frames, Settings::self()->config());

            printf("Delete old frame entrys...\n");
            foreach (const QString &name, frameCfg.readEntry("Names", QStringList())) {
                frameCfg.deleteEntry(QString("Size %1").arg(name));
            }
            frameCfg.deleteEntry("Names");

            printf("Convert old timeline configuration...\n");

            QList<RecordItNow::Timeline::Topic> topics;
            KConfigGroup timelineCfg(Settings::self()->config(), "Timeline");
            count = timelineCfg.readEntry("Topics", 0);
            for (int i = 0; i < count; i++) {
                const QString title = timelineCfg.readEntry(QString("Topic %1 Title").arg(i), i18n("Untitled"));
                const QString icon = timelineCfg.readEntry(QString("Topic %1 Icon").arg(i), "dialog-information");
                const unsigned long duration = timelineCfg.readEntry(QString("Topic %1 Duration").arg(i), 10);

                printf("Save Topic: %s\n", title.toLatin1().constData());

                RecordItNow::Timeline::Topic topic;
                topic.setDuration(RecordItNow::Timeline::Topic::secondsToTime(duration));
                topic.setIcon(icon);
                topic.setTitle(title);

                topics.append(topic);
            }

            TimelineConfig::saveTopics(topics, Settings::self()->config());

            printf("Delete old timeline entrys...\n");
            index = 0;
            key = QString("Topic %1 Title").arg(index);
            while (timelineCfg.hasKey(key)) {
                timelineCfg.deleteEntry(key);
                timelineCfg.deleteEntry(QString("Topic %1 Icon").arg(index));
                timelineCfg.deleteEntry(QString("Topic %1 Duration").arg(index));

                index++;
                key = QString("Topic %1 Title").arg(index);
            }
        }

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
        MainWindow *window = new MainWindow;
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
        RESTORE(MainWindow);
    } else { // no session.. just start up normally        
        startRecordItNow(KCmdLineArgs::parsedArgs());
    }

    return app.exec();

}
