/***************************************************************************ß
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
#include "youtubeservice.h"

// KDE
#include <kdebug.h>
#include <kurl.h>
#include <kmimetype.h>
#include <klocalizedstring.h>

// Qt
#include <QtCore/QFile>



#define DEV_KEY "AI39si4PPp_RmxGSVs4cHH93rcG2e9vSRQU1vC0L3sfuy_ZHmtaAWZOdvSfBjmow3YSZfrerx"\
                "jhsZGX0brUrdSLr5qvNchxeiQ"

YouTubeService::YouTubeService(QObject *parent)
    : QObject(parent)
{

    m_authenticated = false;
    m_state = Idle;

}


YouTubeService::~YouTubeService()
{

    if (m_job) {
        m_job->kill();
    }

}


YouTubeService::State YouTubeService::state() const
{

    return m_state;

}


bool YouTubeService::isAuthenticated() const
{

    return m_authenticated;

}


void YouTubeService::authenticate(const QString &account, const QString &password)
{

    if (state() != Idle || account.isEmpty() || password.isEmpty()) {
        return;
    }

    m_state = Auth;

    const KUrl url("https://www.google.com/youtube/accounts/ClientLogin");

    KIO::MetaData meta;
    meta.insert("content-type", "Content-Type: application/x-www-form-urlencoded");
    meta.insert("ssl_no_client_cert", "TRUE");
    meta.insert("ssl_no_ui", "false");
    meta.insert("ssl", "true");
    meta.insert("UseCache", "false");
    meta.insert("cookies", "none");
    meta.insert("no-auth", "true");
    meta.insert("errorPage", "false");

    QByteArray postData = "Email="+account.toLatin1()+"&Passwd="+password.toLatin1()+"&service="\
                          "youtube&source=RecordItNow";

    KIO::TransferJob *job = KIO::http_post(url, postData, KIO::HideProgressInfo);
    job->addMetaData(meta);
    job->setAutoDelete(true);

    connect(job, SIGNAL(result(KJob* )), SLOT(result(KJob*)));
    connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)), SLOT(data(KIO::Job*, const QByteArray&)));
    connect(job, SIGNAL(dataReq(KIO::Job*,QByteArray&)), this, SLOT(dataReq(KIO::Job*,QByteArray&)));

    m_job = job;

}


void YouTubeService::upload(const QHash<QString, QString> data)
{

    if (state() != Idle) {
        return;
    }

    m_state = Upload;

    QString errorString;
    if (!isAuthenticated()) {
        errorString = i18n("Please authenticate first!");
    }

    const QString video = data["File"];
    const QString title = data["Title"];
    const QString description = data["Description"];
    const QString category = data["Category"]; // TODO
    const QString tags = data["Tags"];

    int index = video.lastIndexOf('/');
    index != -1 ? index++ : index = 0;
    QString fileName = video.mid(index);

    if (!QFile::exists(video)) {
        errorString = i18n("No such file: %1.", video);
    }

    if (title.length() > 60 || title.toLatin1().size() > 100) {
        errorString = i18n("Title too long.");
    }

    if (description.length() > 5000) {
        errorString = i18n("Description too long.");
    }

    if (tags.length() > 120) {
        errorString = i18n("Too long tags.");
    }

    foreach (const QString &keyword, tags.split(',')) {
        if (keyword.length() > 25) {
            errorString = i18n("Tag %1 is too long.", keyword);
            break;
        } else if (keyword.length() < 2) {
            errorString = i18n("Tag %1 is too short.", keyword);
            break;
        }
    }

    if (!errorString.isEmpty()) {
        m_state = Idle;
        emit error(errorString);
        return;
    }

    const KUrl url("http://uploads.gdata.youtube.com/feeds/api/users/"+data["Account"]+"/uploads");

    QByteArray CRLF = "\r\n";
    QByteArray BOUNDARY = "f93dcbA3";

    QFile file(data["File"]);
    if (!file.open(QIODevice::ReadOnly)) {
        emit error(i18n("Cannot open video!"));
        return;
    }
    QByteArray videoData = file.readAll();
    file.close();

    KMimeType::Ptr type = KMimeType::findByUrl(KUrl(data["File"]));
    QByteArray mime = type->name().toLatin1();
    if (mime.isEmpty()) {
        mime = "application/octet-stream";
    }

    KIO::MetaData meta;

    QString header = QString("Authorization: GoogleLogin auth=%1\r\n"\
                             "GData-Version: 2\r\n"\
                             "X-GData-Key: key=%2\r\n"\
                             "Slug: %3\r\n"\
                             "Connection: close\r\n").arg(m_auth).arg(DEV_KEY).arg(fileName);

    meta.insert("content-type", "Content-Type: multipart/related; boundary=\""+BOUNDARY+"\"");
    meta.insert("customHTTPHeader", header);


    QString XML = "<?xml version=\"1.0\"?>"+CRLF+\
                     "<entry xmlns=\"http://www.w3.org/2005/Atom\""+CRLF+\
                       "xmlns:media=\"http://search.yahoo.com/mrss/\""+CRLF+\
                       "xmlns:yt=\"http://gdata.youtube.com/schemas/2007\">"+CRLF+\
                       "<media:group>"+CRLF+\
                         "<media:title type=\"plain\">%1</media:title>"+CRLF+\
                         "<media:description type=\"plain\">"+CRLF+\
                           "%2."+CRLF+\
                         "</media:description>"+CRLF+\
                         "<media:category"+CRLF+\
                           "scheme=\"http://gdata.youtube.com/schemas/2007/categories.cat\">%3"+CRLF+\
                         "</media:category>"+CRLF+\
                         "<media:keywords>%4</media:keywords>"+CRLF+\
                       "</media:group>"+CRLF+\
                     "</entry>";

    XML = XML.arg(title).arg(description).arg(category).arg(tags);

    QByteArray postData;
    postData.append("--"+BOUNDARY);
    postData.append(CRLF);
    postData.append("Content-Type: application/atom+xml; charset=UTF-8");
    postData.append(CRLF);
    postData.append(CRLF);
    postData.append(XML.toLatin1());
    postData.append(CRLF);
    postData.append("--"+BOUNDARY);
    postData.append(CRLF);
    postData.append("Content-Type: "+mime);
    postData.append(CRLF);
    postData.append("Content-Transfer-Encoding: binary");
    postData.append(CRLF);
    postData.append(CRLF);
    postData.append(videoData);
    postData.append(CRLF);
    postData.append("--"+BOUNDARY+"--");
    postData.append(CRLF);

    meta.insert("Content-Length", "Content-Length: "+QString::number(postData.size()).toLatin1());


    KIO::TransferJob *job = KIO::http_post(url, postData);
    job->addMetaData(meta);
    job->setAutoDelete(true);
    job->setTotalSize(postData.size());

    connect(job, SIGNAL(data(KIO::Job*, const QByteArray&)), SLOT(data(KIO::Job*, const QByteArray&)));
    connect(job, SIGNAL(result(KJob* )), SLOT(result(KJob*)));
    connect(job, SIGNAL(infoMessage(KJob*, const QString &, const QString&)), SLOT(infoMessage(KJob*, const QString&)));

    m_job = job;

}



void YouTubeService::data(KIO::Job *job, const QByteArray &data)
{

    Q_UNUSED(job);
    m_bytes.append(data);

}


void YouTubeService::result(KJob *job)
{

    kDebug() << "result";

    QString response = m_bytes;
    const int ret = job->error();

    response = response.trimmed();

    kDebug() << "response:" << response;

    m_bytes .clear();

    switch (m_state) {
    case Auth: {
            m_state = Idle;
            if (response.startsWith("Auth=")) {
                response.remove("Auth=");

                const QStringList lines = response.split('\n');
                QString user = lines.last();
                user.remove(QRegExp(".*="));
                m_auth = lines.first();
                m_authenticated = true;
                emit authenticated();
            } else if (ret == KIO::ERR_USER_CANCELED) {
                emit finished();
            } else {
                emit error(i18n("Authentication failed!"));
            }
            break;
        }
    case Upload: {
            emit finished();
            m_state = Idle;
            break;
        }
    case Idle: break;
    }

}

