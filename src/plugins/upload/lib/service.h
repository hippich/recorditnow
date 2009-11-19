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

#ifndef SERVICE_H
#define SERVICE_H


// KDE
#include <kurl.h>
#include <kio/job.h>

// Qt
#include <QtCore/QObject>


class QNetworkReply;
class QNetworkAccessManager;
namespace KYouBlip {


class Service : public QObject
{
    Q_OBJECT


public:
    Service(QObject *parent = 0);
    ~Service();

    static QString getUniqueId();


private:
    QHash<KJob*, QByteArray> m_data;
    QHash<KIO::Job*, QByteArray> m_reqData;
    QNetworkAccessManager *m_manager;


private slots:
    void jobData(KIO::Job *job, const QByteArray &data);
    void jobResult(KJob *job);
    void jobDataReq(KIO::Job *job, QByteArray &data);
    void infoJobFinished(QNetworkReply *reply);
    void infoJobResult(KJob *job);


protected:
    KJob *post(const KUrl &url, const KIO::MetaData &meta,  const QByteArray &postData,
               const bool &hideProgress = false);
    KJob *get(const KUrl &url, const KIO::LoadType &loadType, const bool &hideProgress = false);
    KJob *post(const KUrl &url, QHash<QString, QString> &header, const QByteArray &data);


protected slots:
    virtual void jobFinished(KJob *job, const QByteArray &data);



}; // Service


}; // KYouBlip


#endif // SERVICE_H
