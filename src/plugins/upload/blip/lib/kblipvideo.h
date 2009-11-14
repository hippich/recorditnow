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


#ifndef KBLIPVIDEO_H
#define KBLIPVIDEO_H


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
class KBlipAccount;
class KDE_EXPORT KBlipVideo : public QObject
{
    Q_OBJECT


public:
    enum License {
        NoLicense = -1,
        CreativeCommonsAttribution_2_0 = 1,
        CreativeCommonsAttribution_NoDerivs_2_0 = 2,
        CreativeCommonsAttribution_NonCommercial_NoDerivs_2_0 = 3,
        CreativeCommonsAttribution_NonCommercial_2_0 = 4,
        CreativeCommonsAttribution_NonCommercial_ShareAlike_2_0 = 5,
        CreativeCommonsAttribution_ShareAlike_2_0 = 6,
        PublicDomain = 7,
        CreativeCommonsAttribution_3_0 = 9,
        CreativeCommonsAttribution_NoDerivs_3_0 = 10,
        CreativeCommonsAttribution_NonCommercial_NoDerivs_3_0 = 11,
        CreativeCommonsAttribution_NonCommercial_3_0 = 12,
        CreativeCommonsAttribution_NonCommercial_ShareAlike_3_0 = 13,
        CreativeCommonsAttribution_ShareAlike_3_0 = 14
    };

    enum Category {
        Art = 27,
        AutosAndVehicles = 29,
        Business = 26,
        CitizenJournalism = 4,
        Comedy = 2,
        ConferencesAndOtherEvents = 10,
        DefaultCategory = -1,
        Documentary = 19,
        Educational = 21,
        FoodAndDrink = 12,
        Friends = 15,
        Gaming = 13,
        Health = 25,
        Literature = 11,
        MoviesAndTelevision = 28,
        MusicAndEntertainment = 5,
        PersonalOrAutoBiographical = 8,
        Politics = 3,
        Religion = 23,
        SchoolandEducation = 16,
        Science = 14,
        Sports = 6,
        Technology = 7,
        TheEnvironment = 22,
        TheMainstreamMedia = 18,
        Travel = 24,
        Videoblogging = 9,
        WebDevelopmentandSites = 17
    };

    KBlipVideo(QObject *parent = 0);
    ~KBlipVideo();


public slots:
    void setTitle(const QString &title);
    void setDescription(const QString &description);
    void setTags(const QStringList &tags);
    void setLicense(const KBlipVideo::License &license);
    void setCategory(const KBlipVideo::Category &id);
    void setFile(const QString &file);

    void send(const KBlipAccount *account);


private:
    QString m_file;
    QString m_title;
    QString m_description;
    QString m_tags;
    int m_category;
    int m_license;
    QPointer<KJob> m_job;

    QByteArray m_bytes;


private slots:
    void jobData(KIO::Job *job, const QByteArray &data);
    void jobInfoMessage(KJob*, const QString &message);
    void jobResult(KJob *job);
    void jobSpeed(KJob *job, unsigned long speed);


signals:
    void finished();
    void error(const QString &errorString);
    void speed(const QString &speed);


};


#endif // KBLIPVIDEO_H
