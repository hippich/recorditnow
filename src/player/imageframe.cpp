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

// KDE
#include <kdebug.h>
#include <kurl.h>

// Qt
#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QDrag>
#include <QtGui/QApplication>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDragMoveEvent>
#include <QtGui/QDropEvent>
#include <QtCore/QUrl>
#include <QtCore/QFile>


namespace RecordItNow {


ImageFrame::ImageFrame(QWidget *parent)
    : QFrame(parent)
{

    setFrameStyle(QFrame::Panel);
    setFrameShadow(QFrame::Sunken);
    setAcceptDrops(true);

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
        QSize size = contentsRect().size();

        if (size.height() > m_pixmap.height()) {
            size.setHeight(m_pixmap.height());
        }

        if (size.width() > m_pixmap.width()) {
            size.setWidth(m_pixmap.width());
        }

        const QPixmap pix = m_pixmap.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

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


void ImageFrame::dragEnterEvent(QDragEnterEvent *event)
{

    const QMimeData *data = event->mimeData();
    if (!data) {
        event->setAccepted(false);
        return;
    }

    if (data->hasImage()) {
        event->setAccepted(true);
        return;
    }

    if (data->hasUrls()) {
        foreach (const QUrl &url, data->urls()) {
            KUrl kurl = url;
            if (kurl.isLocalFile() && QFile::exists(kurl.toLocalFile())) {
                event->setAccepted(true);
                return;
            }
        }
    }
    event->setAccepted(false);

}


void ImageFrame::dragMoveEvent(QDragMoveEvent *event)
{

    event->accept();

}


void ImageFrame::dropEvent(QDropEvent *event)
{

    const QMimeData *data = event->mimeData();
    if (!data) {
        event->setAccepted(false);
        return;
    }

    if (data->hasImage()) {

        QImage image = qvariant_cast<QImage>(data->imageData());
        m_pixmap = QPixmap::fromImage(image);
        update();

        event->accept();
    } else if (data->hasUrls()) {
        foreach (const QUrl &url, data->urls()) {
            KUrl kurl = url;
            if (kurl.isLocalFile() && QFile::exists(kurl.toLocalFile())) {

                m_pixmap = QPixmap(kurl.toLocalFile());
                update();

                event->accept();
                break;
            }
        }
    }

}


} // namespace RecordItNow


#include "imageframe.moc"
