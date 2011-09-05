/*Copyright 2011 George Karagoulis

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
#include "Core/DataObjects/map.h"
GUTIL_USING_CORE_NAMESPACE(DataObjects);

class MapTest : public QObject
{
    Q_OBJECT

public:
    MapTest();

private Q_SLOTS:
    void test_basic_function();

    void test_iterators();
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
    QVERIFY(map.At(1) == "One");
    QVERIFY(map.At(2) == "Two");
    QVERIFY(map.At(3) == "Hello World!");
}

void MapTest::test_iterators()
{

}

QTEST_APPLESS_MAIN(MapTest);

#include "tst_maptest.moc"
