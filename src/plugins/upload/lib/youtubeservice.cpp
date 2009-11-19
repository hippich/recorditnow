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
#include <QtXml/QXmlStreamReader>
#include <QtCore/QDateTime>


#define DEV_KEY "AI39si4PPp_RmxGSVs4cHH93rcG2e9vSRQU1vC0L3sfuy_ZHmtaAWZOdvSfBjmow3YSZfrerx"\
                "jhsZGX0brUrdSLr5qvNchxeiQ"

YouTubeService::YouTubeService(QObject *parent)
    : KYouBlip::Service(parent)
{

    m_authenticated = false;

}


YouTubeService::~YouTubeService()
{

    QHashIterator<KJob*, JobData> it(m_jobs);
    while (it.hasNext()) {
        it.next();
        if (it.key()) {
            it.key()->kill();
        }
    }

}


bool YouTubeService::isAuthenticated(const QString &account) const
{

    return !m_token[account].isEmpty();

}


QString YouTubeService::authenticate(const QString &account, const QString &password)
{

    if (account.isEmpty() || password.isEmpty()) {
        return QString();
    }

    const KUrl url("https://www.google.com/youtube/accounts/ClientLogin");

    KIO::MetaData meta;
    meta.insert("content-type", "Content-Type: application/x-www-form-urlencoded");
    meta.insert("errorPage", "false");

    QByteArray postData = "Email="+account.toLatin1()+"&Passwd="+password.toLatin1()+"&service="\
                          "youtube&source=RecordItNow";

    const QString id = Service::getUniqueId();
    m_accountIds[id] = account;

    m_jobs[post(url, meta, postData, true)] = qMakePair(AuthJob, account);

    return id;

}


QString YouTubeService::upload(const YouTubeVideo *video, const QString &account)
{

    QString errorString;
    if (!isAuthenticated(account)) {
        errorString = i18n("Please authenticate first!");
    }

    const QString videoFile = video->file();
    const QString title = video->title();
    const QString description = video->description();
    const QString category = video->category();
    const QString tags = video->keywords().join(", ");

    int index = videoFile.lastIndexOf('/');
    index != -1 ? index++ : index = 0;
    QString fileName = videoFile.mid(index);

    if (!QFile::exists(videoFile)) {
        errorString = i18n("No such file: %1.", videoFile);
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
        return "Error: "+errorString;
    }

    const KUrl url("http://uploads.gdata.youtube.com/feeds/api/users/"+account+"/uploads");

    QByteArray CRLF = "\r\n";
    QByteArray BOUNDARY = "f93dcbA3";

    QFile file(videoFile);
    if (!file.open(QIODevice::ReadOnly)) {
        emit error(i18n("Cannot open video!"), account);
        return QString ();
    }
    QByteArray videoData = file.readAll();
    file.close();

    KMimeType::Ptr type = KMimeType::findByUrl(KUrl(videoFile));
    QByteArray mime = type->name().toLatin1();
    if (mime.isEmpty()) {
        mime = "application/octet-stream";
    }

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


    const QString id = Service::getUniqueId();
    m_accountIds[id] = account;

    QHash<QString, QString> header;
    header["Authorization"] = "GoogleLogin auth="+m_token[account].toLatin1();
    header["GData-Version"] = "2";
    header["X-GData-Key"] = "key="+QString(DEV_KEY).toLatin1();
    header["Connection"] = "close";
    header["Slug"] = fileName.toLatin1();
    header["Content-Type"] = "multipart/related; boundary=\""+BOUNDARY+"\"";
    header["Content-Length"] = QString::number(postData.size()).toLatin1();

    m_jobs[post(url, header, postData)] = qMakePair(UploadJob, id);

    return id;

}


QString YouTubeService::search(const QString &key, const QString &author, const int &start,
                               const int &max)
{

    if (start < 1) {
        return QString();
    }

    if (max > 50) {
        return QString();
    }

    const QString id = Service::getUniqueId();

    KUrl url("http://gdata.youtube.com/feeds/api/videos");
    url.addQueryItem("q", key);
    url.addQueryItem("start-index", QString::number(start));
    url.addQueryItem("max-results", QString::number(max));

    if (!author.isEmpty()) {
        url.addQueryItem("author", author);
    }

    m_jobs[get(url, KIO::NoReload, true)] = qMakePair(SearchJob, id);
    return id;

}


QString YouTubeService::getFavorites(const QString &user, const int &max)
{

    const QString id = Service::getUniqueId();
    KUrl url("http://gdata.youtube.com/feeds/api/users/"+user+"/favorites?v=2");
    url.addQueryItem("max-results", QString::number(max));
    m_jobs[get(url, KIO::NoReload, true)] = qMakePair(FavoritesJob, id);
    return id;

}


void YouTubeService::jobFinished(KJob *job, const QByteArray &data)
{

    JobData jData = m_jobs[job];
    QString id = jData.second;
    const JobType type = jData.first;
    const int ret = job->error();

    m_jobs.remove(job);

    kDebug() << "job finished:" << type << id << data;

    QString response = data.trimmed();

    switch (type) {
    case AuthJob: {

            const QString key = id;
            id = m_accountIds.key(key);
            m_accountIds.remove(key);

            if (response.startsWith("Auth=")) {
                response.remove("Auth=");

                const QStringList lines = response.split('\n');
                QString user = lines.last();
                user.remove(QRegExp(".*="));
                m_token[user] = lines.first();
                emit authenticated(id);
            } else if (ret == KIO::ERR_USER_CANCELED) {
                emit canceled(id);
            } else {
                emit error(i18n("Authentication failed!"), id);
            }
            break;
        }
    case UploadJob: {

            const QString key = id;
            id = m_accountIds.key(key);
            m_accountIds.remove(key);

            QXmlStreamReader reader(response);
            while (!reader.atEnd()) {
                reader.readNext();
                kDebug() << "name:" << reader.name();
            }
            if (reader.hasError() && ret != KIO::ERR_USER_CANCELED) {
                emit error(i18nc("%1 = error", "Upload failed!\n"
                                 "Response: %1", response), id);
                break;
            }
            emit uploadFinished(id);
            break;
        }
    case FavoritesJob:
    case SearchJob: {
            QList<YouTubeVideo*> videoList;
            QXmlStreamReader reader(data);
            while (!reader.atEnd()) {
                reader.readNext();
                if (reader.isStartElement()) {
                    if (reader.name() == "feed") {
                        while (!reader.atEnd()) {
                            reader.readNext();
                            if (reader.isStartElement() && reader.name() == "entry") {
                                videoList.append(readEntry(&reader));
                            }
                        }
                    }
                }
            }
            if (type == SearchJob) {
                emit searchFinished(videoList, id);
            } else {
                emit favoritesFinished(videoList, id);
            }
            break;
        }
    default: break;
    }

}


YouTubeVideo *YouTubeService::readEntry(QXmlStreamReader *reader)
{

    YouTubeVideo *video = new YouTubeVideo(this);
    while (!reader->atEnd()) {
        reader->readNext();

        if (reader->isEndElement() && reader->name() == "entry") {
            break;
        }

        if (reader->isStartElement()) {
            if (reader->name() == "link"
                && reader->attributes().value("rel").toString() == "alternate"
                && reader->attributes().value("type").toString() == "text/html") {
                QString webpage = reader->attributes().value("href").toString();
                video->setUrl(KUrl(webpage));
            } else if (reader->name() == "author") {
                reader->readNext();
                if (reader->name() == "name") {
                    video->setAuthor(reader->readElementText());
                }
            } else if (reader->name() == "published") {
                video->setPublished(QDateTime::fromString(reader->readElementText(), Qt::ISODate));
            } else if (reader->namespaceUri() == "http://gdata.youtube.com/schemas/2007"
                       && reader->name() == "statistics") {
                video->setViewCount(reader->attributes().value("viewCount").toString().toInt());
                video->setFavoriteCount(reader->attributes().value("favoriteCount").toString().toInt());
            }
            else if (reader->namespaceUri() == "http://search.yahoo.com/mrss/"
                     && reader->name() == "group") {

                // media group
                while (!reader->atEnd()) {
                    reader->readNext();
                    if (reader->isEndElement() && reader->name() == "group") {
                        break;
                    }
                    if (reader->isStartElement()) {
                        if (reader->name() == "thumbnail") {
                            video->setThumbnailUrl(KUrl(reader->attributes().value("url").toString()));
                        } else if (reader->name() == "title") {
                            QString title = reader->readElementText();
                            video->setTitle(title);
                        } else if (reader->name() == "description") {
                            QString desc = reader->readElementText();
                            video->setDescription(desc);
                        } else if (reader->name() == "duration") {
                            QString duration = reader->attributes().value("seconds").toString();
                            video->setDuration(duration.toInt());
                        } else if (reader->name() == "keywords") {
                            video->setKeywords(reader->readElementText());
                        } else if (reader->name() == "category") {
                            video->setCategory(reader->readElementText());
                        }
                    }
                }
            } else if (reader->name() == "rating") {
                video->setRating(reader->attributes().value("average").toString().toDouble());
                video->setRaters(reader->attributes().value("numRaters").toString().toInt());
            }
        }
    }
    return video;

}
