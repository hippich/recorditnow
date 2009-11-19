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

// KDE
#include <kdebug.h>
#include <klocalizedstring.h>



YouTubeVideo::YouTubeVideo(QObject *parent)
    : KYouBlip::Video(parent)
{

    m_categorys["Autos"] = i18n("Autos & Vehicles");
    m_categorys["Comedy"] = i18n("Comedy");
    m_categorys["Education"] = i18n("Education");
    m_categorys["Entertainment"] = i18n("Entertainment");
    m_categorys["Film"] = i18n("Film & Animation");
    m_categorys["Games"] = i18n("Gaming");
    m_categorys["Howto"] = i18n("Howto & Style");
    m_categorys["Music"] = i18n("Music");
    m_categorys["News"] = i18n("News & Politics");
    m_categorys["Nonprofit"] = i18n("Nonprofit & Activism");
    m_categorys["People"] = i18n("People & Blogs");
    m_categorys["Animals"] = i18n("Pets & Animals");
    m_categorys["Tech"] = i18n("Sience & Technology");
    m_categorys["Sports"] = i18n("Sports");
    m_categorys["Travel"] = i18n("Travel & Evends");

}


YouTubeVideo::~YouTubeVideo()
{


}


int YouTubeVideo::viewCount() const
{

    return m_data["ViewCount"].toInt();

}


int YouTubeVideo::raters() const
{

    return m_data["Raters"].toInt();

}


int YouTubeVideo::favoriteCount() const
{

    return m_data["FavCount"].toInt();

}


QStringList YouTubeVideo::categorys() const
{

    return m_categorys.values();

}


QStringList YouTubeVideo::licenses() const
{

    return QStringList();

}


void YouTubeVideo::setViewCount(const int &count)
{

    m_data["ViewCount"] = count;

}


void YouTubeVideo::setRaters(const int &raters)
{

    m_data["Raters"] = raters;

}


void YouTubeVideo::setFavoriteCount(const int &favCount)
{

    m_data["FavCount"] = favCount;

}

