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


// KDE
#include <kio/http.h>
#include <kio/job.h>
#include <kdemacros.h>

// Qt
#include <QtCore/QObject>
#include <QtCore/QPointer>


class KDE_EXPORT YouTubeService : public QObject
{
    Q_OBJECT


public:
    enum State {
        Idle = 0,
        Auth = 1,
        Upload = 2
    };

    YouTubeService(QObject *parent = 0);
    ~YouTubeService();

    YouTubeService::State state() const;
    bool isAuthenticated() const;

    void authenticate(const QString &account, const QString &password);
    void upload(const QHash<QString, QString> data);


private:
    State m_state;

    QPointer<KIO::Job> m_job;
    QByteArray m_bytes;
    QString m_auth;
    bool m_authenticated;


private slots:
    void data(KIO::Job *job, const QByteArray &data);
    void result(KJob *job);


signals:
    void error(const QString &reason);
    void authenticated();
    void finished();


};


#endif // YOUTUBESERVICE_H
