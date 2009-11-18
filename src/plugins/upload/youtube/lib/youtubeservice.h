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


#ifndef YOUTUBESERVICE_H
#define YOUTUBESERVICE_H


// own
#include "service.h"
#include "youtubevideo.h"

// KDE
#include <kio/http.h>
#include <kio/job.h>
#include <kdemacros.h>

// Qt
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QPair>


class QXmlStreamReader;
class KDE_EXPORT YouTubeService : public KoogleData::Service
{
    Q_OBJECT


public:    
    YouTubeService(QObject *parent = 0);
    ~YouTubeService();

    bool isAuthenticated(const QString &account) const;
    static QString getUniqueId();

    QString authenticate(const QString &account, const QString &password);
    QString upload(const YouTubeVideo *video, const QString &account);
    QString search(const QString &key,  const QString &author = QString(), const int &start = 1,
                   const int &max = 10);
    QString getFavorites(const QString &user, const int &max);


private:
    enum JobType {
        AuthJob = 0,
        UploadJob = 1,
        SearchJob = 2,
        FavoritesJob = 3
    };
    typedef QPair<JobType, QString> JobData;

    QHash<KJob*, JobData> m_jobs;
    QHash<QString, QString> m_accountIds;
    QHash<QString, QString> m_token;
    bool m_authenticated;

    YouTubeVideo *readEntry(QXmlStreamReader *reader);


private slots:


protected slots:
    void jobFinished(KJob *job, const QByteArray &data);


signals:
    void error(const QString &reason, const QString &id);
    void authenticated(const QString &id);
    void uploadFinished(const QString &id);
    void searchFinished(const QList<YouTubeVideo*> &videos, const QString &id);
    void favoritesFinished(const QList<YouTubeVideo*> &videos, const QString &id);
    void canceled(const QString &id);


};


#endif // YOUTUBESERVICE_H
