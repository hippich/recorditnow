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
 *  zoom in/out animation
 *  cache
 *  fix clicks
 *  encode in another thread
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
    QList<QDataStream*> cache;
    int currentCache;
    unsigned char *workMem;
  
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
    
}


KastiRecorder::~KastiRecorder()
{

    delete m_context;

}


int KastiRecorder::zoomFactor() const
{

    return m_context->zoomFactor;

}


void KastiRecorder::initContext(KastiContext *ctx, const QRect &frame, const bool &shm)
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
    ctx->currentCache = 0;
    ctx->mouseClick = false;
    ctx->mouseColor = Qt::black;
    
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

}

#warning "TODO: errors!"
void KastiRecorder::record(const AbstractRecorder::Data &d)
{

    if (m_context->running) {
        kWarning() << "already started!";
        return;
    }
    
    initContext(m_context, d.geometry, true);
    Q_ASSERT(m_context->useShm); // TODO
    
    m_context->outputFile = d.outputFile;
    m_context->fps = d.fps;
#warning "TODO: follow mouse cfg"
    m_context->followMouse = true; 

#warning "TODO: cache"
    for (int i = 0; i < 2; i++) {
        QFile *file = new QFile(getTemporaryFile(d.workDir));
        if (!file->open(QIODevice::WriteOnly)) {
            kFatal() << "open failed:" << file->fileName();
            return;
        }
        
        m_context->cache.append(new QDataStream(file));
    }

    m_context->running = true;
    m_context->time.start();
    m_context->duration.start();

    // first frame
    QTimer::singleShot(0, this, SLOT(cheese()));

}


void KastiRecorder::pause()
{

    m_context->pause = !m_context->pause;

}


void KastiRecorder::stop()
{

    m_context->stop = true;

}

#warning "impl mode"
void KastiRecorder::mouseClick(const QColor &color, const bool &pressed, const int &mode)
{

    m_context->mouseClick = pressed;
    m_context->mouseColor = color;

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

    if (m_context->followMouse) {
        updateFrameGeometry();
    }

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
        emit status(i18n("FPS: %1", m_context->frames));
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


        long duration = m_context->duration.elapsed();
        m_context->averageFPS = m_context->frames_total/(duration/1000);

        qDebug() << "Frames:" << m_context->frames_total << "Skipped:" << m_context->skippedFrames <<
                "Duration" << duration << "ms";
        qDebug() << "Average FPS:" << m_context->averageFPS;

        QTimer::singleShot(0, this, SLOT(encode()));
    } else if (nextShot < 0) {
        nextShot = 0;
    }
    QTimer::singleShot(nextShot, this, SLOT(cheese()));

}


void KastiRecorder::updateFrameGeometry()
{

    const QPoint pos = QCursor::pos();
    QRect frame(0, 0, m_context->width, m_context->height);
    frame.moveCenter(pos);
    const QRect screenGeometry = KApplication::desktop()->screenGeometry(QX11Info::appScreen()); // FIXME

    adjustFrame(&frame, &screenGeometry);

    m_context->xOffset = frame.x();
    m_context->yOffset = frame.y();

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

#warning "TODO: cache2/shm"
bool KastiRecorder::cacheData(unsigned char *buff, const int &bytes, const QByteArray &data, const bool &shm)
{

    if (m_context->currentCache < 0) {
        m_context->currentCache = 0;
    } else if (m_context->currentCache >= m_context->cache.size()) {
        m_context->currentCache = 0;
    }

    QDataStream *stream = m_context->cache.at(m_context->currentCache);
    
    // compress
    unsigned char *cData = (unsigned char*)malloc(bytes);
    lzo_uint compressedSize;
    
    lzo1x_1_11_compress(buff, (lzo_uint)bytes, cData, &compressedSize, m_context->workMem);
    
    QByteArray compressedArray = QByteArray::fromRawData((const char*)cData, compressedSize);

    // write
    *stream << compressedArray;
    *stream << bytes;
    *stream << data;
    
    static_cast<QFile*>(stream->device())->flush(); // FIXME

    free(cData);


    //kDebug() << "uncompressed:" << bytes << "compressed:" << compressedSize;


    return true;

}


bool KastiRecorder::readCache(QByteArray *frame, QByteArray *data)
{

    if (m_context->currentCache < 0) {
        m_context->currentCache = 0;
    } else if (m_context->currentCache >= m_context->cache.size()) {
        m_context->currentCache = 0;
    }
    
    QDataStream *stream = m_context->cache.at(m_context->currentCache);
    QByteArray compressedFrame;
    int size;
    
    *stream >> compressedFrame;
    *stream >> size;
    *stream >> *data;

    if (compressedFrame.isEmpty()) { // EOF
        return false;
    }
    
    // uncompress
    lzo_uint compressedSize = compressedFrame.size();
    lzo_uint uncompressedSize;

    unsigned char *uncompressed = (unsigned char*) malloc(size);
    lzo1x_decompress((unsigned char*)compressedFrame.data(), compressedSize, uncompressed, &uncompressedSize, NULL);

    frame->resize(uncompressedSize);
    memcpy(frame->data(), uncompressed, uncompressedSize);
    free(uncompressed);

    //kDebug() << "uncompressed:" << size << "compressed:" << compressedSize;

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

    stream << m_context->mouseClick; // mouse click?
    stream << m_context->mouseColor;

    return data;

}


void KastiRecorder::getData(const QByteArray *data, int *zoom, QPoint *mousePos, QByteArray *pixels,
                    int *cursorWidth, int *cursorHeight, bool *click, QColor *clickColor)
{

    QDataStream stream(*data);
    stream >> *zoom;
    stream >> *mousePos;
    stream >> *pixels;
    stream >> *cursorWidth;
    stream >> *cursorHeight;
    stream >> *click;
    stream >> *clickColor;

}


AVFrame *picture, *tmp_picture = 0;
uint8_t *video_outbuf = 0;
int frame_count, video_outbuf_size = 0;
void KastiRecorder::encode()
{
    
    foreach (QDataStream *stream, m_context->cache) {
        QFile *file = static_cast<QFile*>(stream->device());
        
        file->flush();
        file->close();
        
        if (!file->open(QIODevice::ReadOnly)) {
            kFatal() << "open failed:" << file->fileName();
        }
    }
    
    
    kDebug() << "encode!";

    
    QString filename = m_context->outputFile;

    AVOutputFormat *fmt;
    AVFormatContext *oc;
    AVStream *video_st;

    // initialize libavcodec, and register all codecs and formats
    avcodec_init();
    avcodec_register_all();
    av_register_all();


    // auto detect the output format from the name. default is mpeg.
    fmt = av_guess_format(NULL, filename.toLatin1(), NULL);
    if (!fmt) {
        kDebug() << "Could not deduce output format from file extension: using MPEG";
        fmt = av_guess_format("mpeg", NULL, NULL);
    }

    if (!fmt) {
        kFatal() << "Could not find suitable output format";
    }


    // allocate the output media context
    oc = avformat_alloc_context();
    if (!oc) {
        kFatal() << "Memory error";
    }

    oc->oformat = fmt;
    snprintf(oc->filename, sizeof(oc->filename), "%s", filename.toLatin1().constData());
    // add the video stream using the default format codec and initialize the codec
    video_st = NULL;
    if (fmt->video_codec != CODEC_ID_NONE || m_context->codecID != CODEC_ID_NONE) {
        int id = fmt->video_codec;
        if (m_context->codecID != CODEC_ID_NONE) {
            if (!av_codec_get_tag(fmt->codec_tag, (CodecID)m_context->codecID)) {
                kWarning() << "could not find tag, codec not currently supported in container";
                kWarning() << "Use default codec:" << fmt->video_codec;
            } else {
                id = m_context->codecID;
            }
        }
        video_st = add_video_stream(oc, id, fmt);
    }


    // set the output parameters (must be done even if no parameters).
    if (av_set_parameters(oc, NULL) < 0) {
        fprintf(stderr, "Invalid output format parameters\n");
        exit(1);
    }

    dump_format(oc, 0, filename.toLatin1().constData(), 1);


    open_video(oc, video_st);

    // open the output file, if needed
    if (!(fmt->flags & AVFMT_NOFILE)) {
        if (url_fopen(&oc->pb, filename.toLatin1().constData(), URL_WRONLY) < 0) {
            kFatal() << "Could not open" << filename;
        }
    }

    // write the stream header, if any
    av_write_header(oc);


    double video_pts;


    video_st->quality = video_st->codec->global_quality;

    QByteArray frame;
    QByteArray data;
    int count = 0;
    while (readCache(&frame, &data)) {
        count++;
        
        kDebug() << count << "/" << m_context->frames_total;

       // FIXME: encode in another thread....
        //emit status(i18n("Encode frame %1/%2", count, m_context->frames_total));
        
        video_pts = (double)video_st->pts.val * video_st->time_base.num / video_st->time_base.den;
        write_video_frame(oc, video_st, frame, data);
    
        frame.clear();
        data.clear();
    }

    // close each codec
    close_video(oc, video_st);

    // write the trailer, if any
    av_write_trailer(oc);

    // free the streams
    for (int i = 0; i < (int)oc->nb_streams; i++) {
        av_freep(&oc->streams[i]->codec);
        av_freep(&oc->streams[i]);
    }

    if (!(fmt->flags & AVFMT_NOFILE)) {
        //close the output file
        url_fclose(oc->pb);
    }

    // free the stream
    av_free(oc);


    // Done
    emit finished(AbstractRecorder::Normal);

}


// add a video output stream
AVStream *KastiRecorder::add_video_stream(AVFormatContext *oc, int codec_id, AVOutputFormat *fmt)
{

    AVCodecContext *c;
    AVStream *st;

    st = av_new_stream(oc, 0);
    if (!st) {
        kFatal() << "Could not alloc stream";
    }

    c = st->codec;
    c->codec_id = (CodecID) codec_id;
    c->codec_type = CODEC_TYPE_VIDEO;
    // put sample parameters
    c->bit_rate = 4500000; // FIXME
    // resolution must be a multiple of two
    c->width = m_context->width;
    c->height = m_context->height;

    // time base: this is the fundamental unit of time (in seconds) in terms
    // of which frame timestamps are represented. for fixed-fps content,
    // timebase should be 1/framerate and timestamp increments should be
    // identically 1.
    c->time_base.den = m_context->fps;
    c->time_base.num = 1;
    c->gop_size = 25; // emit one intra frame every twelve frames at most

//    c->pix_fmt = choosePixelFormat(m_context->codecID);
    c->pix_fmt = PIX_FMT_YUV420P; // FIXME
    if (c->codec_id == CODEC_ID_MPEG1VIDEO) {
        // needed to avoid using macroblocks in which some coeffs overflow
        // this doesnt happen with normal video, it just happens here as the
        // motion of the chroma plane doesnt match the luma plane
        c->mb_decision = 2;
    }

    // flags
    c->flags |= CODEC_FLAG2_FAST;
    c->flags &= ~CODEC_FLAG_OBMC;

    if (fmt->flags & AVFMT_GLOBALHEADER) {
        c->flags |= CODEC_FLAG_GLOBAL_HEADER;
    }

    return st;

}



void KastiRecorder::open_video(AVFormatContext *oc, AVStream *st)
{

    AVCodec *codec;
    AVCodecContext *c;

    c = st->codec;

    // find the video encoder
    codec = avcodec_find_encoder(c->codec_id);
    if (!codec) {
        kFatal() << "codec not found";
    }

    // open the codec
    if (avcodec_open(c, codec) < 0) {
        kFatal() << "could not open codec";
    }

    video_outbuf = NULL;
    if (!(oc->oformat->flags & AVFMT_RAWPICTURE)) {
        // allocate output buffer
        // XXX: API change will be done
        // buffers passed into lav* can be allocated any way you prefer,
        // as long as they're aligned enough for the architecture, and
        // they're freed appropriately (such as using av_free for buffers
        // allocated with av_malloc)
        video_outbuf_size = 1835008;
        video_outbuf = (uint8_t*) av_malloc(video_outbuf_size);
    }

    // allocate the encoded raw picture
    picture = alloc_picture(c->pix_fmt, c->width, c->height);
    if (!picture) {
        kFatal() << "Could not allocate picture";
    }

    // if the output format is not YUV420P, then a temporary YUV420P
    // picture is needed too. It is then converted to the required
    // output format
    
    tmp_picture = NULL;
    tmp_picture = alloc_picture(PIX_FMT_RGB32, c->width, c->height);
    if (!tmp_picture) {
        kFatal() << "Could not allocate temporary picture";
    }

}


AVFrame *KastiRecorder::alloc_picture(int pix_fmt, int width, int height)
{

    AVFrame *picture;
    uint8_t *picture_buf;
    int size;

    picture = avcodec_alloc_frame();
    if (!picture) {
        return NULL;
    }

    picture->quality = 10;

    size = avpicture_get_size((PixelFormat) pix_fmt, width, height);
    picture_buf = (uint8_t*) av_malloc(size);

    if (!picture_buf) {
        av_free(picture);
        return NULL;
    }

    avpicture_fill((AVPicture *)picture, picture_buf,
                   (PixelFormat) pix_fmt, width, height);

    return picture;

}


void KastiRecorder::close_video(AVFormatContext *oc, AVStream *st)
{

    Q_UNUSED(oc);

    avcodec_close(st->codec);
    av_free(picture);
    if (tmp_picture) {
        av_free(tmp_picture->data[0]);
        av_free(tmp_picture);
    }
    av_free(video_outbuf);

}


void KastiRecorder::write_video_frame(AVFormatContext *oc, AVStream *st, const QByteArray &frame, const QByteArray &data)
{

    int ret;
    AVCodecContext *c = st->codec;

    // zoom factor + mouse pos
    int zoom;
    QPoint mousePos;
    QByteArray cursorPixels;
    int cursorWidth;
    int cursorHeight;
    bool click;
    QColor clickColor;
    getData(&data, &zoom, &mousePos, &cursorPixels, &cursorWidth, &cursorHeight, &click, &clickColor);

    if (c->pix_fmt != PIX_FMT_YUV420P/*DESTINATION_PIX_FMT*/) {
        kFatal() << "pix_fmt != PIX_FMT_YUV420P";
    } else {
        QImage pImage((uchar*)frame.data(), c->width, c->height, QImage::Format_RGB32); // frame
        // paint on frame
        QPainter imagePainter(&pImage);
        imagePainter.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform);

        if (click) { // mouse click
            drawMouseClick(&imagePainter, mousePos.x(), mousePos.y(), clickColor);
        }

        // cursor
        int size = cursorPixels.size();
        unsigned long *xpixels = (unsigned long*) cursorPixels.data();
        unsigned char *pixels = (unsigned char*) malloc(size);
            for (int i = 0; i < cursorWidth*cursorHeight; i++) {
                unsigned long pix = xpixels[i];
                pixels[i * 4] = pix & 0xff;
                pixels[(i * 4) + 1] = (pix >> 8) & 0xff;
                pixels[(i * 4) + 2] = (pix >> 16) & 0xff;
                pixels[(i * 4) + 3] = (pix >> 24) & 0xff;
            }

        QImage qcursor((uchar*)pixels, cursorWidth, cursorHeight, QImage::Format_ARGB32);
        imagePainter.drawImage(mousePos, qcursor);
        free(pixels);


        imagePainter.end(); // frame ready

        int iWidth  = pImage.width();
        int iHeight = pImage.height();

        // TODO cache
        struct SwsContext *ctx = sws_getContext(iWidth,
                                                iHeight,
                                                PIX_FMT_RGB32,
                                                c->width,
                                                c->height,
                                                PIX_FMT_YUV420P,//DESTINATION_PIX_FMT,
                                                SWS_FAST_BILINEAR,
                                                NULL,
                                                NULL,
                                                NULL);

        QImage im(tmp_picture->data[0], iWidth, iHeight, QImage::Format_RGB32);
        QPainter p(&im);
        p.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform);

        
        // FIXME: zoom in/out animation
        if (zoom != 1) { // zoom
            kDebug() << "zoom:" << zoom;
            QImage scaled = pImage.scaled(iWidth*zoom,
                                          iHeight*zoom,
                                          Qt::KeepAspectRatio,
                                          Qt::SmoothTransformation);

            QRect target = pImage.rect();
            QPoint pos = mousePos*zoom;

            target.moveCenter(pos);

            QRect geometry = scaled.rect();
            adjustFrame(&target, &geometry);

            p.drawImage(im.rect(), scaled, target);
        } else {
            p.drawImage(0, 0, pImage);
        }

        sws_scale(ctx, tmp_picture->data, tmp_picture->linesize, 0, c->height, picture->data, picture->linesize);
        sws_freeContext(ctx);
    }


    if (oc->oformat->flags & AVFMT_RAWPICTURE) {

        kFatal() << "oc->oformat->flags & AVFMT_RAWPICTURE";
        /*
        AVPacket pkt;
        av_init_packet(&pkt);

        pkt.flags |= PKT_FLAG_KEY;
        pkt.stream_index= st->index;
        pkt.data= (uint8_t *)picture;
        pkt.size= sizeof(AVPicture);


        ret = av_write_frame(oc, &pkt);
        */
    } else {
        // encode the image
        const int out_size = avcodec_encode_video(c, video_outbuf, video_outbuf_size, picture);
        // if zero size, it means the image was buffered
        if (out_size > 0) {
            AVPacket pkt;
            av_init_packet(&pkt);

            pkt.pts = av_rescale_q(c->coded_frame->pts, c->time_base, st->time_base);
            if (c->coded_frame->key_frame)
                pkt.flags |= PKT_FLAG_KEY;
            pkt.stream_index= st->index;
            pkt.data= video_outbuf;
            pkt.size= out_size;

            // write the compressed frame in the media file
            ret = av_write_frame(oc, &pkt);
        } else {
            ret = 0;
        }
    }

    if (ret != 0) {
        kFatal() << "Error while writing video frame";
        exit(1);
    }

    frame_count++;

}


void KastiRecorder::drawMouseClick(QPainter *painter, const int &x, const int &y, const QColor &color)
{

    painter->save();

    QRadialGradient grad(x, y, 50);
    grad.setColorAt(0, color);
    grad.setColorAt(1, Qt::transparent);
    painter->setBrush(QBrush(grad));
    painter->setOpacity(0.5);
    painter->drawEllipse(QPoint(x, y), 50, 50);

    painter->restore();

}


#include "kastirecorder.moc"
