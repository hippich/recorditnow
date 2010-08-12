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
#include "keyboardconfig.h"
#include "configosd.h"

// KDE
#include <kdebug.h>
#include <kconfiggroup.h>



KeyboardConfig::KeyboardConfig(KConfig *cfg, QWidget *parent)
    : RecordItNow::ConfigPage(cfg, parent)
{

    setupUi(this);
    kcfg_keyboardOnScreenDisplayHideTime->setSuffix(ki18np(" second", " seconds"));

    m_osd = new RecordItNow::ConfigOSD(this);
    connect(m_osd, SIGNAL(OSDChanged()), this, SLOT(OSDChanged()));

    showOSDButton->setIcon(KIcon("window-new"));
    connect(showOSDButton, SIGNAL(clicked()), this, SLOT(showOSD()));

    hideOSDButton->setIcon(KIcon("window-close"));
    connect(hideOSDButton, SIGNAL(clicked()), this, SLOT(hideOSD()));

}


KeyboardConfig::~KeyboardConfig()
{

    delete m_osd;

}


void KeyboardConfig::loadConfig()
{

    m_osd->loadGeometry(keyloggerGeometry(config()));

}


QVariant KeyboardConfig::keyloggerGeometry(KConfig *cfg)
{

    KConfigGroup group(cfg, "Keyboard");
    return group.readEntry("KeyloggerGeometry", QVariant(QRectF(-1, -1, -1, -1)));

}


void KeyboardConfig::saveConfig()
{

    KConfigGroup group(config(), "Keyboard");
    group.writeEntry("KeyloggerGeometry", m_osd->saveGeometry(m_osd, m_osd->geometry()));

}


void KeyboardConfig::setDefaults()
{

    m_osd->loadGeometry(QVariant(QRectF(-1, -1, -1, -1)));

}


void KeyboardConfig::OSDChanged()
{

    setConfigChanged(m_osd->saveGeometry(m_osd, m_osd->geometry()) != keyloggerGeometry(config()));

}


void KeyboardConfig::showOSD()
{

    m_osd->show();

}


void KeyboardConfig::hideOSD()
{

    m_osd->hide();

}


#include "keyboardconfig.moc"

