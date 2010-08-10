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

#ifndef RECORDITNOW_SCRIPT_H
#define RECORDITNOW_SCRIPT_H


// KDE
#include <kplugininfo.h>


// Qt
#include <QtCore/QObject>
#include <QtScript/QScriptValue>


class QScriptEngine;
namespace RecordItNow {


class Script : public QObject
{
    Q_OBJECT


public:
    enum ScriptType {
        InvalidType = -1,
        RecorderType = 0,
        EncoderType = 1,
        UIType = 2
    };

    explicit Script(QObject *parent, const KPluginInfo &info);
    ~Script();

    QScriptEngine *engine() const;
    KPluginInfo info() const;
    bool isRunning() const;
    QString name() const;
    RecordItNow::Script::ScriptType type() const;


    bool load();
    void unload();
    void addAdaptor(QObject *adaptor, const QString &name);
    void handleException(const QScriptValue &exception);

    static QString scriptFile(const KPluginInfo &info);
    static QByteArray scriptContent(const QString &path);


private:
    KPluginInfo m_info;
    QScriptEngine *m_engine;
    QObject *m_importer;


private slots:
    void signalHandlerException(const QScriptValue &exception);


signals:
    void scriptError(const QString &error);


};


} // namespace RecordItNow


#endif // RECORDITNOW_SCRIPT_H
