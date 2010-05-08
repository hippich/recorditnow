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
#include "kastirecorder.h"
#include "frame/frame.h"
#include "kastiencoder.h"
#include <recorditnow_kasti.h>

// KDE
#include <kplugininfo.h>
#include <kglobal.h>
#include <kstandarddirs.h>
#include <kdebug.h>
#include <kapplication.h>

// Qt
#include <QtGui/QWidget>
#include <QtCore/QDir>
#include <QtCore/QTime>
#include <QtGui/QX11Info>
#include <QtCore/QTimer>
#include <QtGui/QDesktopWidget>
#include <QtGui/QPainter>

// C
#include <sys/shm.h>
#include <stdio.h>

// X
#include <X11/Xlib.h>
#include <X11/xpm.h>
#include <X11/X.h>
#include <X11/Xlibint.h>
#include <X11/Xproto.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/XShm.h>

// ffmpeg
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/pixfmt.h>
#include <libswscale/swscale.h>
}

// LZO
#include <lzo/lzo1x.h>


/**********
 * 
 * TODO:
 *  Sound
 *  cache
 *  error handling
 *  .....
 * 
 * 
***********/


#warning "******************************"
#warning ""         
#warning ""
#warning "Build unstable recorder: Kasti"
#warning ""         
#warning ""         
#warning "******************************"
         
         
         
struct KastiContext {
    Frame *frame;
    bool running;
    
    // FPS time
    QTime time;

    // data
    int xOffset;
    int yOffset;
    int width;
    int height;
    int fps;
    Display *dpy;
    int screen;
    int root;

    // cache/video
    QString outputFile;
    QFile *currentCacheFile;
    QDataStream *currentCacheStream;
    QStringList cacheFiles;
    QString cacheDir;
    unsigned char *workMem;
    long int maxCacheSize;
    long int currentCacheSize;
  
    // current zoom default = 1
    int zoomFactor;

    // mouse stuff
    QPoint mousePos;
    bool followMouse;

    // SHM
    bool useShm;
    XShmSegmentInfo shmInfo;
    XImage *image;

    // mouse clicks
    bool mouseClick;
    QColor mouseColor;
    int mouseMarkSize;
    int markMode;

    // stop/pause recording
    bool stop;
    bool pause;

    // Encoding
    CodecID codecID;

    // Stats
    QTime duration;
    int frames;
    int frames_total;
    int averageFPS;
    int skippedFrames;
    long int uncompressedBytes;
    long int compressedBytes;
};



#warning "TODO: Sound"
K_PLUGIN_FACTORY(myFactory, registerPlugin<KastiRecorder>();)
K_EXPORT_PLUGIN(myFactory("kasti_recorder"))
KastiRecorder::KastiRecorder(QObject *parent, const QVariantList &args)
    : AbstractRecorder(parent, args)
{

    m_context = new KastiContext;
    m_context->codecID = CODEC_ID_NONE;
    m_context->running = false;
    m_context->zoomFactor = 1;
    m_context->workMem = 0;
    m_context->frame = 0;
 
    m_encoder = 0;
 
}


KastiRecorder::~KastiRecorder()
{

    if (m_context->workMem) {
        free(m_context->workMem);
    }
    if (m_context->frame) {
        delete m_context->frame;
    }
    delete m_context;

    if (m_encoder) {
        delete m_encoder;
    }

}


int KastiRecorder::zoomFactor() const
{

    return m_context->zoomFactor;

}


void KastiRecorder::initContext(KastiContext *ctx, const QRect &frame, const bool &shm, const bool &showFrame)
{

    ctx->running = false;
    ctx->xOffset = frame.x();
    ctx->yOffset = frame.y();
    ctx->width = frame.width();
    ctx->height = frame.height();
    ctx->frames = 0;
    ctx->frames_total = 0;
    ctx->fps = 0;
    ctx->dpy = QX11Info::display();
    ctx->screen = QX11Info::appScreen();;
    ctx->root = RootWindow(m_context->dpy, m_context->screen);;
    ctx->mousePos = QPoint(-1, -1);
    ctx->followMouse = false;
    ctx->stop = false;
    ctx->pause = false;
    ctx->skippedFrames = 0;
    ctx->mouseClick = false;
    ctx->mouseColor = Qt::black;
    ctx->uncompressedBytes = 0;
    ctx->compressedBytes = 0;
    ctx->maxCacheSize = 1048576*100; // 100 MB
    ctx->currentCacheSize = 0;
    ctx->currentCacheFile = 0;
    ctx->currentCacheStream = 0;
    ctx->cacheFiles = QStringList();
    
    // SHM
    bool canUseShm = shm;
    if (shm) {
        ctx->image = XShmCreateImage(m_context->dpy,
                                     DefaultVisual(m_context->dpy, m_context->screen),
                                     DefaultDepth(m_context->dpy, m_context->screen),
                                     ZPixmap,
                                     NULL,
                                     &ctx->shmInfo,
                                     m_context->width,
                                     m_context->height);

        if (!ctx->image) {
            canUseShm = false;
        } else {
            ctx->shmInfo.shmid = shmget(IPC_PRIVATE,
                                        m_context->image->bytes_per_line*ctx->image->height,
                                        IPC_CREAT|0777);

            if (ctx->shmInfo.shmid == -1) {
                kWarning() << "Can't get shared memory!";
                canUseShm = false;
            } else {
                ctx->shmInfo.shmaddr = ctx->image->data = (char*) shmat(ctx->shmInfo.shmid, 0, 0);
                ctx->shmInfo.readOnly = False;

                if (!XShmAttach(m_context->dpy, &ctx->shmInfo)) {
                    kWarning() << "Failed to attach shared memory!";
                    canUseShm = false;
                }
            }
        }
    } else {
        ctx->image = 0;
    }
    ctx->useShm = canUseShm;

    if (lzo_init() != LZO_E_OK) {
        kFatal() << "LZO init failed!";
    }

    ctx->workMem = (unsigned char*)malloc(LZO1X_1_11_MEM_COMPRESS);

    if (showFrame) {
        m_context->frame = new Frame(0);
        
        m_context->frame->setZoomMode(true);
        m_context->frame->setView(frame.x(), frame.y(), frame.width(), frame.height());
        m_context->frame->setVisible(true);
    }

    m_context->workMem = (unsigned char*) malloc(LZO1X_1_11_MEM_COMPRESS);

}


#warning "TODO: errors!"
void KastiRecorder::record(const AbstractRecorder::Data &d)
{

    if (m_context->running) {
        kWarning() << "already started!";
        return;
    }
    
    Settings::self()->readConfig();
    
    QRect geometry = d.geometry;
    if (d.winId != -1) {
        XWindowAttributes attributes;
        if (!XGetWindowAttributes(QX11Info::display(), d.winId, &attributes)) {
            emit error(i18n("Invalid window!"));
            return;

        }
        geometry = QRect(attributes.x, attributes.y, attributes.width, attributes.height);
    }

    
    initContext(m_context, geometry, true, Settings::showFrame());
    if (!m_context->useShm) {
        kWarning() << "SHM disabled!";
    }
    
    m_context->outputFile = d.outputFile;
    m_context->fps = d.fps;
    m_context->followMouse = Settings::followMouse(); 


    m_context->cacheDir = d.workDir;
    if (!nextCacheFile()) {
        return; // TODO: error signal
    }

    m_context->mouseMarkSize = d.mouseMarkSize;
    m_context->running = true;
    m_context->time.start();
    m_context->duration.start();

    kDebug() << m_context->xOffset << m_context->yOffset <<  m_context->width << m_context->height;
    
    // first frame
    QTimer::singleShot(0, this, SLOT(cheese()));

}


bool KastiRecorder::nextCacheFile()
{

    if (m_context->currentCacheStream) {
        delete m_context->currentCacheStream;
        m_context->currentCacheStream = 0;
    }
    
    if (m_context->currentCacheFile) {
        m_context->currentCacheFile->close();
    
        delete m_context->currentCacheFile;
        m_context->currentCacheFile = 0;
    }
    
    QFile *file = new QFile(getTemporaryFile(m_context->cacheDir));
    if (!file->open(QIODevice::WriteOnly)) {        
        kWarning() << "open failed:" << file->fileName();
        delete file;
        return false;
    }
    
    QDataStream *stream = new QDataStream(file);

    m_context->currentCacheFile = file;
    m_context->currentCacheStream = stream;
    m_context->cacheFiles.append(file->fileName());

    return true;

}


void KastiRecorder::pause()
{

    m_context->pause = !m_context->pause;

}


void KastiRecorder::stop()
{

    m_context->stop = true;
    if (m_encoder) {
        m_encoder->stop();
    }
        
}


void KastiRecorder::mouseClick(const QColor &color, const bool &pressed, const int &mode)
{

    m_context->mouseClick = pressed;
    m_context->mouseColor = color;
    m_context->markMode = mode;
    
}

    
void KastiRecorder::setZoomFactor(const int &factor)
{

    m_context->zoomFactor = qMax(qMin(factor, 10), 1);

}


#define KastiGetReq(req) Display *dpy = m_context->dpy; GetReq(GetImage, req)
#define KastiSyncHandle() Display *dpy = m_context->dpy; SyncHandle()
#define KastiLock() LockDisplay(m_context->dpy)
#define KastiUnlock() UnlockDisplay(m_context->dpy); KastiSyncHandle()

//#define S_DEBUG true
void KastiRecorder::cheese()
{
        
    Q_ASSERT(m_context->running);
    
#ifdef S_DEBUG
    QTime time;
    time.start();
#endif

    QTime startTime;
    startTime.start();

    if (m_context->pause) {
        scheduleNextShot(&startTime);
        return;
    }

    updateFrameGeometry();

#ifdef S_DEBUG
    int frameTime = time.elapsed();
    time.restart();
#endif

   // qDebug() << "cheese Nr: " << m_context->frames;


    KastiLock();

    char *buff = 0;
    long nbytes = 0;
    if (!m_context->useShm) { // TODO

        xGetImageReply rep;
        xGetImageReq *req;

        KastiGetReq(req);

        req->drawable = m_context->root;
        req->x = m_context->xOffset;
        req->y = m_context->yOffset;
        req->width = m_context->width;
        req->height = m_context->height;
        req->planeMask = (unsigned int)AllPlanes;
        req->format = ZPixmap;

        if (!_XReply(m_context->dpy, (xReply *)&rep, 0, xFalse) || !rep.length) {
            kWarning() << "_XReply() failed!";
            KastiUnlock();
            scheduleNextShot(&startTime);
            return;
        }

        nbytes = (long)rep.length << 2;
        buff = (char *) malloc(nbytes);
        _XReadPad(m_context->dpy, buff, nbytes);
    } else {
        if (!XShmGetImage(m_context->dpy, m_context->root, m_context->image, m_context->xOffset, m_context->yOffset, AllPlanes)) {
            kWarning() << "XShmGetImage() failed!";
        } else {
            nbytes = m_context->image->bytes_per_line*m_context->image->height;
            buff = (char*) m_context->image->data;
        }
    }

    KastiUnlock();

#ifdef S_DEBUG
        int requestTime = time.elapsed();
        time.restart();
#endif

    // cursor
    XFixesCursorImage *xcursor = XFixesGetCursorImage(m_context->dpy);

    int x = xcursor->x;
    x -= m_context->xOffset+xcursor->xhot;
    int y = xcursor->y;
    y -= m_context->yOffset+xcursor->yhot;

    m_context->mousePos = QPoint(x, y); // FIXME

#ifdef S_DEBUG
    int cursorTime = time.elapsed();
    time.restart();
#endif

    if (!cacheData((unsigned char*)buff, nbytes, createData(xcursor), m_context->useShm)) {
        if (!m_context->useShm) {
            free(buff);
        }
        m_context->skippedFrames++;
    } else {
        m_context->frames_total++;
        m_context->frames++;
    }
    XFree(xcursor);

#ifdef S_DEBUG
    int appendTime = time.elapsed();
    time.restart();
#endif


#ifdef S_DEBUG
    int totalTime = frameTime+requestTime+cursorTime+appendTime;

    qDebug() << "Frame:" << frameTime << "Request:" << requestTime  <<
            "Cursor" << cursorTime << "Append:" << appendTime <<
            "Total:" << totalTime << "Possible FPS:" << 1000/totalTime;
#endif

    if (m_context->time.elapsed() >= 1000) {
        emit status(i18n("FPS: %1, Duration: %2, Cache: %3", m_context->frames, 
                         KGlobal::locale()->formatDuration(m_context->duration.elapsed()),
                         KGlobal::locale()->formatByteSize(m_context->compressedBytes)));
        m_context->frames = 0;
        m_context->time.restart();
    }

    scheduleNextShot(&startTime);
    
}


void KastiRecorder::scheduleNextShot(QTime *lastShot)
{

#warning "FIX FPS"
    int nextShot = (1000/m_context->fps)-lastShot->elapsed();

#ifdef S_DEBUG
    qDebug() << "next shot in:" << nextShot << "ms";
#endif

    if (m_context->stop) {
        qDebug() << "stopped!";

        if (!m_context->running) {
            return;
        }
        m_context->running = false;

        long duration = m_context->duration.elapsed();
        m_context->averageFPS = m_context->frames_total/(duration/1000);

        kDebug() << "Frames:" << m_context->frames_total << "Skipped:" << m_context->skippedFrames <<
                "Duration" << duration << "ms";
        kDebug() << "Average FPS:" << m_context->averageFPS;
        kDebug() << "Bytes:" << m_context->uncompressedBytes << "Compressed:" << m_context->compressedBytes;

        encode();
    } else if (nextShot <= 0) {
        cheese();
    } else {
        QTimer::singleShot(nextShot, this, SLOT(cheese()));
    }
    
}


void KastiRecorder::updateFrameGeometry()
{

    const QRect screenGeometry = KApplication::desktop()->screenGeometry(QX11Info::appScreen()); // FIXME
    if (m_context->followMouse) {
        const QPoint pos = QCursor::pos();
        QRect frame(0, 0, m_context->width, m_context->height);
        frame.moveCenter(pos);

        adjustFrame(&frame, &screenGeometry);

        m_context->xOffset = frame.x();
        m_context->yOffset = frame.y();
    }
        
    if (m_context->frame) {
        QRect rect(m_context->xOffset, m_context->yOffset, m_context->width, m_context->height);
        QPoint center;
        if (m_context->followMouse || m_context->zoomFactor != 1) {
            center = QCursor::pos();
        } else {
            center = rect.center();
        }
        
        rect.setWidth(rect.width()/m_context->zoomFactor);
        rect.setHeight(rect.height()/m_context->zoomFactor);
        
        rect.moveCenter(center);
        
        if (!m_context->followMouse && m_context->zoomFactor != 1) {
            adjustFrame(&rect, &QRect(m_context->xOffset, m_context->yOffset, m_context->width, m_context->height));
        } else {
            adjustFrame(&rect, &screenGeometry);
        }
        
        m_context->frame->setZoomFactor(m_context->zoomFactor);
        m_context->frame->setView(rect.x(), rect.y(), rect.width(), rect.height());
    }

}


// Make sure that frame is entirely inside of geometry
void KastiRecorder::adjustFrame(QRect *frame, const QRect *geometry)
{

    if (geometry->contains(*frame, true)) { // nothing todo
        return;
    }

    if (frame->width() > geometry->width()) {
        frame->setWidth(geometry->width());
    }

    if (frame->height() > geometry->height()) {
        frame->setHeight(geometry->height());
    }

    if (frame->left() < geometry->left()) {
        frame->moveLeft(geometry->left());
    }

    if (frame->right() > geometry->right()) {
        frame->moveRight(geometry->right());
    }

    if (frame->top() < geometry->top()) {
        frame->moveTop(geometry->top());
    }

    if (frame->bottom() > geometry->bottom()) {
        frame->moveBottom(geometry->bottom());
    }

}


bool KastiRecorder::cacheData(unsigned char *buff, const int &bytes, const QByteArray &data, const bool &shm)
{

    Q_ASSERT(m_context->running);

    QDataStream *stream = m_context->currentCacheStream;

    // compress
    unsigned char *cData = (unsigned char*)malloc(bytes);
    if (!cData) {
        kFatal() << "malloc() failed!";
    }
    
    lzo_uint compressedSize;
    lzo1x_1_11_compress(buff, (lzo_uint)bytes, cData, &compressedSize, m_context->workMem);
    QByteArray compressedArray = QByteArray::fromRawData((const char*)cData, compressedSize);

    // write
    *stream << compressedArray;
    *stream << bytes;
    *stream << data;
    
    stream->device()->waitForBytesWritten(-1);
    
    free(cData);

    m_context->uncompressedBytes += bytes;
    m_context->compressedBytes += compressedSize;

    m_context->currentCacheSize += compressedSize;
    if (m_context->currentCacheSize > m_context->maxCacheSize) {
        kWarning() << "New cache file...";
        nextCacheFile();
        m_context->currentCacheSize = 0;
    }

    if (!shm) {
        free(buff);
    }

    //kDebug() << "uncompressed:" << bytes << "compressed:" << compressedSize;


    return true;

}



QByteArray KastiRecorder::createData(void *image)
{

    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);

    stream << m_context->zoomFactor;
    stream << m_context->mousePos;

    XFixesCursorImage *xcursor = static_cast<XFixesCursorImage*>(image);
    const int size = (xcursor->width*xcursor->height)*4;
    QByteArray cursorArray = QByteArray::fromRawData((const char*) xcursor->pixels, size);
    stream << cursorArray;
    stream << (int)xcursor->width;
    stream << (int)xcursor->height;
    stream << (int)xcursor->x;
    stream << (int)xcursor->y;
    stream << (int)xcursor->xhot;
    stream << (int)xcursor->yhot;
    stream << m_context->xOffset;
    stream << m_context->yOffset;
    
    stream << m_context->mouseClick; // mouse click?
    stream << m_context->mouseColor;
    stream << m_context->mouseMarkSize;
    stream << m_context->markMode;
    
    return data;

}


void KastiRecorder::encode()
{
    
    emit stateChanged(AbstractRecorder::Encode);
    
    if (m_context->frame) {
        delete m_context->frame;
        m_context->frame = 0;
    }
    
    // close
    m_context->currentCacheFile->flush();
    m_context->currentCacheFile->close();
    delete m_context->currentCacheStream;
    m_context->currentCacheStream = 0;
    delete m_context->currentCacheFile;
    m_context->currentCacheFile = 0;
    
    
    KastiEncoder::KastiEncoderContext *ctx = new KastiEncoder::KastiEncoderContext;
    ctx->width = m_context->width;
    ctx->height = m_context->height;
    ctx->codecID = m_context->codecID;
    ctx->outputFile = m_context->outputFile;
    ctx->fps = m_context->fps;
    ctx->frames_total = m_context->frames_total;
    ctx->cacheFiles = m_context->cacheFiles;
    
    
    m_encoder = new KastiEncoder(ctx, this);
    connect(m_encoder, SIGNAL(finished()), this, SLOT(encoderFinished()));
    connect(m_encoder, SIGNAL(status(QString)), this, SIGNAL(status(QString)));
    m_encoder->start();

}


void KastiRecorder::encoderFinished()
{

    m_encoder->deleteLater();
    m_encoder = 0;

    emit finished(AbstractRecorder::Normal);

}


#include "kastirecorder.moc"
