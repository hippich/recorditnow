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


// own
#include "infojob.h"

// KDE
#include <klocalizedstring.h>
#include <kjobuidelegate.h>
#include <kdebug.h>
#include <kjobtrackerinterface.h>

// Qt
#include <QtNetwork/QNetworkReply>
#include <QtCore/QMutex>


InfoJob::InfoJob(QNetworkReply *reply, QObject *parent)
    : KJob(parent), m_reply(reply)
{

    qRegisterMetaType< QPair<QString,QString> >("QPair<QString,QString>");
    m_state = InfoJob::Idle;

    setAutoDelete(false);

    connect(m_reply, SIGNAL(uploadProgress(qint64,qint64)), this,
            SLOT(uploadProgress(qint64,qint64)));
    connect(m_reply, SIGNAL(finished()), this, SLOT(uploadFinished()));

    setCapabilities(KJob::Killable);
    KIO::getJobTracker()->registerJob(this);

    m_speedTime.start();

}


InfoJob::~InfoJob()
{

    kDebug() << "info finished.....";

}


InfoJob::State InfoJob::state() const
{

    return m_state;

}


QByteArray InfoJob::getResponse() const
{

    return m_reply->readAll();

}


void InfoJob::start()
{



}


void InfoJob::setTotalAmount(qulonglong amount)
{

    KJob::setTotalAmount(KIO::Job::Bytes, amount);

}


void InfoJob::setSource(const QString &source)
{

    m_source = source;
    setState(state());

}


void InfoJob::setState(const InfoJob::State &newState)
{

    if (state() == newState) {
        return;
    }

    switch (newState) {
    case InfoJob::Idle: {
            break;
        }
    case InfoJob::Upload: {
            emit description(this, i18nc("Title", "Upload"),
                             qMakePair(i18nc("video file", "Source"), m_source),
                             qMakePair(i18nc("upload url", "Destination"),
                                       m_reply->request().url().toString()));
            emit infoMessage(this, i18n("Transmit Data"), QString());
            break;
        }
    case InfoJob::Download: {
            emit description(this, i18nc("Title", "Download"),
                             qMakePair(i18nc("video file", "Source"),
                                       m_reply->request().url().toString()));
            break;
        }
    case InfoJob::Kill: {
            emit description(this, i18nc("Title", "Upload"), qMakePair(i18nc("Upload canceled",
                                                                             "Canceled"), m_source));
            break;
        }
    }

}


void InfoJob::uploadProgress(qint64 bytesSent, qint64 bytesTotal)
{

    kDebug() << "upload progress...";
    if (state() != InfoJob::Upload) {
        setState(InfoJob::Upload);
    }
    if (!bytesTotal != -1) {
        const qlonglong speed = bytesSent*1000.0/m_speedTime.elapsed();
        setProcessedAmount(KJob::Bytes, bytesSent);
        emitPercent(bytesSent, bytesTotal);
        emitSpeed(speed);
    }

}


void InfoJob::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{

    kDebug() << "download progress...";
    if (state() != InfoJob::Upload) {
        setState(InfoJob::Download);
    }
    if (!bytesTotal != -1) {
        setProcessedAmount(KJob::Bytes, bytesReceived);
        emitPercent(bytesReceived, bytesTotal);
    }

}


void InfoJob::uploadFinished()
{

    kDebug() << "upload finished....";
    setState(InfoJob::Idle);

    if (m_reply->error() != QNetworkReply::NoError) {
        setError(700);
    }

    emitResult();

}


bool InfoJob::doKill()
{

    if (state() != Kill) {
        setState(Kill);
        kDebug() << "abort...";
        m_reply->disconnect(this);
        m_reply->abort();
    }

    return true;

}


#include "infojob.moc"

