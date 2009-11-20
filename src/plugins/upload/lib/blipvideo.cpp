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
#include <klocalizedstring.h>


#define CategoryIt QHashIterator<int, CategoryStrings> it(m_categorys);


BlipVideo::BlipVideo(QObject *parent)
    : KYouBlip::Video(parent)
{

    m_license = -5;
    m_category = -5;

    m_licenses[BlipVideo::NoLicense] = "No License";
    m_licenses[BlipVideo::CreativeCommonsAttribution_2_0] = "Creative Commons Attribution 2.0";
    m_licenses[BlipVideo::CreativeCommonsAttribution_NoDerivs_2_0] =
            "Creative Commons Attribution-NoDerivs 2.0";
    m_licenses[BlipVideo::CreativeCommonsAttribution_NonCommercial_NoDerivs_2_0] =
            "Creative Commons Attribution-NonCommercial-NoDerivs 2.0";
    m_licenses[BlipVideo::CreativeCommonsAttribution_NonCommercial_2_0] =
            "Creative Commons Attribution-NonCommercial 2.0";
    m_licenses[BlipVideo::CreativeCommonsAttribution_NonCommercial_ShareAlike_2_0] =
            "Creative Commons Attribution-NonCommercial-ShareAlike 2.0";
    m_licenses[BlipVideo::CreativeCommonsAttribution_ShareAlike_2_0] =
            "Creative Commons Attribution-ShareAlike 2.0";
    m_licenses[BlipVideo::PublicDomain] = "Public Domain";
    m_licenses[BlipVideo::CreativeCommonsAttribution_3_0] = "Creative Commons Attribution 3.0";
    m_licenses[BlipVideo::CreativeCommonsAttribution_NoDerivs_3_0] =
            "Creative Commons Attribution-NoDerivs 3.0";
    m_licenses[BlipVideo::CreativeCommonsAttribution_NonCommercial_NoDerivs_3_0] =
            "Creative Commons Attribution-NonCommercial-NoDerivs 3.0";
    m_licenses[BlipVideo::CreativeCommonsAttribution_NonCommercial_3_0] =
            "Creative Commons Attribution-NonCommercial 3.0";
    m_licenses[BlipVideo::CreativeCommonsAttribution_NonCommercial_ShareAlike_3_0] =
            "Creative Commons Attribution-NonCommercial-ShareAlike 3.0";
    m_licenses[BlipVideo::CreativeCommonsAttribution_ShareAlike_3_0] =
            "Creative Commons Attribution-ShareAlike 3.0";


    m_categorys[BlipVideo::Art] =
            qMakePair(QString("Art"), i18n("Art"));
    m_categorys[BlipVideo::AutosAndVehicles] =
            qMakePair(QString("Autos & Vehicles"), i18n("Autos & Vehicles"));
    m_categorys[BlipVideo::Business] =
            qMakePair(QString("Business"), i18n("Business"));
    m_categorys[BlipVideo::CitizenJournalism] =
            qMakePair(QString("Citizen Journalism"), i18n("Citizen Journalism"));
    m_categorys[BlipVideo::Comedy] =
            qMakePair(QString("Comedy"), i18n("Comedy"));
    m_categorys[BlipVideo::ConferencesAndOtherEvents] =
            qMakePair(QString("Conferences and other Events"), i18n("Conferences and other Events"));
    m_categorys[BlipVideo::DefaultCategory] =
            qMakePair(QString("Default Category"), i18n("Default Category"));
    m_categorys[BlipVideo::Documentary] =
            qMakePair(QString("Documentary"), i18n("Documentary"));
    m_categorys[BlipVideo::Educational] =
            qMakePair(QString("Educational"), i18n("Educational"));
    m_categorys[BlipVideo::FoodAndDrink] =
            qMakePair(QString("Food & Drink"), i18n("Food & Drink"));
    m_categorys[BlipVideo::Friends] =
            qMakePair(QString("Friends"), i18n("Friends"));
    m_categorys[BlipVideo::Gaming] =
            qMakePair(QString("Gaming"), i18n("Gaming"));
    m_categorys[BlipVideo::Health] =
            qMakePair(QString("Health"), i18n("Health"));
    m_categorys[BlipVideo::Literature] =
            qMakePair(QString("Literature"), i18n("Literature"));
    m_categorys[BlipVideo::MoviesAndTelevision] =
            qMakePair(QString("Movies and Television"), i18n("Movies and Television"));
    m_categorys[BlipVideo::MusicAndEntertainment] =
            qMakePair(QString("Music and Entertainment"), i18n("Music and Entertainment"));
    m_categorys[BlipVideo::PersonalOrAutoBiographical] =
            qMakePair(QString("Personal or Auto-biographical"), i18n("Personal or Auto-biographical"));
    m_categorys[BlipVideo::Politics] =
            qMakePair(QString("Politics"), i18n("Politics"));
    m_categorys[BlipVideo::Religion] =
            qMakePair(QString("Religion"), i18n("Religion"));
    m_categorys[BlipVideo::SchoolAndEducation] =
            qMakePair(QString("School and Education"), i18n("School and Education"));
    m_categorys[BlipVideo::Science] =
            qMakePair(QString("Science"), i18n("Science"));
    m_categorys[BlipVideo::Sports] =
            qMakePair(QString("Sports"), i18n("Sports"));
    m_categorys[BlipVideo::Technology] =
            qMakePair(QString("Technology"), i18n("Technology"));
    m_categorys[BlipVideo::TheEnvironment] =
            qMakePair(QString("The Environment"), i18n("The Environment"));
    m_categorys[BlipVideo::TheMainstreamMedia] =
            qMakePair(QString("The Mainstream Media"), i18n("The Mainstream Media"));
    m_categorys[BlipVideo::Travel] =
            qMakePair(QString("Travel"), i18n("Travel"));
    m_categorys[BlipVideo::Videoblogging] =
            qMakePair(QString("Videoblogging"), i18n("Videoblogging"));
    m_categorys[BlipVideo::WebDevelopmentAndSites] =
            qMakePair(QString("Web Development and Sites"), i18n("Web Development and Sites"));

}


BlipVideo::~BlipVideo()
{


}


QStringList BlipVideo::categorys() const
{

    CategoryIt
    QStringList list;
    while (it.hasNext()) {
        it.next();
        list.append(it.value().second);
    }
    return list;

}


QStringList BlipVideo::licenses() const
{

    return m_licenses.values();

}


QString BlipVideo::category() const
{

    return m_categorys[m_category].second;

}


QString BlipVideo::license() const
{

    return m_licenses[m_license];

}


void BlipVideo::setCategory(const QString &category)
{

    m_category = -5;
    CategoryIt
    while (it.hasNext()) {
        it.next();
        if (it.value().first == category || it.value().second == category) {
            m_category = it.key();
            break;
        }
    }
    if (m_category == -5) {
        kWarning() << "Invalid category:" << category;
    }

}


void BlipVideo::setLicense(const QString &license)
{

    QHashIterator<int, QString> it(m_licenses);
    while (it.hasNext()) {
        it.next();
        if (it.value() == license) {
            m_license = it.key();
            return;
        }
    }
    kWarning() << "Invalid license:" << license;
    m_license = -5;

}


QString BlipVideo::getCategoryCode() const
{

    return QString::number(m_category);

}


QString BlipVideo::getLicenseCode() const
{

    return QString::number(m_license);

}
