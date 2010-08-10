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
#include "pluginconfig.h"
#include "pluginmanager.h"
#include <config-recorditnow.h>
#ifdef HAVE_QTSCRIPT
    #include "script/scriptmanager.h"
    #include "scriptdialog.h"
#endif
#include "helper.h"

// KDE
#include <klocalizedstring.h>
#include <kfiledialog.h>
#include <kmessagebox.h>


PluginConfig::PluginConfig(KConfig *cfg, QWidget *parent)
    : RecordItNow::ConfigPage(cfg, parent)
{

    setupUi(this);

#ifdef HAVE_QTSCRIPT
    connect(scriptButton, SIGNAL(clicked()), this, SLOT(showScriptDialog()));

    scriptButton->setIcon(KIcon("preferences-plugin"));
#else
    scriptButton->hide();
#endif

}


void PluginConfig::saveConfig()
{

    pluginSelector->updatePluginsState();
    pluginSelector->save();

}


void PluginConfig::setDefaults()
{


}


void PluginConfig::loadConfig()
{

    reloadPluginList();

}


void PluginConfig::reloadPluginList()
{


    // KPluginSelector has no clear function :-(
    pluginSelector->hide();
    pluginLayout->removeWidget(pluginSelector);
    delete pluginSelector;

    pluginSelector = new KPluginSelector(this);
    connect(pluginSelector, SIGNAL(changed(bool)), this, SLOT(pluginSettingsChanged(bool)));

    pluginLayout->insertWidget(0, pluginSelector);

    pluginSelector->addPlugins(RecordItNow::Helper::self()->pluginmanager()->getRecorderList(),
                               KPluginSelector::ReadConfigFile,
                               i18n("Record Plugins"), "Recorder");
    pluginSelector->addPlugins(RecordItNow::Helper::self()->pluginmanager()->getEncoderList(),
                               KPluginSelector::ReadConfigFile,
                               i18n("Encode Plugins"), "Encoder");
#ifdef HAVE_QTSCRIPT
    pluginSelector->addPlugins(RecordItNow::Helper::self()->scriptManager()->availableScripts(),
                               KPluginSelector::ReadConfigFile,
                               i18n("Scripts"), "Script");
#endif

}


void PluginConfig::pluginSettingsChanged(const bool &changed)
{

    setConfigChanged(changed);
    if (!changed) {
        return;
    }
    pluginSelector->updatePluginsState();

}


void PluginConfig::showScriptDialog()
{

#ifdef HAVE_QTSCRIPT
    RecordItNow::ScriptDialog *dialog = new RecordItNow::ScriptDialog(this);
    connect(dialog, SIGNAL(destroyed()), this, SLOT(scriptDialogFinished()));

    dialog->show();
#endif

}


void PluginConfig::scriptDialogFinished()
{

#ifdef HAVE_QTSCRIPT
    reloadPluginList();
#endif

}



#include "pluginconfig.moc"
