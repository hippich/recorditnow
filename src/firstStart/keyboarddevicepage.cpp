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
#include "keyboarddevicepage.h"
#include "keymon/deviceinfo.h"
#include <recorditnow.h>

// KDE
#include <kiconloader.h>


KeyboardDevicePage::KeyboardDevicePage(QWidget *parent)
    : QWizardPage(parent)
{

    setupUi(this);

    QPixmap logo = KIcon("input-keyboard").pixmap(KIconLoader::SizeMedium, KIconLoader::SizeMedium);
    setPixmap(QWizard::LogoPixmap, logo);

    registerField("KeyboardDevice", deviceLine);

    connect(searchWidget, SIGNAL(deviceSelectionChanged(QString)), this, SLOT(deviceChanged(QString)));

}


KeyboardDevicePage::~KeyboardDevicePage()
{


}


void KeyboardDevicePage::initializePage()
{

    searchWidget->search(KeyMon::DeviceInfo::KeyboardType);
    deviceLine->setText(Settings::keyboardDevice());

    QString text;
    if (searchWidget->deviceCount() == 0) {
        text = i18n("Failed to find a suitable device, please skip this step.");
    } else {
        text = i18n("Please select a device.");

    }
    setSubTitle(text);

}


void KeyboardDevicePage::deviceChanged(const QString &device)
{

    deviceLine->setText(device);

}


#include "keyboarddevicepage.moc"
