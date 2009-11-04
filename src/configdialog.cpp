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

    QWidget *general = new QWidget;
    ui_settings.setupUi(general);

    QWidget *recorderPage = new QWidget;
    ui_recorder.setupUi(recorderPage);
    ui_recorder.pluginSelector->addPlugins(m_pluginManager->getRecorderList());
    connect(ui_recorder.pluginSelector, SIGNAL(changed(bool)), this,
            SLOT(recorderSettingsChanged(bool)));

    QWidget *encoderPage = new QWidget;
    ui_encoder.setupUi(encoderPage);
    ui_encoder.pluginSelector->addPlugins(m_pluginManager->getEncoderList());
    connect(ui_encoder.pluginSelector, SIGNAL(changed(bool)), this,
            SLOT(encoderSettingsChanged(bool)));

    updateEncoderCombo();
    ui_encoder.kcfg_encoderIndex->setCurrentItem(Settings::encoderName(), false);

    addPage(general, i18n("RecordItNow"), "configure");
    addPage(recorderPage, i18n("Recorder Plugins"), "preferences-plugin");
    addPage(encoderPage, i18n("Encoder Plugins"), "preferences-plugin");

    connect(this, SIGNAL(finished(int)), this, SLOT(configFinished(int)));

    resize(300, 400);

}


void ConfigDialog::updateEncoderCombo()
{

    const QString oldEncoder = ui_encoder.kcfg_encoderIndex->currentText();
    ui_encoder.kcfg_encoderIndex->clear();
    foreach (const KPluginInfo &info, m_pluginManager->getEncoderList()) {
        if (info.isPluginEnabled()) {
            ui_encoder.kcfg_encoderIndex->addItem(KIcon(info.icon()), info.name());
        }
    }
    ui_encoder.kcfg_encoderIndex->setCurrentItem(oldEncoder, false);

}


void ConfigDialog::configFinished(const int &code)
{

    if (code == KConfigDialog::Accepted) {
        ui_recorder.pluginSelector->updatePluginsState();
        ui_encoder.pluginSelector->updatePluginsState();
        ui_recorder.pluginSelector->save();
        ui_encoder.pluginSelector->save();
        Settings::setEncoderName(ui_encoder.kcfg_encoderIndex->currentText());
    }
    emit dialogFinished();

}


void ConfigDialog::recorderSettingsChanged(const bool &changed)
{

    Q_UNUSED(changed);
    enableButtonApply(true);

}


void ConfigDialog::encoderSettingsChanged(const bool &changed)
{

    enableButtonApply(true);
    if (!changed) {
        return;
    }
    ui_encoder.pluginSelector->updatePluginsState();
    updateEncoderCombo();

}


