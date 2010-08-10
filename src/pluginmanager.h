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

#ifndef RECORDITNOW_PLUGINMANAGER_H
#define RECORDITNOW_PLUGINMANAGER_H


// Qt
#include <QtCore/QObject>

// KDE
#include <kplugininfo.h>


class AbstractRecorder;
class AbstractEncoder;


namespace RecordItNow {


class Plugin;
class PluginManager : public QObject
{
    Q_OBJECT


public:
    PluginManager(QObject *parent = 0);
    ~PluginManager();

    void init();

    RecordItNow::Plugin *loadPlugin(const QString &name);
    void unloadPlugin(RecordItNow::Plugin *plugin);

    QList<KPluginInfo> getList(const QString &category) const;
    QList<KPluginInfo> getRecorderList() const;
    QList<KPluginInfo> getEncoderList() const;


private:
    QHash<KPluginInfo, RecordItNow::Plugin*> m_plugins;

    void clear();
    void loadPluginList();
    void loadInfos(const QString &type);


signals:
    void pluginsChanged();


};


} // namespace RecordItNow


#endif // RECORDITNOW_PLUGINMANAGER_H
