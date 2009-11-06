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

#ifndef FRAMEBOX_H
#define FRAMEBOX_H


// own
#include "line.h"

// Qt
#include <QtCore/QObject>


class QLabel;
class FrameBox : public QObject
{
    Q_OBJECT


public:
    FrameBox(QWidget *parentconst, QRect pos);
    ~FrameBox();

    QRect boxGeometry() const;
    bool isEnabled() const;

    void setEnabled(const bool &enabled);


private:
    QWidget *m_parent;
    Line *m_left;
    Line *m_top;
    Line *m_right;
    Line *m_bottom;
    QFrame *m_sizeFrame;
    QLabel *m_sizeLabel;
    QTimer *m_timer;
    bool m_enabled;

    void adjustToParent();
    void adjustLines();
    void adjustSizeFrame(const QRect &boxGeo);


private slots:
    void lineGeometryChanged(const Line::Side &side, const QRect &geometry);
    void hideFrame();


protected:
    bool eventFilter(QObject *watched, QEvent *event);


};


#endif // FRAMEBOX_H
