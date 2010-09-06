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

#ifndef RECORDITNOW_PLUGINTEST_H
#define RECORDITNOW_PLUGINTEST_H


// own
#include "plugin.h"


class QEventLoop;
namespace RecordItNow {


class PluginTest : public RecordItNow::Plugin
{
    Q_OBJECT


public:
    explicit PluginTest(QObject *parent = 0);


private:
    QEventLoop *m_loop;
    int m_id;
    QString m_errorString;
    int m_finId;


private slots:
    void cleanup();
    void initTestCase();
    void testMove();
    void testRemove();
    void testGetTemporaryFile();
    void testUnique();
    void cleanupTestCase();


protected slots:
    void jobFinished(const int &id, const QString &errorString);


};


} // namespace RecordItNow


#endif // RECORDITNOW_PLUGINTEST_H
