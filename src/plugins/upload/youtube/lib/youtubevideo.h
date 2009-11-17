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


// KDE
#include <kurl.h>
#include <kdemacros.h>

// Qt
#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtCore/QStringList>


class KDE_EXPORT YouTubeVideo : public QObject
{
    Q_OBJECT


public:
    YouTubeVideo(QObject *parent = 0);
    ~YouTubeVideo();

    QString title() const;
    QString description() const;
    QStringList keywords() const;
    KUrl url() const;
    QString category() const;
    int duration() const;
    QString author() const;
    KUrl thumbnailUrl() const;
    int viewCount() const;
    QDateTime published() const;
    QString file() const;

    void setTitle(const QString &title);
    void setDescription(const QString &description);
    void setKeywords(const QString &keywords);
    void setUrl(const KUrl &url);
    void setCategory(const QString &category);
    void setDuration(const int &duration);
    void setAuthor(const QString &author);
    void setThumbnail(const KUrl &url);
    void setViewCount(const int &count);
    void setPublished(const QDateTime &date);
    void setFile(const QString &file);


private:
    QHash<QString, QVariant> m_data;


};

#endif // YOUTUBEVIDEO_H
