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

#ifndef SCREENSHOTRECORDER_H
#define SCREENSHOTRECORDER_H

// own
#include "abstractrecorder.h"

// Qt
#include <QtCore/QVariantList>


class ScreenshotRecorder : public AbstractRecorder
{


public:
    ScreenshotRecorder(QObject *parent = 0, const QVariantList &args = QVariantList());
    ~ScreenshotRecorder();

    bool hasFeature(const AbstractRecorder::Feature &feature) const;

    void record(const AbstractRecorder::Data &d);
    void pause();
    void stop();


};


#endif // SCREENSHOTRECORDER_H