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


#ifndef APPLICATION_H
#define APPLICATION_H


// KDE
#include <kapplication.h>

// Qt
#include <QtCore/QPointer>


class QWidget;
class CursorWidget;
class Application : public KApplication
{
    Q_OBJECT


public:
    Application();
    ~Application();

    CursorWidget *getCursorWidget(QWidget *parent);


private:
    QPointer<CursorWidget> m_cursor;


protected:
    bool x11EventFilter(XEvent *event);


};


#endif // APPLICATION_H