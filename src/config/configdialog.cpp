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
#include "helper.h"
#include "pluginmanager.h"
#include "config/mouseconfig.h"
#include "config/frameconfig.h"
#include "config/keyboardconfig.h"
#include "config/shortcutsconfig.h"
#include "config/zoomconfig.h"
#include "config/timelineconfig.h"
#include "config/pluginconfig.h"
#include "config/notificationconfig.h"
#include "config/playerconfig.h"

// KDE
#include <kdebug.h>
#include <kpluginselector.h>
#include <kactioncollection.h>
#include <kmenu.h>


ConfigDialog::ConfigDialog(QWidget *parent, KActionCollection *collection)
    : KConfigDialog(parent, "settings", Settings::self()),
    m_collection(collection)
{

    setAttribute(Qt::WA_DeleteOnClose);
    setInitialSize(QSize(600, 550));
    init();

}


ConfigDialog::~ConfigDialog()
{



}


void ConfigDialog::init()
{

    setFaceType(KConfigDialog::List);

    KConfig *cfg = Settings::self()->config();

    QWidget *generalPage = new QWidget(this);
    ui_settings.setupUi(generalPage);

    updateEncoderCombo();
    ui_settings.encoderCombo->setCurrentItem(Settings::encoderName(), false);

    connect(ui_settings.encoderCombo, SIGNAL(currentIndexChanged(QString)), this,
            SLOT(encoderChanged()));


    RecordItNow::ConfigPage *pluginPage = new PluginConfig(cfg, this);
    RecordItNow::ConfigPage *mousePage = new MouseConfig(cfg, this);
    RecordItNow::ConfigPage *zoomPage = new ZoomConfig(cfg, this);
    RecordItNow::ConfigPage *timelinePage = new TimelineConfig(cfg, this);
    RecordItNow::ConfigPage *shortcutsPage = new ShortcutsConfig(m_collection, cfg, this);
    RecordItNow::ConfigPage *framePage = new FrameConfig(cfg, this);
    RecordItNow::ConfigPage *keyboardPage = new KeyboardConfig(cfg, this);
    RecordItNow::ConfigPage *notificationPage = new NotificationConfig(cfg, this);
    //RecordItNow::ConfigPage *playerPage = new PlayerConfig(cfg, this);

    m_pageList.append(pluginPage);
    m_pageList.append(mousePage);
    m_pageList.append(zoomPage);
    m_pageList.append(timelinePage);
    m_pageList.append(shortcutsPage);
    m_pageList.append(framePage);
    m_pageList.append(keyboardPage);
    m_pageList.append(notificationPage);
   // m_pageList.append(playerPage);

    addPage(generalPage, i18n("RecordItNow"), "configure");
    addPage(pluginPage, i18n("Plugins"), "preferences-plugin");
    addPage(framePage, i18nc("Widget to select a screen area", "Frame"), "draw-rectangle");
    addPage(mousePage, i18n("Mouse Monitor"), "input-mouse");
    addPage(keyboardPage, i18n("Keyboard Monitor"), "input-keyboard");
    addPage(zoomPage, i18n("Zoom"), "zoom-in");
    addPage(timelinePage, i18n("Timeline"), "recorditnow-timeline");
  //  addPage(playerPage, i18n("Player"), "media-playback-start");
    addPage(shortcutsPage, i18n("Shortcuts"), "configure-shortcuts");
    addPage(notificationPage, i18n("Notifications"), "preferences-desktop-notification");

    connect(this, SIGNAL(finished(int)), this, SLOT(configFinished(int)));
    connect(this, SIGNAL(settingsChanged(QString)), this, SLOT(saveSettings()));

    foreach (RecordItNow::ConfigPage *page, m_pageList) {
        page->load();
        connect(page, SIGNAL(settingsChanged()), this, SLOT(pageConfigChanged()));
    }

}


void ConfigDialog::updateEncoderCombo()
{

    const QString oldEncoder = ui_settings.encoderCombo->currentText();
    ui_settings.encoderCombo->clear();
    foreach (const KPluginInfo &info, RecordItNow::Helper::self()->pluginmanager()->getEncoderList()) {
        if (info.isPluginEnabled()) {
            ui_settings.encoderCombo->addItem(KIcon(info.icon()), info.name());
        }
    }
    ui_settings.encoderCombo->setCurrentItem(oldEncoder, false);

}


void ConfigDialog::configFinished(const int &code)
{

    Q_UNUSED(code);
    emit dialogFinished();

}


void ConfigDialog::encoderChanged()
{

    enableButtonApply(true);

}


void ConfigDialog::pageConfigChanged()
{

    updateEncoderCombo();
    updateButtons();

}


void ConfigDialog::saveSettings()
{

    Settings::setEncoderName(ui_settings.encoderCombo->currentText());
    foreach (RecordItNow::ConfigPage *page, m_pageList) {
        page->save();
    }
    updateButtons();

    emit settingsSaved();

}


bool ConfigDialog::hasChanged()
{

    foreach (RecordItNow::ConfigPage *page, m_pageList) {
        if (page->hasChanged()) {
            return true;
        }
    }
    return false;

}


void ConfigDialog::closeEvent(QCloseEvent *event)
{

    configFinished(0);
    KConfigDialog::closeEvent(event);

}


void ConfigDialog::updateWidgetsDefault()
{

    KConfigDialog::updateWidgetsDefault();

    foreach (RecordItNow::ConfigPage *page, m_pageList) {
        page->defaults();
    }
    enableButtonApply(true);

}


void ConfigDialog::updateSettings()
{

    saveSettings();

}


#include "configdialog.moc"

