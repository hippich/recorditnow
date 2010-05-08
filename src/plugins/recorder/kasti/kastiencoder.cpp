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
#include "kastiencoder.h"
#include "kastirecorder.h"

// KDE
#include <kdebug.h>
#include <klocalizedstring.h>

// Qt
#include <QtGui/QPainter>
#include <QtCore/QFile>
#include <QtCore/QDataStream>

// ffmpeg
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/pixfmt.h>
#include <libswscale/swscale.h>
}

// LZO
#include <lzo/lzo1x.h>



AVFrame *picture, *tmp_picture = 0;
uint8_t *video_outbuf = 0;
int frame_count, video_outbuf_size = 0;
KastiEncoder::KastiEncoder(KastiEncoderContext *ctx, QObject *parent)
    : QThread(parent), m_context(ctx)
{
        
    m_context->zoomAnimationFactor = -1;
    m_context->lastZoomOut = false;
    m_context->targetZoomFactor = 1;
    m_context->currentClickFrame = 0;
    m_context->stop = false;
    m_context->currentCacheFile = 0;
    m_context->currentCacheStream = 0;
    
}


KastiEncoder::~KastiEncoder()
{
    
    delete m_context;
    
}

void KastiEncoder::stop()
{

    m_context->stop = true;

}


void KastiEncoder::nextCacheFile(const QString &cache)
{
    if (m_context->currentCacheStream) {
        delete m_context->currentCacheStream;
        m_context->currentCacheStream = 0;
    }
    
    if (m_context->currentCacheFile) {
        m_context->currentCacheFile->close();
        m_context->currentCacheFile->remove();
    
        delete m_context->currentCacheFile;
        m_context->currentCacheFile = 0;
    }
    
    if (cache.isEmpty()) {
        return;
    }
    
    QFile *file = new QFile(cache);
    if (!file->open(QIODevice::ReadOnly)) {
        kFatal() << "open failed!";
    }
    QDataStream *stream = new QDataStream(file);

    m_context->currentCacheFile = file;
    m_context->currentCacheStream = stream;

}


void KastiEncoder::run()
{
    
    nextCacheFile(m_context->cacheFiles.takeFirst());
    
    
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
        kFatal() << "Invalid output format parameters";
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
        
        //kDebug() << count << "/" << m_context->frames_total;

        emit status(i18n("Encode frame %1/%2", count, m_context->frames_total));
        
        video_pts = (double)video_st->pts.val * video_st->time_base.num / video_st->time_base.den;
        write_video_frame(oc, video_st, frame, data);
    
        frame.clear();
        data.clear();
    
        if (m_context->stop) {
            break;
        }
    
    }

    foreach (const QString &file, m_context->cacheFiles) {
        nextCacheFile(file); // delete files
    }
    nextCacheFile(QString());
    

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
    emit finished();
    
}



void KastiEncoder::getData(const QByteArray *data, int *zoom, bool *click, QColor *clickColor, 
                           int *markSize, int *mouseMarkMode, CursorData *cursor)
{
    
    QDataStream stream(*data);
    stream >> *zoom;
    stream >> cursor->imgPos;
    stream >> cursor->pixels;
    stream >> cursor->width;
    stream >> cursor->height;
    stream >> cursor->x;
    stream >> cursor->y;
    stream >> cursor->xHot;
    stream >> cursor->yHot;
    stream >> cursor->xOffset;
    stream >> cursor->yOffset;
    stream >> *click;
    stream >> *clickColor;
    stream >> *markSize;
    stream >> *mouseMarkMode;

}



// add a video output stream
AVStream *KastiEncoder::add_video_stream(AVFormatContext *oc, int codec_id, AVOutputFormat *fmt)
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



void KastiEncoder::open_video(AVFormatContext *oc, AVStream *st)
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


AVFrame *KastiEncoder::alloc_picture(int pix_fmt, int width, int height)
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


void KastiEncoder::close_video(AVFormatContext *oc, AVStream *st)
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


void KastiEncoder::zoomImage(const float &factor, const QPoint &mousePos, QImage &image, QRect &target)
{

    QImage scaled = image.scaled(image.width()*factor, 
                                 image.height()*factor,
                                 Qt::KeepAspectRatio,
                                 Qt::SmoothTransformation);

    target = image.rect();
    QPoint pos = mousePos*factor;

    target.moveCenter(pos);

    QRect geometry = scaled.rect();
    KastiRecorder::adjustFrame(&target, &geometry);

    image = scaled;

}


void KastiEncoder::write_video_frame(AVFormatContext *oc, AVStream *st, const QByteArray &frame, const QByteArray &data)
{

    int ret;
    AVCodecContext *c = st->codec;

    // zoom factor + mouse pos
    int zoom;
    bool click;
    QColor clickColor;
    int mouseMarkMode;
    int markSize;
    CursorData cursorData;
    
    getData(&data, &zoom, &click, &clickColor, &markSize, &mouseMarkMode, &cursorData);

    if (c->pix_fmt != PIX_FMT_YUV420P/*DESTINATION_PIX_FMT*/) {
        kFatal() << "pix_fmt != PIX_FMT_YUV420P";
    } else {
        QImage pImage((uchar*)frame.data(), c->width, c->height, QImage::Format_RGB32); // frame
        // paint on frame
        QPainter imagePainter(&pImage);
        imagePainter.setRenderHints(QPainter::Antialiasing|QPainter::SmoothPixmapTransform);

        if (click || 
            (m_context->currentClickFrame <= m_context->fps && m_context->currentClickFrame != -1)) { // mouse click
            if (click) {
                m_context->mouseMarkMode = mouseMarkMode;
                m_context->clickColor = clickColor;
                m_context->currentClickFrame = 1;
            } else {
                mouseMarkMode = m_context->mouseMarkMode;
                clickColor = m_context->clickColor;
                m_context->currentClickFrame++;
            }
            drawMouseClick(&imagePainter, clickColor, markSize, mouseMarkMode, &cursorData);
        } else {
            m_context->currentClickFrame = -1;
        }

        // cursor
        int size = cursorData.pixels.size();
        unsigned long *xpixels = (unsigned long*) cursorData.pixels.data();
        unsigned char *pixels = (unsigned char*) malloc(size);
            for (int i = 0; i < cursorData.width*cursorData.height; i++) {
                unsigned long pix = xpixels[i];
                pixels[i * 4] = pix & 0xff;
                pixels[(i * 4) + 1] = (pix >> 8) & 0xff;
                pixels[(i * 4) + 2] = (pix >> 16) & 0xff;
                pixels[(i * 4) + 3] = (pix >> 24) & 0xff;
            }

        QImage qcursor((uchar*)pixels, cursorData.width, cursorData.height, QImage::Format_ARGB32);
        imagePainter.drawImage(cursorData.imgPos, qcursor);
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

        
        if ((zoom != 1 && !m_context->lastZoomOut) || 
            (m_context->zoomAnimationFactor != -1 && zoom == 1)) { 
           // kDebug() << "zoom:" << zoom;
            
            if (m_context->zoomAnimationFactor != -1 && zoom == 1) { // last zoom out animation
                m_context->lastZoomOut = true;
            }
        
            float currentZoom;
            if (m_context->zoomAnimationFactor == -1) { // zoom in animation start
                m_context->zoomAnimationFactor = 1.0;
            }
            
            if (m_context->targetZoomFactor != zoom) {
                m_context->targetZoomFactor = zoom;
            }
 
            int zoomFactor = zoom*100;
            int lastFactor = m_context->zoomAnimationFactor*100;
            if (lastFactor != zoomFactor) { // zoom factor changed
                if (lastFactor < zoomFactor) {
                    m_context->zoomAnimationFactor = m_context->zoomAnimationFactor+0.1; // zoom in
                } else {
                    m_context->zoomAnimationFactor = m_context->zoomAnimationFactor-0.1; // zoom out
                }
            } else if (m_context->lastZoomOut) { // last zoom out Done
                m_context->lastZoomOut = false;
                m_context->zoomAnimationFactor = -1;
            }
                
            currentZoom = m_context->zoomAnimationFactor;

            QRect target;
            zoomImage(currentZoom, cursorData.imgPos, pImage, target);


            p.drawImage(im.rect(), pImage, target);
        } else {
            p.drawImage(0, 0, pImage); // no zoom
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


void KastiEncoder::drawMouseClick(QPainter *painter, const QColor &color, const int &mode, const int &size, CursorData *cursor)
{
    
    QPoint mousePos(cursor->x-cursor->xOffset, cursor->y-cursor->yOffset); 
    QRect mouseRect(0, 0, size, size);
    mouseRect.moveCenter(mousePos);
    
    painter->save();

    switch (mode) {
        case 0: { // LED
            QRect rect = mouseRect;
            rect.moveTopLeft(mousePos+QPoint(cursor->width, cursor->height));

            // base
            QBrush brush;
            brush.setStyle(Qt::SolidPattern);
            brush.setColor(color);
            painter->setBrush(brush);
            painter->drawEllipse(rect);

            // spot
            QRadialGradient grad(rect.center(), rect.size().height()/2);
            grad.setColorAt(0, Qt::white);
            grad.setColorAt(1, Qt::transparent);
            grad.setFocalPoint(rect.center()-QPoint(rect.size().height()/4, rect.size().height()/4));
            painter->setBrush(QBrush(grad));
            painter->drawEllipse(rect);

            // border
            QPen pen;
            pen.setWidth(2);
            pen.setColor(Qt::black);
            painter->setPen(pen);
            painter->drawEllipse(rect);
            break;
        }
        case 2: { // Target            
            QPen pen;
            pen.setWidth(2);
            pen.setColor(color);
            painter->setPen(pen);

            QRect ellipse = mouseRect;
            ellipse.setWidth(ellipse.width()/1.5);
            ellipse.setHeight(ellipse.height()/1.5);
            ellipse.moveCenter(mouseRect.center());

            QLine hLine1;
            hLine1.setP1(QPoint(mouseRect.left(), mouseRect.center().y()));
            hLine1.setP2(QPoint(mouseRect.x()+mouseRect.width()/3, mouseRect.center().y()));

            QLine hLine2;
            hLine2.setP1(QPoint(mouseRect.right()-(mouseRect.width()/3), mouseRect.center().y()));
            hLine2.setP2(QPoint(mouseRect.right(), mouseRect.center().y()));

            QLine vLine1;
            vLine1.setP1(QPoint(mouseRect.center().x(), mouseRect.top()));
            vLine1.setP2(QPoint(mouseRect.center().x(), mouseRect.y()+mouseRect.height()/3));

            QLine vLine2;
            vLine2.setP1(QPoint(mouseRect.center().x(), mouseRect.bottom()));
            vLine2.setP2(QPoint(mouseRect.center().x(), mouseRect.bottom()-(mouseRect.height()/3)));

            painter->drawLine(hLine1);
            painter->drawLine(hLine2);
            painter->drawLine(vLine1);
            painter->drawLine(vLine2);

            painter->drawEllipse(ellipse);
            break;
        }
        default: { // Circle
            QRadialGradient grad(mouseRect.x(), mouseRect.y(), mouseRect.height());
            grad.setColorAt(0, color);
            grad.setColorAt(1, Qt::transparent);
            painter->setBrush(QBrush(grad));
            painter->setOpacity(0.5);
            painter->drawEllipse(mouseRect);
        }
    }
    painter->restore();

}


bool KastiEncoder::readCache(QByteArray *frame, QByteArray *data)
{

    QDataStream *stream = m_context->currentCacheStream;
    if (stream->atEnd()) {
        if (!m_context->cacheFiles.isEmpty()) {
            nextCacheFile(m_context->cacheFiles.takeFirst());
    
            stream = m_context->currentCacheStream;
        } else {
            nextCacheFile(QString()); // delete last file
            return false;
        }
    }

    QByteArray compressedFrame;
    int size;
    
    *stream >> compressedFrame;
    *stream >> size;
    *stream >> *data;

    if (compressedFrame.isEmpty()) { 
        kFatal() << "empty data";
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


#include "kastiencoder.moc"
