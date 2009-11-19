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


BlipService::BlipService(QObject *parent) :
    KYouBlip::Service(parent)
{



}


BlipService::~BlipService()
{

    QHashIterator<KJob*, QString> it(m_jobs);
    while (it.hasNext()) {
        it.next();
        if (it.key()) {
            it.key()->kill();
        }
    }

}


QString BlipService::upload(const BlipVideo *video, const QString &account, const QString &password)
{

    if (video->title().isEmpty()) {
        return "Error: "+i18n("No title specified");
    }

    if (video->file().isEmpty() || !QFile::exists(video->file())) {
        return "Error: "+i18n("No video specified");
    }

    if (account.isEmpty() || password.isEmpty()) {
        return "Error: "+i18n("No account/password specified");
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
    fields["categories_id"] = QString::number(video->category());
    fields["license"] = QString::number(video->license());


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

    m_jobs[post(url, header, data)] = id;

    return id;

}


void BlipService::jobFinished(KJob *job, const QByteArray &data)
{

    const QString id = m_jobs[job];
    const int ret = job->error();
    QString text = data;
    QString errorString;

    m_jobs.remove(job);

    kDebug() << "response:" << data;

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

}


