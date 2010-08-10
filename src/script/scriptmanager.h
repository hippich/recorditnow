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

#ifndef RECORDITNOW_SCRIPTMANAGER_H
#define RECORDITNOW_SCRIPTMANAGER_H


// Qt
#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtCore/QStringList>
#include <QtScript/QScriptEngine>

// KDE
#include <kplugininfo.h>


class QScriptEngine;
namespace RecordItNow {


class Script;
class Plugin;
class ScriptManager : public QObject
{
    Q_OBJECT


public:
    explicit ScriptManager(QObject *parent = 0);
    ~ScriptManager();

    QList<KPluginInfo> availableScripts() const;
    RecordItNow::Plugin *loadRecorder(const KPluginInfo &info);
    QString scriptSaveLocation() const;

    void reloadPluginList();
    void reloadScripts();

    bool installScript(const QString &path);
    bool uninstallScript(const QString &path);


private:
    QList<RecordItNow::Script*> m_scripts;


signals:
    void scriptError(const QString &message);


};


} // namespace RecordItNow


#endif // SCRIPTMANAGER_H
