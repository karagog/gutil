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
#include "Core/DataObjects/interfaces.h"
#include "Core/DataObjects/vector.h"
#include "Core/DataObjects/list.h"
GUTIL_USING_CORE_NAMESPACE(DataObjects);

class RandomAccessContainerTest : public QObject
{
    Q_OBJECT

public:
    RandomAccessContainerTest();

private Q_SLOTS:
    void test_vectorRAC();
    void test_listRAC();

private:
    void _test_rac(RandomAccessContainer<int> &);
};

RandomAccessContainerTest::RandomAccessContainerTest()
{
}

void RandomAccessContainerTest::test_vectorRAC()
{
    VectorRandomAccessContainer<int> r;
    _test_rac(r);
}

void RandomAccessContainerTest::test_listRAC()
{
    ListRandomAccessContainer<int> r;
    _test_rac(r);
}

#define ITEM_COUNT 10000

void RandomAccessContainerTest::_test_rac(RandomAccessContainer<int> &r)
{
    for(int i(0); i < ITEM_COUNT; ++i)
    {
        r.InsertAt(i, i);
        QVERIFY(r.CountContainerItems() == (i + 1));

        // Check both at functions for correctness
        QVERIFY(r.At(0) == 0);
        QVERIFY(r.At(i) == i);
        QVERIFY(const_cast<const RandomAccessContainer<int> &>(r).At(0) == 0);
        QVERIFY(const_cast<const RandomAccessContainer<int> &>(r).At(i) == i);
    }

    for(int i(0); i < ITEM_COUNT; ++i)
    {
        // Check both front/back functions for correctness
        QVERIFY(r.At(0) == i);
        QVERIFY(r.At(ITEM_COUNT - i - 1) == ITEM_COUNT - 1);
        QVERIFY(const_cast<const RandomAccessContainer<int> &>(r).At(0) == i);
        QVERIFY(const_cast<const RandomAccessContainer<int> &>(r).At(ITEM_COUNT - i - 1) == ITEM_COUNT - 1);

        QVERIFY(r.CountContainerItems() == (ITEM_COUNT - i));
        r.RemoveAt(0);
    }


    for(int i(0); i < ITEM_COUNT; ++i)
    {
        r.InsertAt(i, 0);
        QVERIFY(r.CountContainerItems() == (i + 1));

        // Check both front/back functions for correctness
        QVERIFY(r.At(0) == i);
        QVERIFY(r.At(i) == 0);
        QVERIFY(const_cast<const RandomAccessContainer<int> &>(r).At(0) == i);
        QVERIFY(const_cast<const RandomAccessContainer<int> &>(r).At(i) == 0);
    }

    for(int i(0); i < ITEM_COUNT; ++i)
    {
        // Check both front/back functions for correctness
        QVERIFY(r.At(0) == ITEM_COUNT - 1);
        QVERIFY(r.At(ITEM_COUNT - i - 1) == i);
        QVERIFY(const_cast<const RandomAccessContainer<int> &>(r).At(0) == ITEM_COUNT - 1);
        QVERIFY(const_cast<const RandomAccessContainer<int> &>(r).At(ITEM_COUNT - i - 1) == i);

        QVERIFY(r.CountContainerItems() == (ITEM_COUNT - i));
        r.RemoveAt(ITEM_COUNT - i - 1);
    }


    // Test the clear function
    const int cnt( 10 );
    for(int i(0); i < cnt; i++)
    {
        r.InsertAt(i, i);
    }
    QVERIFY(r.CountContainerItems() == cnt);

    r.FlushContainer();
    QVERIFY(r.CountContainerItems() == 0);
}

QTEST_APPLESS_MAIN(RandomAccessContainerTest);

#include "tst_randomaccesscontainertest.moc"
