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

#ifndef RECORDITNOW_PLUGIN_H
#define RECORDITNOW_PLUGIN_H


// KDE
#include <kdemacros.h>
#include <kplugininfo.h>

// Qt
#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtCore/QStringList>
#include <QtCore/QMetaType>


class KJob;
namespace RecordItNow {


class PluginPrivate;
class KDE_EXPORT Plugin : public QObject
{
    Q_OBJECT
    friend class PluginPrivate;


public:
    Plugin(const QVariantList &args, QObject *parent = 0);
    ~Plugin();


    KPluginInfo info() const;


private:
    RecordItNow::PluginPrivate *d;


protected:
    int move(const QString &from ,const QString &to);
    int remove(const QString &file);
    QString getTemporaryFile(const QString &workDir) const;
    QString unique(const QString &file) const;

    virtual void jobFinished(const int &id, const QString &errorString);


};


} // namespace RecordItNow


Q_DECLARE_METATYPE(KPluginInfo)


#endif // RECORDITNOW_PLUGIN_H
