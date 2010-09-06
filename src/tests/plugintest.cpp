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
#include "plugintest.h"

// KDE
#include <kdebug.h>

// Qt
#include <QtTest/QTest>
#include <QtCore/QDir>
#include <QtCore/QTemporaryFile>
#include <QtCore/QEventLoop>



namespace RecordItNow {


PluginTest::PluginTest(QObject *parent)
    : RecordItNow::Plugin(QVariantList() << qVariantFromValue(KPluginInfo()), parent)
{


}


void PluginTest::cleanup()
{

    m_finId = -2;
    m_errorString.clear();

}


void PluginTest::initTestCase()
{

    m_loop = new QEventLoop(this);
    m_id = -2;
    m_errorString = QString();

}


void PluginTest::testMove()
{

    QTemporaryFile tmp;
    QVERIFY(tmp.open());
    tmp.write("Foo Bar\n");
    tmp.close();

    QVERIFY(move("", "") == -1);
    QVERIFY(move(tmp.fileName(), "") == -1);

    QString to = tmp.fileName();
    while (QFile::exists(to)) {
        to.append('_');
    }


    m_id = move(tmp.fileName(), to);
    QVERIFY(m_id != -1);

    if (m_id != -1) {
        m_loop->exec();
    }

    QFile::remove(to);

    QVERIFY(m_id == m_finId);
    QVERIFY(m_errorString.isEmpty());

}


void PluginTest::testRemove()
{

    QTemporaryFile tmp;
    QVERIFY(tmp.open());
    tmp.write("Foo Bar\n");
    tmp.close();

    m_id = remove(tmp.fileName());
    QVERIFY(m_id != -1);

    if (m_id != -1) {
        m_loop->exec();
    }

    tmp.remove();

    QVERIFY(m_id == m_finId);
    QVERIFY(m_errorString.isEmpty());

}


void PluginTest::testGetTemporaryFile()
{

    QString file = getTemporaryFile(QDir::homePath());
    QVERIFY(!QFile::exists(file));

}


void PluginTest::testUnique()
{

    QTemporaryFile tmp;
    QVERIFY(tmp.open());
    QString file = tmp.fileName();
    QVERIFY(QFile::exists(file));
    file = unique(file);
    QVERIFY(!QFile::exists(file));
    tmp.close();
    QVERIFY(tmp.remove());

}


void PluginTest::cleanupTestCase()
{

    delete m_loop;

}


void PluginTest::jobFinished(const int &id, const QString &errorString)
{

    m_errorString = errorString;
    m_finId = id;

    m_loop->exit();

}



} // namespace RecordItNow


QTEST_MAIN(RecordItNow::PluginTest)


#include "plugintest.moc"
