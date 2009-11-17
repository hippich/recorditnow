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

// KDE
#include <kdebug.h>



namespace KoogleData {


Service::Service(QObject *parent)
    : QObject(parent)
{



}

Service::~Service()
{



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


KJob *Service::post(const KUrl &url, const KIO::MetaData &meta, const QByteArray &postData,
                    const bool &hideProgress)
{

    KIO::TransferJob *job;
    if (hideProgress) {
        job = KIO::http_post(url, postData, KIO::HideProgressInfo);
    } else {
        job = KIO::http_post(url, postData);
    }

    job->addMetaData(meta);
    job->setAutoDelete(true);
    job->setTotalSize(postData.size());

    connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)), SLOT(jobData(KIO::Job*, const QByteArray&)));
    connect(job, SIGNAL(result(KJob* )), SLOT(jobResult(KJob*)));

    m_data[job] = QByteArray();

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

    return job;

}


void Service::jobFinished(KJob *job, const QByteArray &data)
{

    Q_UNUSED(job);
    Q_UNUSED(data);

}


}; // KoogleData


