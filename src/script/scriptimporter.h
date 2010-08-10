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

#ifndef RECORDITNOW_SCRIPTIMPORTER_H
#define RECORDITNOW_SCRIPTIMPORTER_H


// Qt
#include <QtScript/QScriptEngine>


namespace RecordItNow {


class Script;
class ScriptImporter : public QObject
{
    Q_OBJECT


public:
    explicit ScriptImporter(RecordItNow::Script *script, QObject *parent = 0);
    ~ScriptImporter();


public slots:
    bool importBinding(const QString &binding);
    bool include(const QString &relativeFilename);


private:
    RecordItNow::Script *m_script;
    QList<QObject*> m_ext;

    void registerObject(QObject *object, const QString &name);
    void registerClass(QScriptEngine::FunctionSignature func, const QString &name);


};


} // namespace RecordItNow



#endif // RECORDITNOW_SCRIPTIMPORTER_H
