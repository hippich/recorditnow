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
#include "windowgrabber.h"

// KDE
#include <kdebug.h>

// Qt
#include <QtGui/QMouseEvent>
#include <QtGui/QX11Info>

// X
#include <X11/Xlib.h>


namespace RecordItNow {


WindowGrabber::WindowGrabber()
    : QWidget(0, Qt::X11BypassWindowManagerHint)
{

    setAttribute(Qt::WA_UpdatesDisabled, true);
    setMaximumSize(1, 1);
    move(-1000, -1000);

}


void WindowGrabber::startGrab()
{

    show();
    grabMouse(Qt::CrossCursor);

}


WindowGrabber::~WindowGrabber()
{



}


void WindowGrabber::mousePressEvent(QMouseEvent *event)
{

    event->accept();

}


void WindowGrabber::mouseReleaseEvent(QMouseEvent *event)
{

    event->accept();

    releaseMouse();
    hide();
    if (event->button() == Qt::LeftButton) {
        Window root;
        Window child;
        uint mask;
        int rootX, rootY, winX, winY;

        XGrabServer(x11Info().display());
        XQueryPointer(x11Info().display(), x11Info().appRootWindow(x11Info().appScreen()), &root,
                      &child, &rootX, &rootY, &winX, &winY, &mask);

        if (child == None) {
            child = x11Info().appRootWindow(x11Info().appScreen());
        }

        XUngrabServer(x11Info().display());


        if (child == None) {
            return;
        }

        emit windowGrabbed(child);
    } else {
        emit grabCanceled();
    }
    deleteLater();

}


} // namespace RecordItNow


#include "windowgrabber.moc"
