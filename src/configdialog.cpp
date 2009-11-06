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
#include "configdialog.h"
#include <recorditnow.h>
#include "recorditnowpluginmanager.h"

// KDE
#include <kdebug.h>
#include <kpluginselector.h>


ConfigDialog::ConfigDialog(QWidget *parent, RecordItNowPluginManager *manager)
    : KConfigDialog(parent, "settings", Settings::self()), m_pluginManager(manager)
{

    Q_ASSERT(m_pluginManager);
    setAttribute(Qt::WA_DeleteOnClose);
    init();

}


ConfigDialog::~ConfigDialog()
{

}


void ConfigDialog::init()
{

    QWidget *generalPage = new QWidget(this);
    ui_settings.setupUi(generalPage);

    m_pluginSelector = new KPluginSelector(this);
    connect(m_pluginSelector, SIGNAL(changed(bool)), this, SLOT(pluginSettingsChanged(bool)));

    m_pluginSelector->addPlugins(m_pluginManager->getRecorderList(),
                               KPluginSelector::ReadConfigFile,
                               i18n("Recorder"), I18N_NOOP("Recorder"));
    m_pluginSelector->addPlugins(m_pluginManager->getEncoderList(),
                               KPluginSelector::ReadConfigFile,
                               i18n("Encoder"), I18N_NOOP("Encoder"));
    m_pluginSelector->addPlugins(m_pluginManager->getUploaderList(),
                               KPluginSelector::ReadConfigFile,
                               i18n("Uploader"), I18N_NOOP("Uploader"));
    
    updateEncoderCombo();
    ui_settings.encoderCombo->setCurrentItem(Settings::encoderName(), false);

    connect(ui_settings.encoderCombo, SIGNAL(currentIndexChanged(QString)), this,
            SLOT(encoderChanged()));

    addPage(generalPage, i18n("RecordItNow"), "configure");
    addPage(m_pluginSelector, i18n("Plugins"), "preferences-plugin");

    connect(this, SIGNAL(finished(int)), this, SLOT(configFinished(int)));
    setMinimumHeight(450); // workaround for kpluginselector hang/bug

}


void ConfigDialog::updateEncoderCombo()
{

    const QString oldEncoder = ui_settings.encoderCombo->currentText();
    ui_settings.encoderCombo->clear();
    foreach (const KPluginInfo &info, m_pluginManager->getEncoderList()) {
        if (info.isPluginEnabled()) {
            ui_settings.encoderCombo->addItem(KIcon(info.icon()), info.name());
        }
    }
    ui_settings.encoderCombo->setCurrentItem(oldEncoder, false);

}


void ConfigDialog::configFinished(const int &code)
{

    if (code == KConfigDialog::Accepted) {
        m_pluginSelector->updatePluginsState();
        m_pluginSelector->save();
        Settings::setEncoderName(ui_settings.encoderCombo->currentText());
    }
    emit dialogFinished();

}


void ConfigDialog::pluginSettingsChanged(const bool &changed)
{

    enableButtonApply(true);
    if (!changed) {
        return;
    }
    m_pluginSelector->updatePluginsState();
    updateEncoderCombo();

}


void ConfigDialog::encoderChanged()
{

    enableButtonApply(true);

}


