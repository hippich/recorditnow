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
#include "imageframe.h"

// Qt
#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QDrag>
#include <QtGui/QApplication>


namespace RecordItNow {


ImageFrame::ImageFrame(QWidget *parent)
    : QFrame(parent)
{

    setFrameStyle(QFrame::Panel);
    setFrameShadow(QFrame::Sunken);

}


ImageFrame::~ImageFrame()
{



}


void ImageFrame::setPixmap(const QPixmap &pixmap)
{

    m_pixmap = pixmap;
    update();

}


void ImageFrame::paintEvent(QPaintEvent *event)
{

    QFrame::paintEvent(event);


    QPainter painter(this);
    painter.setClipRegion(event->region());
    painter.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform);

    painter.fillRect(contentsRect(), Qt::black);

    if (!m_pixmap.isNull()) {
        const QPixmap pix = m_pixmap.scaled(contentsRect().size(),
                                            Qt::KeepAspectRatio,
                                            Qt::SmoothTransformation);

        QRect rect = pix.rect();
        rect.moveCenter(contentsRect().center());

        painter.drawPixmap(rect, pix);
    }

}


void ImageFrame::mousePressEvent(QMouseEvent *event)
{

    event->accept();
    m_lastPos = event->pos();

}


void ImageFrame::mouseMoveEvent(QMouseEvent *event)
{

    event->accept();
    const QPoint point = event->pos() - m_lastPos;
    if (point.manhattanLength() > QApplication::startDragDistance() && !m_pixmap.isNull()) {
        QDrag *drag = new QDrag(this);

        QMimeData *mimeData = new QMimeData;
        mimeData->setImageData(m_pixmap);
        drag->setMimeData(mimeData);

        QPixmap pixmap = m_pixmap.scaled(128, 128, Qt::KeepAspectRatio, Qt::FastTransformation);
        drag->setPixmap(pixmap);

        drag->exec();
    }

}


void ImageFrame::mouseReleaseEvent(QMouseEvent *event)
{

    event->accept();
    m_lastPos = QPoint();

}


} // namespace RecordItNow


#include "imageframe.moc"
