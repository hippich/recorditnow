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

#ifndef LINE_H
#define LINE_H


// Qt
#include <QtGui/QFrame>



class Line : public QFrame
{
    Q_OBJECT


public:
    enum Side {
        Left = 0,
        Top = 1,
        Right = 2,
        Bottom = 3
    };
    Line(const Line::Side &side);
    ~Line();

    Line::Side side() const;



private:
    Side m_side;
    QWidget *m_parent;



protected:
    void mouseMoveEvent(QMouseEvent *event);


signals:
    void geometryChanged(const Line::Side &side, const QRect &geometry);


};


#endif // LINE_H
