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


#ifndef INFOJOB_H
#define INFOJOB_H


// KDE
#include <kio/job.h>

// Qt
#include <QtCore/QTime>


class QNetworkReply;
class InfoJob : public KJob
{
    Q_OBJECT


public:
    enum State {
        Idle = 0,
        Upload = 1,
        Download = 2,
        Kill = 3
    };

    InfoJob(QNetworkReply *reply, QObject *parent = 0);
    ~InfoJob();

    InfoJob::State state() const;
    QByteArray getResponse() const;

    void start();
    void setTotalAmount(qulonglong amount);


private:
    QNetworkReply *m_reply;
    InfoJob::State m_state;
    QTime m_speedTime;

    void setState(const InfoJob::State &newState);


private slots:
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void uploadFinished();


protected:
     bool doKill();


};


#endif // INFOJOB_H
