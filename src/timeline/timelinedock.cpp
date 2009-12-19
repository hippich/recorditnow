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
#include "timelinedock.h"
#include "timeline.h"
#include <recorditnow.h>

// KDE
#include <klocalizedstring.h>
#include <kconfiggroup.h>


TimelineDock::TimelineDock(QWidget *parent)
    : QDockWidget(i18n("Timeline"), parent)
{

    setObjectName("Timeline");
    setAllowedAreas(Qt::AllDockWidgetAreas);
    setFeatures(QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetFloatable);
    Timeline *line = new Timeline(this);
    setWidget(line);

    KConfigGroup cfg(Settings::self()->config(), "Timeline");
    line->loadTopics(&cfg);

}


TimelineDock::~TimelineDock()
{

    KConfigGroup cfg(Settings::self()->config(), "Timeline");
    timeline()->saveTopics(&cfg);

}


Timeline *TimelineDock::timeline() const
{

    return static_cast<Timeline*>(widget());

}


#include "timelinedock.moc"
