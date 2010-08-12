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
#include "scriptimporter.h"
#include "script.h"
#include "mainwindowscriptadaptor.h"
#include "recorditnow_script_export.h"
#include "widgets/checkicon.h"
#include "debugscriptadaptor.h"
#include "configscriptadaptor.h"

// KDE
#include <kdebug.h>
#include <kplugininfo.h>

// Qt
#include <QtCore/QStringList>
#include <QtCore/QFile>
#include <QtCore/QDir>


RECORDITNOW_SCRIPT_CREATE_OBJECT(CheckIcon, "CheckIcon", QWidget*);


namespace RecordItNow {


ScriptImporter::ScriptImporter(RecordItNow::Script *script, QObject *parent)
    : QObject(parent), m_script(script)
{

    registerObject(this, "RecordItNowImporter");

}


ScriptImporter::~ScriptImporter()
{

    qDeleteAll(m_ext);
    m_ext.clear();

}


bool ScriptImporter::importBinding(const QString &binding)
{

    kDebug() << "IMPORT:" << binding;

    const QStringList qtBindings = QStringList() <<
                                   "qt.core" <<
                                   "qt.gui" <<
                                   "qt.sql" <<
                                   "qt.webkit" <<
                                   "qt.xml" <<
                                   "qt.uitools" <<
                                   "qt.network";

    const QStringList RecordItNowBindings = QStringList() <<
                                            "RecordItNow.MainWindow" <<
                                            "RecordItNow.Widgets" <<
                                            "RecordItNow.Debug" <<
                                            "RecordItNow.Config";


    if (qtBindings.contains(binding)) {
        m_script->engine()->importExtension(binding);
    } else if (RecordItNowBindings.contains(binding)) {
        if (binding == QLatin1String("RecordItNow.MainWindow")) {
            registerObject(new RecordItNow::MainWindowScriptAdaptor(this), "RecordItNowMainWindow");
        } else if (binding == QLatin1String("RecordItNow.Widgets")) {
            RECORDITNOW_SCRIPT_REGISTER(CheckIcon, "CheckIcon");
        } else if (binding == QLatin1String("RecordItNow.Debug")) {
            registerObject(new RecordItNow::DebugScriptAdaptor(this), "RecordItNowDebug");
        } else if (binding == QLatin1String("RecordItNow.Config")) {
            registerObject(new RecordItNow::ConfigScriptAdaptor(m_script->name(), this), "RecordItNowConfig");
        } else {
            return false;
        }
    } else {
        return false;
    }
    return true;

}


bool ScriptImporter::include(const QString &relativeFilename)
{

    kDebug() << "INCLUDE:" << relativeFilename;

    QString absolutePath = QFileInfo(m_script->info().entryPath()).absolutePath();
    absolutePath.append(QDir::separator());
    absolutePath.append(relativeFilename);

    QFile file(absolutePath);
    if (!file.exists()) {
        kDebug() << "no such include file:" << absolutePath;
        return false;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        kDebug() << "open failed:" << absolutePath << file.errorString();
        return false;
    }

    const QByteArray contents = file.readAll();
    file.close();

    QScriptEngine *engine = m_script->engine();
    engine->currentContext()->setActivationObject(engine->currentContext()->parentContext()->activationObject());
    engine->evaluate(contents, absolutePath);

    return true;

}


void ScriptImporter::registerObject(QObject *object, const QString &name)
{

    QScriptValue value = m_script->engine()->newQObject(object);
    m_script->engine()->globalObject().setProperty(name, value);

    if (object != this) {
        m_ext.append(object);
    }

}


void ScriptImporter::registerClass(QScriptEngine::FunctionSignature func, const QString &name)
{

    QScriptValue ctor = m_script->engine()->newFunction(func);
    QScriptValue metaObject = m_script->engine()->newQMetaObject(&QObject::staticMetaObject, ctor);
    m_script->engine()->globalObject().setProperty(name, metaObject);

}


} // namespace RecordItNow


#include "scriptimporter.moc"
