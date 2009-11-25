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
#include "zoomconfig.h"
#include <recorditnow.h>

// KDE
#include <kactioncollection.h>
#include <kaction.h>


ZoomConfig::ZoomConfig(QWidget *parent)
    : QWidget(parent)
{

    setupUi(this);

}


ZoomConfig::~ZoomConfig()
{



}


void ZoomConfig::loadConfig()
{

    if (!Settings::zoomShortcut().isEmpty()) {
        shortcutWidget->setKeySequence(QKeySequence(Settings::zoomShortcut()));
    } else {
        defaults();
    }

}


void ZoomConfig::saveConfig()
{

    Settings::self()->setZoomShortcut(shortcutWidget->keySequence().toString());

}


void ZoomConfig::defaults()
{

    shortcutWidget->setKeySequence(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_Z));

}


void ZoomConfig::keySequenceChanged(const QKeySequence &seq)
{

    Q_UNUSED(seq);
    emit configChanged();

}
