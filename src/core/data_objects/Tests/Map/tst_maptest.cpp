/*Copyright 2010-2012 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include <QtCore/QString>
#include <QtTest/QtTest>
#include <string>
#include "gutil_map.h"
USING_NAMESPACE_GUTIL;
USING_NAMESPACE_GUTIL1(DataObjects);

class MapTest : public QObject
{
    Q_OBJECT

public:
    MapTest();

private Q_SLOTS:
    void test_basic_function();

    void test_iterators();

    void test_load();
};

MapTest::MapTest()
{
}

void MapTest::test_basic_function()
{
    Map<int, QString> map;
    map.Insert(1, "One");
    map.Insert(2, "Two");
    map.Insert(3, "Hello World!");
    QVERIFY2(map.At(1) == "One", map.At(1).toAscii());
    QVERIFY(map.At(2) == "Two");
    QVERIFY(map.At(3) == "Hello World!");
    QVERIFY(map.Contains(1));
    QVERIFY(map.Contains(2));
    QVERIFY(map.Contains(3));
    QVERIFY(!map.Contains(100));
    QVERIFY(map.Size() == 3);

    map[1] = "Fun";
    QVERIFY(map[1] == "Fun");

    map.Insert(1, "One");
    QVERIFY(map[1] == "One");

    map.Remove(3);
    QVERIFY(!map.Contains(3));
    QVERIFY(map[1] == "One");
    QVERIFY(map[2] == "Two");
    bool exception_caught(false);
    try
    {
        if(map.At(3) == "")
        {}
    }
    catch(const IndexOutOfRangeException<> &)
    {
        exception_caught = true;
    }
    QVERIFY(exception_caught);

    exception_caught = false;
    try
    {
        if(map[3] == "")
        {}
    }
    catch(const IndexOutOfRangeException<> &)
    {
        exception_caught = true;
    }
    QVERIFY(!exception_caught);

    // Try InsertMulti
    Map<int, QString>::iterator iter;
    map.Insert(100, "one");
    map.InsertMulti(100, "two");
    QVERIFY((iter = map.Search(100))->Value() == "two");
    QVERIFY(iter->Values().Size() == 2);

//    // Test our auto-remove key when the last value is popped
//    iter->Values().Pop();
//    iter->Values().Pop();
//    exception_caught = false;
//    try
//    {
//        if(map[100] == "")
//        {}
//    }
//    catch(const IndexOutOfRangeException &)
//    {
//        exception_caught = true;
//    }
//    QVERIFY(exception_caught);
}

void MapTest::test_iterators()
{
    Map<int, QString> map;
    map.Insert(1, "One");
    map.Insert(2, "Two");
    map.Insert(3, "Three");
    map.Insert(4, "Four");

    int cnt(0);
    for(Map<int, QString>::const_iterator iter(map.begin());
        iter;
        iter++, cnt++)
    {
        switch(iter->Key())
        {
        case 1:
            QVERIFY(iter->Values()[iter->Values().Size() - 1] == "One");
            break;
        case 2:
            QVERIFY(iter->Values()[iter->Values().Size() - 1] == "Two");
            break;
        case 3:
            QVERIFY(iter->Values()[iter->Values().Size() - 1] == "Three");
            break;
        case 4:
            QVERIFY(iter->Values()[iter->Values().Size() - 1] == "Four");
            break;
        default:
            QVERIFY2(false, QString("Unrecognized key: %1").arg(iter->Key()).toStdString().c_str());
            break;
        }
    }
    QVERIFY(cnt == map.Size());

    // Iterate in reverse order
    for(Map<int, QString>::const_iterator iter(--map.end());
        iter;
        iter--)
    {
        switch(iter->Key())
        {
        case 1:
            QVERIFY(iter->Values()[iter->Values().Size() - 1] == "One");
            break;
        case 2:
            QVERIFY(iter->Values()[iter->Values().Size() - 1] == "Two");
            break;
        case 3:
            QVERIFY(iter->Values()[iter->Values().Size() - 1] == "Three");
            break;
        case 4:
            QVERIFY(iter->Values()[iter->Values().Size() - 1] == "Four");
            break;
        default:
            QVERIFY2(false, QString("Unrecognized key: %1").arg(iter->Key()).toStdString().c_str());
            break;
        }
    }

    // Now insert the numbers in reverse order; we should still iterate 1 then 2
    map.Clear();
    map.Insert(2, "");
    map.Insert(1, "");
    map.Insert(0, "");
    cnt = 0;
    for(Map<int, QString>::const_iterator iter(map.begin());
        iter;
        iter++, cnt++)
    {
        QVERIFY(iter->Key() == cnt);
    }
}

void MapTest::test_load()
{

}

QTEST_APPLESS_MAIN(MapTest);

#include "tst_maptest.moc"
