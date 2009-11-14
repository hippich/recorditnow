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
#include "kblipvideo.h"
#include "kblipaccount.h"

// KDE
#include <kdebug.h>
#include <kio/job.h>
#include <klocale.h>
#include <klocalizedstring.h>

// Qt
#include <QtCore/QFile>
#include <QtCore/QHash>


KBlipVideo::KBlipVideo(QObject *parent) :
    QObject(parent)
{



}


KBlipVideo::~KBlipVideo()
{

    if (m_job) {
        m_job->kill();
    }

}


void KBlipVideo::setTitle(const QString &title)
{

    m_title = title;

}


void KBlipVideo::setDescription(const QString &description)
{

    m_description = description;

}


void KBlipVideo::setTags(const QStringList &tags)
{

    m_tags = tags.join(", ");

}


void KBlipVideo::setLicense(const KBlipVideo::License &license)
{

    m_license = static_cast<int>(license);

}


void KBlipVideo::setCategory(const KBlipVideo::Category &id)
{

    m_category = static_cast<int>(id);

}


void KBlipVideo::setFile(const QString &file)
{

    m_file = file;

}


void KBlipVideo::send(const KBlipAccount *account)
{

    if (m_title.isEmpty()) {
        emit error(i18n("No title specified"));
        return;
    }

    if (m_file.isEmpty() || !QFile::exists(m_file)) {
        emit error(i18n("No video specified"));
        return;
    }

    if (account->m_username.isEmpty() || account->m_password.isEmpty()) {
        emit error(i18n("No Account or Password specified"));
        return;
    }

    KUrl url("http://blip.tv/file/post");
    QByteArray BOUNDARY = "-----------$$SomeFancyBoundary$$";
    QByteArray CRLF = "\r\n";


    QHash<QString, QString> fields;
    fields["post"] = "1";
    fields["skin"] = "xmlhttprequest";
    fields["file_role"] = "Web";
    fields["item_type"] = "file";

    fields["userlogin"] = account->m_username;
    fields["password"] = account->m_password;
    fields["title"] = m_title;
    fields["description"] = m_description;
    fields["topics"] = m_tags;
    fields["categories_id"] = QString::number(m_category);
    fields["license"] = QString::number(m_license);


    QHash<QString, QString> files;
    files["file"] = m_file;


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
            emit error(i18nc("%1 = file, %2 = error string", "Cannot open %1.\n"
                             "Reason: %1", fit.value(), file.errorString()));
            return;
        }
        data.append(file.readAll());
        data.append(CRLF);
    }

    data.append("--" + BOUNDARY + "--");
    data.append(CRLF);

    KIO::MetaData meta;
    meta.insert("content-type", "Content-type: multipart/form-data; boundary="+BOUNDARY);
    meta.insert("content-length", QString::number(data.size()));

    KIO::Job *job = KIO::http_post(url, data);
    job->addMetaData(meta);
    job->setAutoDelete(true);

    connect(job, SIGNAL(data(KIO::Job*, QByteArray)), SLOT(jobData(KIO::Job*, QByteArray)));
    connect(job, SIGNAL(result(KJob*)), SLOT(jobResult(KJob*)));
    connect(job, SIGNAL(infoMessage(KJob*, QString, QString)), SLOT(jobInfoMessage(KJob*, QString)));
    connect(job, SIGNAL(speed(KJob*, unsigned long)), this, SLOT(jobSpeed(KJob*, unsigned long)));

    m_job = job;

}


void KBlipVideo::jobData(KIO::Job *, const QByteArray &data)
{

    m_bytes += data;

}


void KBlipVideo::jobInfoMessage(KJob *, const QString &message)
{

    kDebug() << "info message:" << message;

}


void KBlipVideo::jobResult(KJob *job)
{

    kDebug() << "data:" << m_bytes;
    kDebug() << "error:" << job->error();

    QString text = m_bytes;
    QString errorString;

    if (text.contains("error")) {
        const QRegExp rx("<error>.*</error>");
        rx.indexIn(text);
        errorString = rx.cap();
        errorString.remove("<error>");
        errorString.remove("</error>");
    }

    if (job->error() || !errorString.isEmpty()) {
        if (!errorString.isEmpty()) {
            emit error(errorString);
        } else {
            emit error(job->errorString());
        }
    } else {
        emit finished();
    }

}


void KBlipVideo::jobSpeed(KJob *, unsigned long bytes)
{

    emit speed(KGlobal::locale()->formatByteSize(bytes));

}
