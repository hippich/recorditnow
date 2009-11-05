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
#include "recorditnowplugin.h"

// KDE
#include <kstandarddirs.h>

// Qt
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QRegExp>


RecordItNowPlugin::RecordItNowPlugin(QObject *parent)
    : QObject(parent)
{



}


RecordItNowPlugin::~RecordItNowPlugin()
{



}


QString RecordItNowPlugin::getTemporaryFile(const QString &workDir) const
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


QString RecordItNowPlugin::unique(const QString &file) const
{

    QString result = file;
    const QRegExp rx("-[0-9]+$");
    const QRegExp frx("\\..{3}$");

    frx.indexIn(result);
    const QString format = frx.cap();
    result.remove(frx);

    while (QFile::exists(result+format)) {
        rx.indexIn(result);
        if (!rx.cap().isEmpty()) {
            int number = rx.cap().remove(0, 1).toInt();
            number++;
            result.remove(rx);
            result += '-';
            result += QString::number(number);
        } else {
            result += "-1";
        }
    }
    result += format;

    return result;

}

