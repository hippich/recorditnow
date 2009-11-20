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
#include "blipservice.h"
#include "blipvideo.h"

// KDE
#include <kdebug.h>
#include <kio/job.h>
#include <klocale.h>
#include <klocalizedstring.h>

// Qt
#include <QtCore/QFile>
#include <QtCore/QHash>
#include <QtXml/QXmlStreamReader>
#include <QtCore/QDateTime>


BlipService::BlipService(QObject *parent) :
    KYouBlip::Service(parent)
{



}


BlipService::~BlipService()
{

    QHashIterator<KJob*, JobData> it(m_jobs);
    while (it.hasNext()) {
        it.next();
        if (it.key()) {
            it.key()->kill();
        }
    }

}


QString BlipService::upload(const BlipVideo *video, const QString &account, const QString &password)
{

    QString errorString;
    if (video->title().isEmpty()) {
        errorString = i18n("No title specified.");
    }

    if (video->file().isEmpty() || !QFile::exists(video->file())) {
        errorString = i18n("No video specified.");
    }

    if (account.isEmpty() || password.isEmpty()) {
        errorString = i18n("No account/password specified.");
    }

    if (video->getCategoryCode() == "-5") {
        errorString = i18n("Invalid category.");
    }
    
    if (video->getLicenseCode() == "-5") {
        errorString = i18n("Invalid license.");
    }


    if (!errorString.isEmpty()) {
        return "Error: "+errorString;
    }

    const KUrl url("http://blip.tv/file/post");

    QByteArray BOUNDARY = "-----------$$SomeFancyBoundary$$";
    QByteArray CRLF = "\r\n";


    QHash<QString, QString> fields;
    fields["post"] = "1";
    fields["skin"] = "xmlhttprequest";
    fields["file_role"] = "Web";
    fields["item_type"] = "file";

    fields["userlogin"] = account;
    fields["password"] = password;
    fields["title"] = video->title();
    fields["description"] = video->description();
    fields["topics"] = video->keywords().join(", ");
    fields["categories_id"] = video->getCategoryCode();
    fields["license"] = video->getLicenseCode();


    QHash<QString, QString> files;
    files["file"] = video->file();


    QHashIterator<QString, QString> fit(files);
    QHashIterator<QString, QString> it(fields);


    QByteArray data = CRLF;
    while (it.hasNext()) {
        it.next();
        data.append("--"+BOUNDARY);
        data.append(CRLF);
        data.append(QString("Content-Disposition: form-data; name=\"%1\"").arg(it.key()).toLatin1());
        data.append(CRLF);
        data.append(CRLF);
        data.append(it.value().toLatin1());
        data.append(CRLF);
    }

    while (fit.hasNext()) {
        fit.next();
        data.append("--"+BOUNDARY);
        data.append(CRLF);
        data.append(QString("Content-Disposition: form-data; name=\"%1\"; filename=\"%2\"")
                    .arg(fit.key()).arg(fit.value()).toLatin1());
        data.append(CRLF);
        data.append("Content-Type: application/octet-stream");
        data.append(CRLF);
        data.append(CRLF);
        QFile file(fit.value());
        if (!file.open(QIODevice::ReadOnly)) {
            kDebug() << "open failed!";
            return "Error: "+i18nc("%1 = file, %2 = error string", "Cannot open %1.\n"
                                   "Reason: %1", fit.value(), file.errorString());
        }
        data.append(file.readAll());
        data.append(CRLF);
    }

    data.append("--" + BOUNDARY + "--");
    data.append(CRLF);

    QHash<QString, QString> header;
    header["Content-type"] = "multipart/form-data; boundary="+BOUNDARY;
    header["content-length"] = QString::number(data.size());

    const QString id = getUniqueId();

    InfoJob *job = post(url, header, data);
    m_jobs[job] = qMakePair(UploadJob, id);
    job->setSource(video->file());

    return id;

}


QString BlipService::search(const QString &term)
{

    KUrl url("http://www.blip.tv/search/");
    url.addQueryItem("search", term);
    url.addQueryItem("skin", "rss");

    const QString id = getUniqueId();

    m_jobs[get(url, KIO::NoReload, true)] = qMakePair(SearchJob, id);

    return id;

}


void BlipService::jobFinished(KJob *job, const QByteArray &data)
{

    const JobType type = m_jobs[job].first;
    const QString id = m_jobs[job].second;
    const int ret = job->error();
    QString text = data;
    QString errorString;

    m_jobs.remove(job);

    kDebug() << "response:" << data;

    switch (type) {
    case UploadJob: {
            const QRegExp rx("<error>.*</error>");
            rx.indexIn(text);
            if (!rx.cap().isEmpty()) {
                errorString = rx.cap();
                errorString.remove("<error>");
                errorString.remove("</error>");
            } else if (ret != 0 && ret != KIO::ERR_USER_CANCELED) {
                errorString = job->errorString();
                if (errorString.isEmpty()) {
                    errorString = i18nc("%1 = error", "Unkown error: %1.", ret);
                }
            }

            if (ret == KIO::ERR_USER_CANCELED) {
                emit canceled(id);
            } else if (!errorString.isEmpty()) {
                emit error(errorString, id);
            } else {
                emit uploadFinished(id);
            }
            break;
        }
    case SearchJob: {
            QList<BlipVideo*> videoList;
            QXmlStreamReader reader(data);
            while (!reader.atEnd()) {
                reader.readNext();
                if (reader.name() == "item") {
                    videoList.append(readEntry(&reader));
                }
            }
            emit searchFinished(videoList, id);
            break;
        }
    default: break;
    }

}



BlipVideo *BlipService::readEntry(QXmlStreamReader *reader)
{

    BlipVideo *video = new BlipVideo(this);

    reader->readNextStartElement();
    while (reader->name() != "item") {
        reader->readNext();

        if (reader->name().isEmpty() || reader->isEndElement()) {
            continue;
        }

        if (reader->name() == "title" && reader->prefix().isEmpty()) {
            video->setTitle(reader->readElementText().trimmed());
        } else if (reader->name() == "user" && reader->prefix() == "blip") {
            video->setAuthor(reader->readElementText().trimmed());
        } else if (reader->name() == "rating" && reader->prefix() == "blip") {
            video->setRating(reader->readElementText().toDouble());
        } else if (reader->name() == "license" && reader->prefix() == "blip") {
            video->setLicense(reader->readElementText().trimmed());
        } else if (reader->name() == "runtime" && reader->prefix() == "blip") {
            video->setDuration(reader->readElementText().toInt());
        } else if (reader->name() == "content" && reader->prefix() == "media") {
            if (reader->attributes().value("isDefault").toString() == "true") {
                video->setUrl(KUrl(reader->attributes().value("url").toString()));
            }
        } else if (reader->name() == "puredescription" && reader->prefix() == "blip") {
            video->setDescription(reader->readElementText());
        } else if (reader->name() == "pubDate") {
            QString string = reader->readElementText().remove(" +0000");

            string.replace("Mon", QDate::shortDayName(1, QDate::DateFormat));
            string.replace("Tue", QDate::shortDayName(2, QDate::DateFormat));
            string.replace("Wed", QDate::shortDayName(3, QDate::DateFormat));
            string.replace("Thu", QDate::shortDayName(4, QDate::DateFormat));
            string.replace("Fri", QDate::shortDayName(5, QDate::DateFormat));
            string.replace("Sat", QDate::shortDayName(6, QDate::DateFormat));
            string.replace("Sun", QDate::shortDayName(7, QDate::DateFormat));

            string.replace("Jan", QDate::shortMonthName(1, QDate::DateFormat));
            string.replace("Feb", QDate::shortMonthName(2, QDate::DateFormat));
            string.replace("Mar", QDate::shortMonthName(3, QDate::DateFormat));
            string.replace("Apr", QDate::shortMonthName(4, QDate::DateFormat));
            string.replace("May", QDate::shortMonthName(5, QDate::DateFormat));
            string.replace("Jun", QDate::shortMonthName(6, QDate::DateFormat));
            string.replace("Jul", QDate::shortMonthName(7, QDate::DateFormat));
            string.replace("Aug", QDate::shortMonthName(8, QDate::DateFormat));
            string.replace("Sep", QDate::shortMonthName(9, QDate::DateFormat));
            string.replace("Oct", QDate::shortMonthName(10, QDate::DateFormat));
            string.replace("Nov", QDate::shortMonthName(11, QDate::DateFormat));
            string.replace("Dec", QDate::shortMonthName(12, QDate::DateFormat));

            video->setPublished(QDateTime::fromString(string, "ddd, dd MMM yyyy HH:mm:ss"));
            kDebug() << "string:" << string << "=" << QDateTime::fromString(string, "ddd, dd MMM yyyy HH:mm:ss");
        } else if (reader->name() == "thumbnail") {
            video->setThumbnailUrl(KUrl(reader->attributes().value("url").toString()));
        } else if (reader->name() == "category") {
            if (video->category().isEmpty()) {
                video->setCategory(reader->readElementText());
            } else {
                QString keys = video->keywords().join(", ");
                if (!keys.isEmpty()) {
                    keys.append(", ");
                }
                keys.append(reader->readElementText());
                video->setKeywords(keys);
            }
        }
    }

    return video;

}
