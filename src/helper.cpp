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
#include "helper.h"
#include <recorditnow.h>
#include "pluginmanager.h"
#include <config-recorditnow.h>
#ifdef HAVE_QTSCRIPT
    #include "script/scriptmanager.h"
#endif
#include "mainwindow.h"

// KDE
#include <kmanagerselection.h>
#include <kwindowsystem.h>

// Qt
#include <QtGui/QX11Info>

// Phonon
#include <Phonon/MediaObject>
#include <Phonon/AudioOutput>


namespace RecordItNow {


Helper::Helper()
    : QObject(0)
{
    
    m_window = 0;
    m_pluginManager = new RecordItNow::PluginManager(this);

    m_firstStart = Settings::firstStart();
    if (firstStart()) {
        Settings::self()->setFirstStart(false);
    }

    Display *dpy = QX11Info::display();
    int screen = DefaultScreen(dpy);
    char net_wm_cm_name[100];
    sprintf(net_wm_cm_name, "_NET_WM_CM_S%d", screen);

    m_compositeWatcher = new KSelectionWatcher(net_wm_cm_name, -1, this);
    connect(m_compositeWatcher, SIGNAL(newOwner(Window)), this, SLOT(compositingChanged()));
    connect(m_compositeWatcher, SIGNAL(lostOwner()), this, SLOT(compositingChanged()));

    m_compositingActive = KWindowSystem::compositingActive();

    m_audioPlayer = new Phonon::MediaObject;
    m_audioOutput = new Phonon::AudioOutput(Phonon::MusicCategory);
    Phonon::createPath(m_audioPlayer, m_audioOutput);

    if (Settings::mouseSoundVolume() != -1) {
        m_audioOutput->setVolume(Settings::mouseSoundVolume());
    }

#ifdef HAVE_QTSCRIPT
    m_scriptManager = new RecordItNow::ScriptManager(this);
#endif

}


Helper::~Helper()
{

    Settings::setMouseSoundVolume(m_audioOutput->volume());
    
    delete m_compositeWatcher;
    m_audioPlayer->deleteLater();
    m_audioOutput->deleteLater();
    
    delete m_pluginManager;

#ifdef HAVE_QTSCRIPT
    Q_ASSERT(!m_scriptManager);
#endif

}


class HelperSingleton
{
    public:
        Helper self;
};


K_GLOBAL_STATIC(HelperSingleton, privateSelf)


Helper *Helper::self()
{

    return &privateSelf->self;

}


bool Helper::firstStart() const
{

    return m_firstStart;

}


bool Helper::compositingActive() const
{

    return m_compositingActive;

}


Phonon::AudioOutput *Helper::audioOutput() const
{

    return m_audioOutput;

}


RecordItNow::PluginManager *Helper::pluginmanager() const
{

    return m_pluginManager;

}


RecordItNow::ScriptManager *Helper::scriptManager() const
{

    return m_scriptManager;

}


RecordItNow::MainWindow *Helper::window() const
{

    return m_window;

}


QString Helper::currentFile() const
{

    return m_window->outputWidget->outputFile();

}


void Helper::playSound(const QString &file)
{

    if (!QFile::exists(file)) {
        return;
    }
    
    m_audioPlayer->setCurrentSource(Phonon::MediaSource(file));
    m_audioPlayer->play();

}


void Helper::setMainWindow(RecordItNow::MainWindow *window)
{

    m_window = window;

}


void Helper::unloadScriptManager()
{

#ifdef HAVE_QTSCRIPT
    delete m_scriptManager;
    m_scriptManager = 0;
#endif

}


void Helper::compositingChanged()
{

    m_compositingActive = m_compositeWatcher->owner() != None;
    emit compositingChanged(m_compositingActive);

}


} // namespace RecordItNow


#include "helper.moc"
