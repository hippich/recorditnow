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

#ifndef FFMPEGENCODERCONFIG_H
#define FFMPEGENCODERCONFIG_H


// own
#include "ui_ffmpeg.h"

// KDE
#include <kcmodule.h>


class FfmpegEncoderConfig : public KCModule
{


public:
    FfmpegEncoderConfig(QWidget *parent = 0, const QVariantList &args = QVariantList());
    ~FfmpegEncoderConfig();


private:
     Ui::Ffmpeg ui_cfg;


};


#endif // SCREENSHOTRECORDERCONFIG_H