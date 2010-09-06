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


#ifndef RECORDITNOW_PLUGIN_P_H
#define RECORDITNOW_PLUGIN_P_H


// KDE
#include <kplugininfo.h>

// Qt
#include <QtCore/QObject>
#include <QtCore/QHash>


class KJob;
namespace RecordItNow {


class Plugin;
class PluginPrivate : public QObject
{
    Q_OBJECT
    friend class Plugin;


public:
    PluginPrivate(Plugin *plugin);
    ~PluginPrivate();


    KPluginInfo info;
    QHash<KJob*, int> jobs;

    int getUniqueId();
    void removeUniqueId(const int &id);


private:
    Plugin *q;
    QList<int> m_uniqueIds;


private slots:
    void jobFinishedInternal(KJob *job);


};


}



#endif // RECORDITNOW_PLUGIN_P_H
