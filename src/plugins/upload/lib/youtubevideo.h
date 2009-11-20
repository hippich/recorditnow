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

#ifndef YOUTUBEVIDEO_H
#define YOUTUBEVIDEO_H


// own
#include "video.h"

// KDE
#include <kurl.h>
#include <kdemacros.h>

// Qt
#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtCore/QStringList>


class KDE_EXPORT YouTubeVideo : public KYouBlip::Video
{
    Q_OBJECT
    friend class YouTubeService;


public:
    YouTubeVideo(QObject *parent = 0);
    ~YouTubeVideo();

    int viewCount() const;
    int raters() const;
    int favoriteCount() const;
    QStringList licenses() const;
    QStringList categorys() const;
    QString category() const;
    QString license() const { return QString(); }

    void setViewCount(const int &count);
    void setRaters(const int &raters);
    void setFavoriteCount(const int &favCount);
    void setCategory(const QString &category);
    void setLicense(const QString &) {}


private:
    QHash<QString, QString> m_categorys;
    QHash<QString, QVariant> m_data;
    QString m_category;

    QString getCategory() const;


};


#endif // YOUTUBEVIDEO_H
