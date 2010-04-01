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
#include "firststartassistant.h"
#include "welcomepage.h"
#include "createconfigpage.h"
#include "mousedevicepage.h"
#include "keyboarddevicepage.h"
#include "../config/keyboardconfig.h"
#include "../keyboard/keyboardkey.h"
#include <recorditnow.h>

// KDE
#include <klocalizedstring.h>
#include <kdebug.h>
#include <kcmdlineargs.h>

// Qt
#include <QtGui/QPainter>
#include <QtGui/QKeyEvent>

// C
#include <linux/input.h>


extern void startRecordItNow(KCmdLineArgs *parsed = 0);
namespace RecordItNow {


FirstStartAssistant::FirstStartAssistant(QWidget *parent)
    : QWizard(parent)
{

    addPage(new WelcomePage(this));
    addPage(new CreateConfigPage(this));
    addPage(new MouseDevicePage(this));
    addPage(new KeyboardDevicePage(this));

    connect(this, SIGNAL(finished(int)), this, SLOT(assistantFinished(int)));

}


FirstStartAssistant::~FirstStartAssistant()
{


}

void FirstStartAssistant::assistantFinished(const int &ret)
{

    hide();

    if (ret == QDialog::Accepted) {
        Settings::setShowFirstStartAssistant(false);

        const QString mouseDevice = field("MouseDevice").toString();
        if (!mouseDevice.isEmpty()) {
            Settings::setMouseDevice(mouseDevice);
            Settings::setShowActivity(true);
        }

        const QString keyboardDevice = field("KeyboardDevice").toString();
        if (!keyboardDevice.isEmpty()) {
            Settings::setKeyboardDevice(keyboardDevice);
            Settings::setEnableKeyboard(true);

            if (KeyboardConfig::readConfig(Settings::self()->config()).isEmpty()) {
                KeyboardConfig::saveConfig(KeyboardConfig::defaultKeys(), Settings::self()->config());
            }
        }

        Settings::self()->writeConfig();

        startRecordItNow();
    }

    deleteLater();

}


} // namespace RecordItNow


#include "firststartassistant.moc"
