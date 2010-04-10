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
#include <QtCore/QtConcurrentRun>
#include <QtCore/QFutureWatcher>
#include <QtCore/QTimer>


namespace RecordItNow {


ImageFrame::ImageFrame(QWidget *parent)
    : QFrame(parent)
{

    setFrameStyle(QFrame::Panel);
    setFrameShadow(QFrame::Sunken);
    setAcceptDrops(true);

    m_updateTimer = new QTimer(this);
    m_updateTimer->setInterval(50);
    m_updateTimer->setSingleShot(true);
    connect(m_updateTimer, SIGNAL(timeout()), this, SLOT(updateImage()));

}


ImageFrame::~ImageFrame()
{

    delete m_updateTimer;

}


void ImageFrame::setPixmap(const QPixmap &pixmap)
{

    m_image = pixmap.toImage();
    scheduleUpdate();

}


void ImageFrame::setScaledImage(const QImage &image)
{

    m_scaledImage = image;
    update();

}


void ImageFrame::scheduleUpdate()
{

    if (m_updateTimer->isActive()) {
        m_updateTimer->stop();
    }
    m_updateTimer->start();

}


void ImageFrame::updateImage()
{

    QFuture<QImage> future = QtConcurrent::run(scaleImage, m_image, contentsRect().size());
    QFutureWatcher<QImage> *watcher = new QFutureWatcher<QImage>();
    watcher->setFuture(future);

    connect(watcher, SIGNAL(finished()), this, SLOT(imageUpdateDone()));
    connect(watcher, SIGNAL(finished()), watcher, SLOT(deleteLater()));

}


QImage ImageFrame::scaleImage(const QImage &image, const QSize &parentSize)
{

    QImage scaled = image;
    if (!image.isNull()) {
        QSize size = parentSize;

        if (size.height() > image.height()) {
            size.setHeight(image.height());
        }

        if (size.width() > image.width()) {
            size.setWidth(image.width());
        }

        scaled = image.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    return scaled;

}


void ImageFrame::imageUpdateDone()
{

    QFutureWatcher<QImage> *watcher = static_cast< QFutureWatcher<QImage>* >(sender());
    m_scaledImage = watcher->future().result();

    update();

}


void ImageFrame::paintEvent(QPaintEvent *event)
{

    QFrame::paintEvent(event);


    QPainter painter(this);
    painter.setClipRegion(event->region());
    painter.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform);

    painter.fillRect(contentsRect(), Qt::black);
    if (!m_scaledImage.isNull()) {
        QRect rect = m_scaledImage.rect();
        rect.moveCenter(contentsRect().center());

        painter.drawImage(rect, m_scaledImage);
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
    if (point.manhattanLength() > QApplication::startDragDistance() && !m_image.isNull()) {
        QDrag *drag = new QDrag(this);

        QMimeData *mimeData = new QMimeData;
        mimeData->setImageData(m_image);
        drag->setMimeData(mimeData);

        QImage image = m_image.scaled(128, 128, Qt::KeepAspectRatio, Qt::FastTransformation);
        drag->setPixmap(QPixmap::fromImage(image));

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

        m_image = qvariant_cast<QImage>(data->imageData());
        scheduleUpdate();

        event->accept();
    } else if (data->hasUrls()) {
        foreach (const QUrl &url, data->urls()) {
            KUrl kurl = url;
            if (kurl.isLocalFile() && QFile::exists(kurl.toLocalFile())) {

                m_image = QImage(kurl.toLocalFile());
                scheduleUpdate();

                event->accept();
                break;
            }
        }
    }

}


void ImageFrame::resizeEvent(QResizeEvent *event)
{

    QFrame::resizeEvent(event);
    scheduleUpdate();

}


} // namespace RecordItNow


#include "imageframe.moc"
