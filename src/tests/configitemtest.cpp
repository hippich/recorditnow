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
#include "configitemtest.h"
#include "src/configitem.h"

// Qt
#include <QtTest/QTest>


namespace RecordItNow {


class TestItem : public RecordItNow::ConfigItem
{
public:
    TestItem() : RecordItNow::ConfigItem() {}
    QString getTestData(const QString &key) const
    {

        return data<QString>(key);

    }
    void setTestData(const QString &key, const QString &value)
    {

        setData(key, value);

    }
};


#define TEST_KEY QString("__TEST_KEY")
#define TEST_VALUE QString("__TEST_VALUE")
ConfigItemTest::ConfigItemTest(QObject *parent)
    : QObject(parent)
{


}


void ConfigItemTest::cleanup()
{


}


void ConfigItemTest::initTestCase()
{


}


void ConfigItemTest::testData()
{

    TestItem item;
    item.setTestData(TEST_KEY, TEST_VALUE);
    QCOMPARE(TEST_VALUE, item.getTestData(TEST_KEY));

}


void ConfigItemTest::testItemOperator()
{

    TestItem item;
    TestItem item2;

    item.setTestData(TEST_KEY, TEST_VALUE);
    item2.setTestData(TEST_KEY, TEST_VALUE);

    QVERIFY(item == item2);

    item2.setTestData(TEST_KEY, QString());
    QVERIFY(item != item2);

    item2 = TestItem(item);
    QVERIFY(item == item2);


    QByteArray data;
    QDataStream in(&data, QIODevice::WriteOnly);
    in << item;

    QDataStream out(&data, QIODevice::ReadOnly);
    item2 = TestItem();
    out >> item2;
    QVERIFY(item == item2);

}


void ConfigItemTest::cleanupTestCase()
{


}


} // namespace RecordItNow


QTEST_MAIN(RecordItNow::ConfigItemTest)


#include "configitemtest.moc"
