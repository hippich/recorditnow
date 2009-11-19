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


#ifndef BLIPSERVICE_H
#define BLIPSERVICE_H


// own
#include "service.h"

// KDE
#include <kdemacros.h>

// Qt
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QPointer>


namespace KIO {
    class Job;
};
class KJob;
class BlipVideo;
class KDE_EXPORT BlipService : public KYouBlip::Service
{
    Q_OBJECT


public:
    BlipService(QObject *parent = 0);
    ~BlipService();


public slots:
    QString upload(const BlipVideo *video, const QString &account, const QString &password);



private:
    QHash<KJob*, QString> m_jobs;


protected slots:
    void jobFinished(KJob *job, const QByteArray &data);


signals:
    void error(const QString &reason, const QString &id);
    void uploadFinished(const QString &id);
    void canceled(const QString &id);


};


#endif // BlipService_H
