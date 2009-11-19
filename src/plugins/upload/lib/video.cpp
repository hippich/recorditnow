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
#include "video.h"

// KDE
#include <kdebug.h>
#include <kio/job.h>
#include <kcodecs.h>

// Qt
#include <QtCore/QDateTime>
#include <QtCore/QDir>


namespace KYouBlip {


Video::Video(QObject *parent)
    : KYouBlip::Service(parent)
{



}


Video::~Video()
{


}


QString Video::title() const
{

    return m_data["Title"].toString();

}


QString Video::description() const
{

    return m_data["Description"].toString();

}


QStringList Video::keywords() const
{

    return m_data["Keywords"].toString().split(',');

}


KUrl Video::url() const
{

    return m_data["Url"].value<KUrl>();

}


QString Video::category() const
{

    return m_data["Category"].toString();

}


int Video::duration() const
{

    return m_data["Duration"].toInt();

}


QString Video::author() const
{

    return m_data["Author"].toString();

}


KUrl Video::thumbnailUrl() const
{

    return m_data["ThumbnailUrl"].value<KUrl>();

}


QDateTime Video::published() const
{

    return m_data["Published"].toDateTime();

}


QString Video::file() const
{

    return m_data["File"].toString();

}


QString Video::thumbnail() const
{

    return m_data["ThumbnailFile"].toString();

}


double Video::rating() const
{

    return m_data["Rating"].toDouble();

}


QString Video::license() const
{

    return m_data["License"].toString();

}


void Video::setTitle(const QString &title)
{

    m_data["Title"] = title;

}


void Video::setDescription(const QString &description)
{

    m_data["Description"] = description;

}


void Video::setKeywords(const QString &keywords)
{

    m_data["Keywords"] = keywords;

}


void Video::setUrl(const KUrl &url)
{

    m_data["Url"] = url;

}


void Video::setCategory(const QString &category)
{

    m_data["Category"] = category;

}


void Video::setDuration(const int &duration)
{

    m_data["Duration"] = QString::number(duration);

}


void Video::setAuthor(const QString &author)
{

    m_data["Author"] = author;

}


void Video::setThumbnailUrl(const KUrl &url)
{

    m_data["ThumbnailUrl"] = url;

}


void Video::setPublished(const QDateTime &date)
{

    m_data["Published"] = date;

}


void Video::setFile(const QString &file)
{

    m_data["File"] = file;

}


void Video::setThumbnail(const QString &file)
{

    m_data["ThumbnailFile"] = file;

}


void Video::setRating(const double &rating)
{

    m_data["Rating"] = rating;

}


void Video::setLicense(const QString &license)
{

    m_data["License"] = license;

}


void Video::updateThumbnail(const QString &thumbnailDir)
{

    QDir dir(thumbnailDir);
    if (thumbnailDir.isEmpty() || !dir.exists()) {
        kWarning() << "invalid thumnail dir:" << thumbnailDir;
        emit thumbnailUpdateFailed();
        return;
    }

    const KUrl url(thumbnailUrl());
    if (url.isEmpty()) {
        kWarning() << "empty url...";
        emit thumbnailUpdateFailed();
        return;
    }

    QFile old(thumbnail());
    if (old.exists()) {
        if (!old.remove()) {
            kWarning() << "remove() failed!";
            emit thumbnailUpdateFailed();
            return;
        }
    }
    setThumbnail(thumbnailDir);

    m_thumbnailJob = get(url, KIO::NoReload, true);

}


bool Video::loadThumbnail(const QString &thumbnailDir)
{

    const QString file = thumbnailDir+'/'+getMD5String();
    if (QFile::exists(file)) {
        setThumbnail(file);
        return true;
    } else {
        return false;
    }

}


QByteArray Video::getMD5String() const
{

    KMD5 context ((title()+'_'+description()).toLatin1());
    return context.hexDigest().data();

}


void Video::jobFinished(KJob *job, const QByteArray &data)
{

    if (job == m_thumbnailJob) {
        QFile file(thumbnail()+'/'+getMD5String());
        if (file.exists()) {
            emit thumbnailUpdateFailed();
            return;
        }

        if (!file.open(QIODevice::WriteOnly)) {
            kWarning() << "open() failed!";
            emit thumbnailUpdateFailed();
            return;
        }
        file.write(data);
        file.close();

        setThumbnail(file.fileName());

        emit thumbnailUpdated(file.fileName());
    }

}


}; // namespace KYouBlip
