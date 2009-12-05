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
#include "service.h"
#include "infojob.h"

// KDE
#include <kdebug.h>
#include <kio/scheduler.h>
#include <kapplication.h>

// Qt
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtCore/QUuid>


namespace KYouBlip {


Service::Service(QObject *parent)
    : QObject(parent)
{

    m_manager = new QNetworkAccessManager(this);
    connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(infoJobFinished(QNetworkReply*)));

}


Service::~Service()
{

    delete m_manager;

}


QString Service::getUniqueId()
{

    return QUuid::createUuid().toString();

}


void Service::jobData(KIO::Job *job, const QByteArray &data)
{

    m_data[job] += data;

}


void Service::jobResult(KJob *job)
{

    const QByteArray data = m_data[job];
    m_data.remove(job);
    jobFinished(job, data);

}


void Service::jobDataReq(KIO::Job *job, QByteArray &data)
{

    data = m_reqData[job].left(1024*1024);
    m_reqData[job] = m_reqData[job].mid(1024*1024);

    if (m_reqData[job].size() == 0) {
        m_reqData.remove(job);
    }

}


void Service::infoJobFinished(QNetworkReply *reply)
{

    reply->deleteLater();

}


void Service::infoJobResult(KJob *job)
{

    kDebug() << "jobFinished..!";
    jobFinished(job, static_cast<InfoJob*>(job)->getResponse());
    job->deleteLater();

}


KJob *Service::post(const KUrl &url, const KIO::MetaData &meta, const QByteArray &postData,
                    const bool &hideProgress)
{

    KIO::TransferJob *job;

    KIO::JobFlags flags;
    if (hideProgress) {
        flags = KIO::HideProgressInfo;
    }

    if (postData.size() > (1024*1024)) {
        job = KIO::http_post(url, QByteArray(), flags);
        m_reqData[job] = postData;
        connect(job, SIGNAL(dataReq(KIO::Job*,QByteArray&)), this,
                SLOT(jobDataReq(KIO::Job*,QByteArray&)));
    } else {
        job = KIO::http_post(url, postData, flags);
    }

    job->addMetaData(meta);
    job->setAutoDelete(true);
    job->setTotalSize(postData.size());

    connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)), SLOT(jobData(KIO::Job*, const QByteArray&)));
    connect(job, SIGNAL(result(KJob* )), SLOT(jobResult(KJob*)));

    m_data[job] = QByteArray();

    KIO::Scheduler::scheduleJob(job);

    return job;

}


InfoJob *Service::post(const KUrl &url, QHash<QString, QString> &header, const QByteArray &data)
{

    QNetworkRequest request;
    request.setUrl(url);

    QHashIterator<QString, QString> it(header);
    while (it.hasNext()) {
        it.next();
        request.setRawHeader(it.key().toLatin1(), it.value().toLatin1());
    }
    QNetworkReply *reply = m_manager->post(request, data);

    InfoJob *job = new InfoJob(reply);
    connect(job, SIGNAL(result(KJob*)), this, SLOT(infoJobResult(KJob*)));
    job->setTotalAmount(data.size());

    return job;

}


KJob *Service::get(const KUrl &url, const KIO::LoadType &loadType, const bool &hideProgress)
{

    KIO::TransferJob *job;
    if (hideProgress) {
        job = KIO::get(url, loadType, KIO::HideProgressInfo);
    } else {
        job = KIO::get(url, loadType);
    }

    job->setAutoDelete(true);

    connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)), SLOT(jobData(KIO::Job*, const QByteArray&)));
    connect(job, SIGNAL(result(KJob* )), SLOT(jobResult(KJob*)));

    m_data[job] = QByteArray();

    KIO::Scheduler::scheduleJob(job);

    return job;

}


void Service::jobFinished(KJob *job, const QByteArray &data)
{

    Q_UNUSED(job);
    Q_UNUSED(data);

}


}; // KYouBlip


#include "service.moc"

