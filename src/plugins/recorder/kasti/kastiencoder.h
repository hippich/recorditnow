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

#ifndef KASTIENCODER_H
#define KASTIENCODER_H


// Qt
#include <QtCore/QThread>
#include <QtGui/QColor>
#include <QtCore/QPoint>
#include <QtGui/QImage>


struct AVStream;
struct AVFrame;
struct AVFormatContext;
class QPainter;
struct AVOutputFormat;
class QDataStream;
class KastiEncoder : public QThread
{
    Q_OBJECT
    
    
public:
    struct KastiEncoderContext {
        QList<QDataStream*> cache;
        QString outputFile;
        int width;
        int height;
        int fps;
        int codecID;
        int frames_total;
        int currentCache;
        
        double zoomAnimationFactor;
        int targetZoomFactor;
        bool lastZoomOut;
    };
    
    
    KastiEncoder(KastiEncoderContext *ctx, QObject *parent);
    ~KastiEncoder();
    
    
private:
    KastiEncoderContext *m_context;
    
    inline void getData(const QByteArray *data, int *zoom, QPoint *mousePos, QByteArray *pixels,
                        int *cursorWidth, int *cursorHeight, bool *click, QColor *clickColor);

    inline AVStream *add_video_stream(AVFormatContext *oc, int codec_id, AVOutputFormat *fmt);
    inline void open_video(AVFormatContext *oc, AVStream *st);
    inline AVFrame *alloc_picture(int pix_fmt, int width, int height);
    inline void close_video(AVFormatContext *oc, AVStream *st);
    inline void write_video_frame(AVFormatContext *oc, AVStream *st, const QByteArray &data, const QByteArray &cfg);
    inline void drawMouseClick(QPainter *painter, const int &x, const int &y, const QColor &color);
    inline bool readCache(QByteArray *frame, QByteArray *data);
    inline void zoomImage(const float &factor, const QPoint &mousePos, QImage &image, QRect &target);
    
    
protected:
    void run();
    
    
signals:
    void status(const QString &text);
    
    
};



#endif // KASTIENCODER_H
