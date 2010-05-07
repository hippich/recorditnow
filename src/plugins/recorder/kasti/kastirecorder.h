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

#ifndef KASTIRECORDER_H
#define KASTIRECORDER_H


// own
#include "abstractrecorder.h"

// Qt
#include <QtCore/QVariantList>
#include <QtCore/QPointer>


class QPainter;
struct AVFrame;
struct AVOutputFormat;
struct AVFormatContext;
struct AVStream;
class KastiContext;
class KastiRecorder : public AbstractRecorder
{
    Q_OBJECT


public:
    explicit KastiRecorder(QObject *parent = 0, const QVariantList &args = QVariantList());
    ~KastiRecorder();

    int zoomFactor() const;
    
    void record(const AbstractRecorder::Data &d);
    void pause();
    void stop();

    void mouseClick(const QColor &color, const bool &pressed, const int &mode);
    void setZoomFactor(const int &factor);
    
    
private:
    KastiContext *m_context;
    
    void initContext(KastiContext *ctx, const QRect &frame, const bool &shm);
    inline void scheduleNextShot(QTime *lastShot);
    inline void updateFrameGeometry();
    inline void adjustFrame(QRect *frame, const QRect *geometry);
    inline bool cacheData(unsigned char *buff, const int &bytes, const QByteArray &data, const bool &shm);
    bool readCache(QByteArray *frame, QByteArray *data);
    
    
    inline QByteArray createData(void *image);
    void getData(const QByteArray *data, int *zoom, QPoint *mousePos, QByteArray *pixels,
                 int *cursorWidth, int *cursorHeight, bool *click, QColor *clickColor);

    AVStream *add_video_stream(AVFormatContext *oc, int codec_id, AVOutputFormat *fmt);
    void open_video(AVFormatContext *oc, AVStream *st);
    AVFrame *alloc_picture(int pix_fmt, int width, int height);
    void close_video(AVFormatContext *oc, AVStream *st);
    void write_video_frame(AVFormatContext *oc, AVStream *st, const QByteArray &data, const QByteArray &cfg);
    void drawMouseClick(QPainter *painter, const int &x, const int &y, const QColor &color);

    
private slots:
    void cheese();
    void encode();
    

protected slots:


};


#endif // KASTIRECORDER_H
