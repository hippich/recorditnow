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


class QFile;
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
        QString outputFile;
        int width;
        int height;
        int fps;
        int codecID;
        int frames_total;
        int currentCache;
        int bitrate;
        
        QStringList cacheFiles;
        QFile *currentCacheFile;
        QDataStream *currentCacheStream;
        
        // zoom
        double zoomAnimationFactor;
        int targetZoomFactor;
        bool lastZoomOut;
        
        // clicks
        int currentClickFrame;
        QColor clickColor;
        int mouseMarkMode;
        
        // stop
        bool stop;
    };
    
    
    KastiEncoder(KastiEncoderContext *ctx, QObject *parent);
    ~KastiEncoder();
    
    void stop();
    
    
private:
    struct CursorData {
        QByteArray pixels;
        QPoint imgPos;
        int width;
        int height;
        int x;
        int y;
        int xHot;
        int yHot;
        int xOffset;
        int yOffset;
    };
    KastiEncoderContext *m_context;
    QByteArray m_lastFrame;

    inline void getData(const QByteArray *data, int *zoom, bool *click, QColor *clickColor, 
                        int *markSize, int *mouseMarkMode, CursorData *cursor);

    inline AVStream *add_video_stream(AVFormatContext *oc, int codec_id, AVOutputFormat *fmt);
    inline void open_video(AVFormatContext *oc, AVStream *st);
    inline AVFrame *alloc_picture(int pix_fmt, int width, int height);
    inline void close_video(AVFormatContext *oc, AVStream *st);
    inline void write_video_frame(AVFormatContext *oc, AVStream *st, const QByteArray &data, const QByteArray &cfg);
    inline void drawMouseClick(QPainter *painter, const QColor &color, const int &size, const int &mode, CursorData *cursor);
    inline bool readCache(QByteArray *frame, QByteArray *data);
    inline void zoomImage(const float &factor, const QPoint &mousePos, QImage &image, QRect &target);
    inline void nextCacheFile(const QString &cache);
    
    
protected:
    void run();
    
    
signals:
    void status(const QString &text);
    
    
};



#endif // KASTIENCODER_H
