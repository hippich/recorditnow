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
#include "youtubevideo.h"

// Qt
#include <QtCore/QDateTime>


YouTubeVideo::YouTubeVideo(QObject *parent)
    : QObject(parent)
{



}


YouTubeVideo::~YouTubeVideo()
{


}


QString YouTubeVideo::title() const
{

    return m_data["Title"].toString();

}


QString YouTubeVideo::description() const
{

    return m_data["Description"].toString();

}


QStringList YouTubeVideo::keywords() const
{

    return m_data["Keywords"].toString().split(',');

}


KUrl YouTubeVideo::url() const
{

    return m_data["Url"].toUrl();

}


QString YouTubeVideo::category() const
{

    return m_data["Category"].toString();

}


int YouTubeVideo::duration() const
{

    return m_data["Duration"].toInt();

}


QString YouTubeVideo::author() const
{

    return m_data["Author"].toString();

}


KUrl YouTubeVideo::thumbnailUrl() const
{

    return m_data["ThumbnailUrl"].toUrl();

}


int YouTubeVideo::viewCount() const
{

    return m_data["ViewCount"].toInt();

}


QDateTime YouTubeVideo::published() const
{

    return m_data["Published"].toDateTime();

}


QString YouTubeVideo::file() const
{

    return m_data["File"].toString();

}


void YouTubeVideo::setTitle(const QString &title)
{

    m_data["Title"] = title;

}


void YouTubeVideo::setDescription(const QString &description)
{

    m_data["Description"] = description;

}


void YouTubeVideo::setKeywords(const QString &keywords)
{

    m_data["Keywords"] = keywords;

}


void YouTubeVideo::setUrl(const KUrl &url)
{

    m_data["Url"] = url;

}


void YouTubeVideo::setCategory(const QString &category)
{

    m_data["Category"] = category;

}


void YouTubeVideo::setDuration(const int &duration)
{

    m_data["Duration"] = QString::number(duration);

}


void YouTubeVideo::setAuthor(const QString &author)
{

    m_data["Author"] = author;

}


void YouTubeVideo::setThumbnail(const KUrl &url)
{

    m_data["ThumbnailUtl"] = url;

}


void YouTubeVideo::setViewCount(const int &count)
{

    m_data["ViewCount"] = count;

}


void YouTubeVideo::setPublished(const QDateTime &date)
{

    m_data["Published"] = date;

}


void YouTubeVideo::setFile(const QString &file)
{

    m_data["File"] = file;

}

