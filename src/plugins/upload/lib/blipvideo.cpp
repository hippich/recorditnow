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
#include "blipvideo.h"

// KDE
#include <kdebug.h>
#include <kio/job.h>
#include <kcodecs.h>

// Qt
#include <QtCore/QDateTime>
#include <QtCore/QDir>


BlipVideo::BlipVideo(QObject *parent)
    : KYouBlip::Service(parent)
{



}


BlipVideo::~BlipVideo()
{


}


QString BlipVideo::title() const
{

    return m_data["Title"].toString();

}


QString BlipVideo::description() const
{

    return m_data["Description"].toString();

}


QStringList BlipVideo::keywords() const
{

    return m_data["Keywords"].toString().split(',');

}


KUrl BlipVideo::url() const
{

    return m_data["Url"].value<KUrl>();

}


BlipVideo::Category BlipVideo::category() const
{

    return static_cast<BlipVideo::Category>(m_data["Category"].toInt());

}


QString BlipVideo::file() const
{

    return m_data["File"].toString();

}


BlipVideo::License BlipVideo::license() const
{

    return static_cast<BlipVideo::License>(m_data["License"].toInt());

}


void BlipVideo::setTitle(const QString &title)
{

    m_data["Title"] = title;

}


void BlipVideo::setDescription(const QString &description)
{

    m_data["Description"] = description;

}


void BlipVideo::setKeywords(const QString &keywords)
{

    m_data["Keywords"] = keywords;

}


void BlipVideo::setUrl(const KUrl &url)
{

    m_data["Url"] = url;

}


void BlipVideo::setCategory(const BlipVideo::Category &category)
{

    m_data["Category"] = static_cast<int>(category);

}


void BlipVideo::setFile(const QString &file)
{

    m_data["File"] = file;

}


void BlipVideo::setLicense(const BlipVideo::License &license)
{

    m_data["License"] = static_cast<int>(license);

}
