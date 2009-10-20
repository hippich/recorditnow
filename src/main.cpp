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
                     ki18n("RecordMyDesktop GUI"),
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
            //kcomedown *widget = new kcomedown;
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
