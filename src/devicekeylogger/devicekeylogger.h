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

#ifndef RECORDITNOW_DEVICEKEYLOGGER_H
#define RECORDITNOW_DEVICEKEYLOGGER_H


// own
#include "src/libs/keylogger/abstractkeylogger.h"

// KDE
#include <kauth.h>
#include <kdemacros.h>

// Qt
#include <QtCore/QObject>


namespace Ui {
    class DeviceWidget;
}

using namespace KAuth;
class KDE_EXPORT DeviceKeylogger : public RecordItNow::AbstractKeylogger
{
    Q_OBJECT


public:
    DeviceKeylogger(QObject *parent);
    ~DeviceKeylogger();

    QWidget *getKeyboardConfigWidget(QWidget *parent, const KConfig *cfg);
    QWidget *getMouseConfigWidget(QWidget *parent, const KConfig *cfg);


    bool isRunning();
    QString error() const;
    bool start(const KConfig *cfg);
    void stop();
    bool waitForStarted();
    void saveConfig(KConfig *cfg);
    bool hasConfigChanged(const KConfig *cfg);


private:
    bool m_started;
    bool m_watching;
    QString m_error;
    Ui::DeviceWidget *ui_mouse;
    Ui::DeviceWidget *ui_keyboard;

    QString parseError(const int &errorCode);


private slots:
    void progressStep(const QVariantMap &data);
    void actionPerformed(const ActionReply &reply);
    void actionStarted();

    void showKeyboardDeviceDialog();
    void showMouseDeviceDialog();


};


#endif // KEYMONMANAGER_H
