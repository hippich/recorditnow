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
#include "application.h"
#include "cursorwidget.h"

// KDE
#include <kdebug.h>

// Qt
#include <QtCore/QAbstractEventDispatcher>

// X11
#include <X11/Xlib.h>


Application::Application()
    : KApplication(), m_cursor(0)
{


}


Application::~Application()
{




}



CursorWidget *Application::getCursorWidget(QWidget *parent)
{

    if (!m_cursor) {
        m_cursor = new CursorWidget(parent);
    }
    return m_cursor;

}



bool Application::x11EventFilter(XEvent *event)
{

    if (!m_cursor) {
        return false;
    }

    if (event->type == ButtonPress) {
        if (event->xany.window == QX11Info::appRootWindow()) {
            m_cursor->click(event->xbutton.button);
            return true; // dont process grabed events
        }
    }
    return false;

}


#include "application.moc"
