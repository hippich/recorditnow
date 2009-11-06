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


#ifndef ABSTRACTUPLOADER_H
#define ABSTRACTUPLOADER_H


// own
#include "recorditnowplugin.h"

// KDE
#include <kdemacros.h>

// Qt
#include <QtCore/QVariantList>
#include <QtGui/QAction>


class KDE_EXPORT AbstractUploader : public RecordItNowPlugin
{
    Q_OBJECT


public:
    AbstractUploader(QObject *parent = 0, const QVariantList &args = QVariantList());
    ~AbstractUploader();

    virtual void show(const QString &file, QWidget *parent) = 0;


};


#endif // ABSTRACTUPLOADER_H
