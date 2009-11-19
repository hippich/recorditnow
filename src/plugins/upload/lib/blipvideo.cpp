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


BlipVideo::BlipVideo(QObject *parent)
    : KYouBlip::Video(parent)
{


    m_licenses[BlipVideo::NoLicense] = i18n("No License");
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


    m_categorys[BlipVideo::Art] = i18n("Art");
    m_categorys[BlipVideo::AutosAndVehicles] = i18n("Autos & Vehicles");
    m_categorys[BlipVideo::Business] = i18n("Business");
    m_categorys[BlipVideo::CitizenJournalism] = i18n("Citizen Journalism");
    m_categorys[BlipVideo::Comedy] = i18n("Comedy");
    m_categorys[BlipVideo::ConferencesAndOtherEvents] = i18n("Conferences and other Events");
    m_categorys[BlipVideo::DefaultCategory] = i18n("Default Category");
    m_categorys[BlipVideo::Documentary] = i18n("Documentary");
    m_categorys[BlipVideo::Educational] = i18n("Educational");
    m_categorys[BlipVideo::FoodAndDrink] = i18n("Food & Drink");
    m_categorys[BlipVideo::Friends] = i18n("Friends");
    m_categorys[BlipVideo::Gaming] = i18n("Gaming");
    m_categorys[BlipVideo::Health] = i18n("Health");
    m_categorys[BlipVideo::Literature] = i18n("Literature");
    m_categorys[BlipVideo::MoviesAndTelevision] = i18n("Movies and Television");
    m_categorys[BlipVideo::MusicAndEntertainment] = i18n("Music and Entertainment");
    m_categorys[BlipVideo::PersonalOrAutoBiographical] = i18n("Personal or Auto-biographical");
    m_categorys[BlipVideo::Politics] = i18n("Politics");
    m_categorys[BlipVideo::Religion] = i18n("Religion");
    m_categorys[BlipVideo::SchoolAndEducation] = i18n("School and Education");
    m_categorys[BlipVideo::Science] = i18n("Science");
    m_categorys[BlipVideo::Sports] = i18n("Sports");
    m_categorys[BlipVideo::Technology] = i18n("Technology");
    m_categorys[BlipVideo::TheEnvironment] = i18n("The Environment");
    m_categorys[BlipVideo::TheMainstreamMedia] = i18n("The Mainstream Media");
    m_categorys[BlipVideo::Travel] = i18n("Travel");
    m_categorys[BlipVideo::Videoblogging] = i18n("Videoblogging");
    m_categorys[BlipVideo::WebDevelopmentAndSites] = i18n("Web Development and Sites");

}


BlipVideo::~BlipVideo()
{


}


QStringList BlipVideo::categorys() const
{

    return m_categorys.values();

}


QStringList BlipVideo::licenses() const
{

    return m_licenses.values();

}

