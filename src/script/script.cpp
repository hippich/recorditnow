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
#include "script.h"
#include "scriptimporter.h"

// KDE
#include <kdebug.h>
#include <klocalizedstring.h>

// Qt
#include <QtScript/QScriptEngine>
#include <QtCore/QDir>
#include <QtCore/QFile>


namespace RecordItNow {


Script::Script(QObject *parent, const KPluginInfo &info)
    : QObject(parent),
    m_info(info),
    m_engine(0),
    m_importer(0)
{



}


Script::~Script()
{

    unload();

}


QScriptEngine *Script::engine() const
{

    Q_ASSERT(m_engine);
    return m_engine;

}


KPluginInfo Script::info() const
{

    return m_info;

}


bool Script::isRunning() const
{

    return m_engine != 0;

}


QString Script::name() const
{

    return info().name();

}


RecordItNow::Script::ScriptType Script::type() const
{

    const QString type = info().property("X-RecordItNow-ScriptType").toString();
    if (type == QLatin1String("Recorder")) {
        return RecordItNow::Script::RecorderType;
    } else if (type == QLatin1String("Encoder")) {
        return RecordItNow::Script::EncoderType;
    } else if (type == QLatin1String("UI")) {
        return RecordItNow::Script::UIType;
    } else {
        return RecordItNow::Script::InvalidType;
    }

}


bool Script::load()
{

    if (m_engine) {
        return false;
    }

    QString main = scriptFile(m_info);
    QByteArray content = scriptContent(main);
    if (content.isEmpty()) {
        return false;
    }

    m_engine = new QScriptEngine(this);
    m_engine->globalObject().setProperty("ScriptFile", main);
    m_importer = new RecordItNow::ScriptImporter(this);

    QScriptValue value = m_engine->evaluate(content, main);
    value = m_engine->evaluate("load();");

    if (m_engine->hasUncaughtException()) {
        handleException(value);

        delete m_engine;
        m_engine = 0;
        delete m_importer;
        m_importer = 0;

        return false;
    }

    connect(m_engine, SIGNAL(signalHandlerException(QScriptValue)), this,
            SLOT(signalHandlerException(QScriptValue)));

    return true;

}


void Script::unload()
{

    if (!m_engine) {
        return;
    }
    
    m_engine->evaluate("unload();");
    if (m_engine->isEvaluating()) {
        m_engine->abortEvaluation();
    }

    delete m_engine;
    delete m_importer;

    m_importer = 0;
    m_engine = 0;

}


void Script::handleException(const QScriptValue &exception)
{

    if (!m_engine->hasUncaughtException()) {
        return;
    }

    QString message = exception.toString();
    const int line = m_engine->uncaughtExceptionLineNumber();
    message = i18nc("%1 = script name, %2 = line number, %3 = error string",
                    "<u><b>The script \"%1\" has caused an error:</b></u>\nLine: %2: %3", name(), line, message);

    kDebug() << message;

    emit scriptError(message);

}


QString Script::scriptFile(const KPluginInfo &info)
{

    QString main = QFileInfo(info.entryPath()).absolutePath();
    main.append(QDir::separator());
    main.append("main.js");

    return main;

}


QByteArray Script::scriptContent(const QString &path)
{

    QFile file(path);
    if (!file.exists()) {
        kDebug() << "Script not found:" << path;
        return QByteArray();
    } else if (!file.open(QIODevice::ReadOnly)) {
        kDebug() << "Open failed:" << path << file.errorString();
        return QByteArray();
    }

    const QByteArray content = file.readAll();
    file.close();

    return content;

}


void Script::signalHandlerException(const QScriptValue &exception)
{

    handleException(exception);

}


} // namespace RecordItNow


#include "script.moc"
