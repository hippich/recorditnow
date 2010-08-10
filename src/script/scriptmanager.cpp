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


// own
#include "scriptmanager.h"
#include "pluginmanager.h"
#include "script.h"
#include "recorderscriptadaptor.h"

// KDE
#include <kdebug.h>
#include <kstandarddirs.h>
#include <kconfig.h>
#include <kconfiggroup.h>
#include <kio/copyjob.h>
#include <kio/deletejob.h>

// Qt
#include <QtScript/QScriptEngine>
#include <QtCore/QDir>


namespace RecordItNow {


ScriptManager::ScriptManager(QObject *parent)
    : QObject(parent)
{

    reloadPluginList();

}


ScriptManager::~ScriptManager()
{

    qDeleteAll(m_scripts);

}


QList<KPluginInfo> ScriptManager::availableScripts() const
{

    QList<KPluginInfo> infos;
    foreach (const RecordItNow::Script *script, m_scripts) {
        infos.append(script->info());
    }
    return infos;

}


RecordItNow::Plugin *ScriptManager::loadRecorder(const KPluginInfo &info)
{

    RecordItNow::Script *scriptObj = 0;
    foreach (RecordItNow::Script *script, m_scripts) {
        if (script->info().name() == info.name()) {
            scriptObj = script;
            break;
        }
    }
    if (!scriptObj) {
        return 0;
    }

    RecordItNow::RecorderScriptAdaptor *adaptor = new RecordItNow::RecorderScriptAdaptor(scriptObj, this);
    if (!adaptor->initPlugin()) {
        delete adaptor;
        return 0;
    }
    return adaptor;

}


QString ScriptManager::scriptSaveLocation() const
{

    return KGlobal::dirs()->saveLocation("appdata", "scripts/", true);

}


void ScriptManager::reloadPluginList()
{

    qDeleteAll(m_scripts);
    m_scripts.clear();

    const QStringList dirs = KGlobal::dirs()->findDirs("appdata", "scripts/");
    QStringList scripts;

    kDebug() << "DIRS:" << dirs;

    foreach (const QString &path, dirs) {
        QDir dir(path);
        foreach (const QString &scriptDirPath, dir.entryList(QDir::Dirs)) {
            QDir scriptDir(dir.absoluteFilePath(scriptDirPath));
            foreach (const QString &file, scriptDir.entryList(QStringList() << "*.desktop", QDir::Files)) {
                scripts.append(scriptDir.absoluteFilePath(file));
            }
        }
    }

    KConfig cfg("recorditnowrc");
    KConfigGroup pCfg(&cfg, "Plugins");
    foreach (const QString &desktopFile, scripts) {
        KPluginInfo info(KService::serviceByStorageId(desktopFile));

        info.setConfig(pCfg);
        info.load(pCfg);

        RecordItNow::Script *script = new RecordItNow::Script(this, info);
        connect(script, SIGNAL(scriptError(QString)), this, SIGNAL(scriptError(QString)));

        m_scripts.append(script);
    }

}


void ScriptManager::reloadScripts()
{
    
    KConfig cfg("recorditnowrc");
    KConfigGroup pCfg(&cfg, "Plugins");
    foreach (RecordItNow::Script *script, m_scripts) {
        KPluginInfo info = script->info();
        info.setConfig(pCfg);
        info.load(pCfg);

        if (info.isPluginEnabled() &&
            !script->isRunning() &&
            script->type() == Script::UIType) { // load
            if (!script->load()) {
                // TODO
            }
        } else if (!info.isPluginEnabled() &&
                   script->isRunning() &&
                   script->type() == Script::UIType) { // unload
            script->unload();
        }
    }

}


bool ScriptManager::installScript(const QString &path)
{

    QDir scriptDir(path);
    if (scriptDir.entryList(QStringList() << "*.desktop").isEmpty() ||
        !scriptDir.entryList().contains("main.js")) {
        return false;
    }

    const QString installPath = scriptSaveLocation() + QDir::separator() + scriptDir.dirName();

    KIO::CopyJob *job = KIO::copy(KUrl(path), KUrl(installPath), KIO::HideProgressInfo);
    job->exec();

    const bool ok = job->error() == 0;
    delete job;

    return ok;

}


bool ScriptManager::uninstallScript(const QString &path)
{

    if (!path.startsWith(scriptSaveLocation())) {
        return false;
    }

    KIO::DeleteJob *job = KIO::del(KUrl(path), KIO::HideProgressInfo);
    job->exec();

    const bool ok = job->error() == 0;
    delete job;

    return ok;

}




} // namespace RecordItNow


#include "scriptmanager.moc"
