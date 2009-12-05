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
#include "line.h"

// Qt
#include <QtGui/QResizeEvent>
#include <QtGui/QMoveEvent>

// KDE
#include <kdebug.h>


#define MIN_SIZE 20


Line::Line(const Line::Side &side)
    : QWidget(0, Qt::FramelessWindowHint|Qt::X11BypassWindowManagerHint),
    m_side(side)
{

    // init size
    switch (m_side) {
    case Left:
    case Right: resize(7, 101); break;
    case Top:
    case Bottom: resize(101, 7); break;
    }

    setMouseTracking(true);

    hide();

}



Line::~Line()
{



}


Line::Side Line::side() const
{

    return m_side;

}


void Line::mouseMoveEvent(QMouseEvent *event)
{

    // set cursor
    QRect r = rect();
    switch (m_side) {
    case Left: {
            r.setHeight(size().width());
            r.moveBottomLeft(rect().bottomLeft());
            if (r.contains(event->pos())) {
                setCursor(Qt::SizeBDiagCursor);
                break;
            }
            r.moveTopLeft(rect().topLeft());
            if (r.contains(event->pos())) {
                setCursor(Qt::SizeFDiagCursor);
                break;
            }
            if (event->buttons() == Qt::LeftButton) {
                break;
            }
            setCursor(Qt::SizeHorCursor);
            break;
        }
    case Right: {
            r.setHeight(size().width());
            r.moveBottomRight(rect().bottomRight());
            if (r.contains(event->pos())) {
                setCursor(Qt::SizeFDiagCursor);
                break;
            }
            r.moveTopLeft(rect().topLeft());
            if (r.contains(event->pos())) {
                setCursor(Qt::SizeBDiagCursor);
                break;
            }
            if (event->buttons() == Qt::LeftButton) {
                break;
            }
            setCursor(Qt::SizeHorCursor);
            break;
        }
    case Top: {
            setCursor(Qt::SizeVerCursor);
            break;
        }
    case Bottom: {
            setCursor(Qt::SizeVerCursor);
            break;
        }
    }

    // move
    if (event->buttons() == Qt::NoButton) {
        return;
    }

    const bool dia = (cursor().shape() == Qt::SizeFDiagCursor || cursor().shape() == Qt::SizeBDiagCursor);
    QRect geo = geometry();
    switch (m_side) {
    case Left: {
            if (dia) {
                if (cursor().shape() == Qt::SizeFDiagCursor) { // bottom
                    geo.setTopLeft(event->globalPos());
                } else { // top
                    geo.setBottomLeft(event->globalPos());
                }
            } else { // left/right
                geo.setLeft(event->globalX());
            }
            geo.setWidth(width());
            if (geo.height() < MIN_SIZE) {
                return;
            }
            emit geometryChanged(m_side, geo);
            break;
        }
    case Top: {
            if (dia) {
                if (cursor().shape() != Qt::SizeFDiagCursor) { // right
                    geo.setTopRight(event->globalPos());
                } else { // left
                    geo.setTopLeft(event->globalPos());
                }
            } else { // top/bottom
                geo.setTop(event->globalY());
            }
            geo.setHeight(height());
            if (geo.width() < MIN_SIZE) {
                return;
            }
            emit geometryChanged(m_side, geo);
            break;
        }
    case Right: {
            if (dia) {
                if (cursor().shape() != Qt::SizeFDiagCursor) { // bottom
                    geo.setTopLeft(event->globalPos());
                } else { // top
                    geo.setBottomLeft(event->globalPos());
                }
            } else { // left/right
                geo.setLeft(event->globalX());
            }
            geo.setWidth(width());
            if (geo.height() < MIN_SIZE) {
                return;
            }
            emit geometryChanged(m_side, geo);
            break;
        }
    case Bottom: {
            if (dia) {
                geo.moveTo(event->globalPos());
            } else { // top/bottom
                geo.setTop(event->globalY());
            }
            geo.setHeight(height());

            emit geometryChanged(m_side, geo);
            break;
        }
    }

}


#include "line.moc"

