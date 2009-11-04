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
#include "abstractencoder.h"

// KDE
#include <kstandarddirs.h>

// Qt
#include <QtCore/QFile>
#include <QtCore/QDir>


AbstractEncoder::AbstractEncoder(QObject *parent, const QVariantList &args)
    : QObject(parent)
{

    Q_UNUSED(args);

}


AbstractEncoder::~AbstractEncoder()
{



}


QString AbstractEncoder::getTemporaryFile(const QString &workDir) const
{

    QString tmpDir = workDir;
    if (tmpDir.isEmpty()) {
        tmpDir = KGlobal::dirs()->locateLocal("tmp", "");
    }

    if (tmpDir.isEmpty()) {
        tmpDir = QDir::homePath();
    }

    if (!tmpDir.endsWith('/')) {
        tmpDir.append('/');
    }
    QString path = (tmpDir+"recorditnow_tmp");

    path = unique(path);

    return path;

}


QString AbstractEncoder::unique(const QString &file) const
{

    QString path = file;
    while (QFile::exists(path)) {
        if (path.length() > 4 && path[path.length()-4] == '.') {
            path.insert(path.length()-4, "_");
        } else {
            path.append('_');
        }
    }
    return path;

}


#include "abstractencoder.moc"

