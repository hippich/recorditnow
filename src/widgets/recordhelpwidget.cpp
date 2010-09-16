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
#include "recordhelpwidget.h"

// KDE
#include <kicon.h>


namespace RecordItNow {


RecordHelpWidget::RecordHelpWidget(QAction *frame, QAction *fullscreen, QAction *window, QWidget *parent)
    : QWidget(parent)
{

    setupUi(this);

    frameButton->setIcon(KIcon(frame->icon()));
    fullscreenButton->setIcon(KIcon(fullscreen->icon()));
    windowButton->setIcon(KIcon(window->icon()));

    connect(frameButton, SIGNAL(clicked()), frame, SLOT(trigger()));
    connect(fullscreenButton, SIGNAL(clicked()), fullscreen, SLOT(trigger()));
    connect(windowButton, SIGNAL(clicked()), window, SLOT(trigger()));

    foreach (const QAction *action, QList<QAction*>() << frame << fullscreen << window) {
        connect(action, SIGNAL(triggered()), this, SIGNAL(clicked()));
    }

}


} // namespace RecordItNow


#include "recordhelpwidget.moc"
