/***************************************************************************
 *   Copyright (C) 2010 by Kai Dombrowe <just89@gmx.de>                    *
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

#ifndef FRAMESIZE_H
#define FRAMESIZE_H

// own
#include "configitem.h"

// KDE
#include <kdemacros.h>

// Qt
#include <QtCore/QString>
#include <QtCore/QSize>
#include <QtCore/QMetaType>


class KDE_EXPORT FrameSize : public RecordItNow::ConfigItem
{


public:
    FrameSize(const QSize &size, const QString &text);
    FrameSize();
    ~FrameSize();

    QSize size() const;
    QString text() const;

    void setSize(const QSize &size);
    void setText(const QString &text);


};


Q_DECLARE_METATYPE(FrameSize)


#endif // FRAMESIZE_H
