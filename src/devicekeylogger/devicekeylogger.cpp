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
#include "devicekeylogger.h"
#include "src/libs/keylogger/keyloggerevent.h"
#include "ui_devicewidget.h"
#include "devicesearchdialog.h"

// Qt
#include <QtCore/QEventLoop>
#include <QtCore/QTime>
#include <QtCore/QTimer>

// KDE
#include <kglobal.h>
#include <kdebug.h>
#include <kauth.h>
#include <klocalizedstring.h>
#include <kapplication.h>
#include <kicon.h>


#define NOT_STARTED_ERROR_STRING "RecordItNow`s Key-Monitor could not be started.\n" \
                                 "For this reason, some features may not be used!\n" \
                                 "The error message was:\n\n"
DeviceKeylogger::DeviceKeylogger(QObject *parent)
    : RecordItNow::AbstractKeylogger(parent),
    m_started(false),
    m_watching(false)
{

    ui_mouse = ui_keyboard = 0;

}


DeviceKeylogger::~DeviceKeylogger()
{

//    stop(); crash
    if (ui_mouse) {
        delete ui_mouse;
    }
    if (ui_keyboard) {
        delete ui_keyboard;
    }

}


QWidget *DeviceKeylogger::getKeyboardConfigWidget(QWidget *parent, const KConfig *cfg)
{

    if (!ui_keyboard) {
        ui_keyboard = new Ui::DeviceWidget();
    }

    KConfigGroup configGroup(cfg, "Keyboard");

    QWidget *widget = new QWidget(parent);
    ui_keyboard->setupUi(widget);
    ui_keyboard->deviceLine->setText(configGroup.readEntry("KeyboardDevice"));
    connect(ui_keyboard->deviceLine, SIGNAL(textChanged(QString)), this, SIGNAL(configChanged()));
    connect(ui_keyboard->searchButton, SIGNAL(clicked()), this, SLOT(showKeyboardDeviceDialog()));

    ui_keyboard->searchButton->setIcon(KIcon("system-search"));

    return widget;

}


QWidget *DeviceKeylogger::getMouseConfigWidget(QWidget *parent, const KConfig *cfg)
{

    if (!ui_mouse) {
        ui_mouse = new Ui::DeviceWidget();
    }

    KConfigGroup configGroup(cfg, "Mouse");

    QWidget *widget = new QWidget(parent);
    ui_mouse->setupUi(widget);
    ui_mouse->deviceLine->setText(configGroup.readEntry("MouseDevice"));
    connect(ui_mouse->deviceLine, SIGNAL(textChanged(QString)), this, SIGNAL(configChanged()));
    connect(ui_mouse->searchButton, SIGNAL(clicked()), this, SLOT(showMouseDeviceDialog()));

    ui_mouse->searchButton->setIcon(KIcon("system-search"));

    return widget;

}


void DeviceKeylogger::saveConfig(KConfig *cfg)
{

    if (ui_mouse) {
        KConfigGroup configGroup(cfg, "Mouse");
        configGroup.writeEntry("MouseDevice", ui_mouse->deviceLine->text());
        qDebug() << "save cfggggggggg";
    }
    if (ui_keyboard) {
        KConfigGroup configGroup(cfg, "Keyboard");
        configGroup.writeEntry("KeyboardDevice", ui_keyboard->deviceLine->text());
        qDebug() << "save cfggggggggg";
    }

}


bool DeviceKeylogger::hasConfigChanged(const KConfig *cfg)
{

    KConfigGroup mouseGroup(cfg, "Mouse");
    KConfigGroup keyboardGroup(cfg, "Keyboard");

    if (ui_mouse && mouseGroup.readEntry("MouseDevice", QString()) !=  ui_mouse->deviceLine->text()) {
        return true;
    } else if (ui_keyboard && keyboardGroup.readEntry("KeyboardDevice", QString()) != ui_keyboard->deviceLine->text()) {
        return true;
    }

    return false;

}


bool DeviceKeylogger::isRunning()
{

    return m_started;

}


QString DeviceKeylogger::error() const
{

    return m_error;

}


bool DeviceKeylogger::start(const KConfig *cfg)
{

    if (m_started) {
        return true;
    }

    QStringList devs;
    KConfigGroup group(cfg, "Keyboard");
    devs << group.readEntry("KeyboardDevice", QString());
    group = KConfigGroup(cfg, "Mouse");
    devs << group.readEntry("MouseDevice", QString());

    if (devs.isEmpty()) {
        return false;
    }

    KAuth::Action action("org.kde.recorditnow.helper.watch");
    connect(action.watcher(), SIGNAL(progressStep(QVariantMap)), this,
            SLOT(progressStep(QVariantMap)));
    connect(action.watcher(), SIGNAL(actionPerformed(ActionReply)), this,
            SLOT(actionPerformed(ActionReply)));

    QVariantMap args;
    args["Devs"] = devs;

    action.setArguments(args);
    action.setExecutesAsync(true);

    KAuth::ActionReply reply = action.execute("org.kde.recorditnow.helper");
    if (reply.errorCode() != KAuth::ActionReply::NoError) {
        if (reply.errorCode() == KAuth::ActionReply::UserCancelled) {
            m_error.clear();
            action.watcher()->disconnect(this);
            return false;
        } else {
            action.watcher()->disconnect(this);
            m_error = parseError(reply.errorCode());
            return false;
        }
    }
    m_started = true;

    return true;

}


void DeviceKeylogger::stop()
{

    if (!m_started) {
        return;
    }

    KAuth::Action action("org.kde.recorditnow.helper.watch");
    action.setHelperID("org.kde.recorditnow.helper");
    action.watcher()->disconnect(this);
    action.stop();

    m_started = false;

}


bool DeviceKeylogger::waitForStarted()
{

    if (!m_started) {
        return false;
    }

    QTime time;
    bool timeout = false;
    
    time.start();
    while (!m_watching) {
        kapp->processEvents(QEventLoop::WaitForMoreEvents|QEventLoop::AllEvents, 1000);

        // timeout (broken kauth etc)
        if (time.elapsed() > (1000*60)) { // 1 min
            timeout = true;
            
            m_error = i18n(NOT_STARTED_ERROR_STRING);
            m_error.append(i18n("Timeout"));
            break;
        } else if (!m_started) { // cancelled
            return false;
        }
    }

    return !timeout;

}


QString DeviceKeylogger::parseError(const int &errorCode)
{

    KAuth::ActionReply::Error reply = static_cast<KAuth::ActionReply::Error>(errorCode);

    QString error;
    switch (reply) {
    case KAuth::ActionReply::NoResponder:
    case KAuth::ActionReply::NoSuchAction:
    case KAuth::ActionReply::InvalidAction:
    case KAuth::ActionReply::HelperBusy:
    case KAuth::ActionReply::DBusError: error = i18n("An internal error has occurred.\n"
                                                     "Error code: %1\n", errorCode); break;
    case KAuth::ActionReply::AuthorizationDenied: error = i18n("You don't have the authorization to"
                                                               " use this feature."); break;
    default: break;
    }

    if (!error.isEmpty()) {
        error.prepend(i18n(NOT_STARTED_ERROR_STRING));
    }

    return error;

}


void DeviceKeylogger::progressStep(const QVariantMap &data)
{

    if (data.contains("Started")) {
        m_watching = true;
        kapp->postEvent(this, new QEvent(QEvent::User), Qt::HighEventPriority); // see waitForStarted
        
        emit started();
        return;
    }

    const RecordItNow::KeyloggerEvent event = data.value("Event").value<RecordItNow::KeyloggerEvent>();
    emit keyEvent(event);

}


void DeviceKeylogger::actionPerformed(const ActionReply &reply)
{

    if (reply.type() != KAuth::ActionReply::Success) {
        if (reply.type() == KAuth::ActionReply::HelperError) {
            m_error = reply.data().value("ErrorString").toString();
        } else {
            m_error = parseError(reply.errorCode());
        }
    }

    KAuth::Action action("org.kde.recorditnow.helper.watch");
    action.setHelperID("org.kde.recorditnow.helper");
    action.watcher()->disconnect(this);

    m_started = m_watching = false;

    emit stopped();

}


void DeviceKeylogger::actionStarted()
{

    emit started();

}


void DeviceKeylogger::showKeyboardDeviceDialog()
{

    DeviceSearchDialog *dialog = new DeviceSearchDialog(KeyMon::DeviceInfo::KeyboardType, ui_keyboard->deviceLine->parentWidget());
    connect(dialog, SIGNAL(deviceSelected(QString)), ui_keyboard->deviceLine, SLOT(setText(QString)));

    dialog->show();

}


void DeviceKeylogger::showMouseDeviceDialog()
{

    DeviceSearchDialog *dialog = new DeviceSearchDialog(KeyMon::DeviceInfo::MouseType, ui_mouse->deviceLine->parentWidget());
    connect(dialog, SIGNAL(deviceSelected(QString)), ui_mouse->deviceLine, SLOT(setText(QString)));

    dialog->show();

}


#include "devicekeylogger.moc"
