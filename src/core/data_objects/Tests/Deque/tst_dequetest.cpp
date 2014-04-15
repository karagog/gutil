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
#include "gutil_icollections.h"
#include "gutil_vector.h"
#include "gutil_list.h"
#include "gutil_dlist.h"
USING_NAMESPACE_GUTIL;

class DequeTest : public QObject
{
    Q_OBJECT

public:
    DequeTest();

private Q_SLOTS:
    void test_vectorQueue();
    void test_listQueue();
    void test_dlistQueue();

private:

    static void _test_deque(Deque<int> &);
};

DequeTest::DequeTest()
{
}

void DequeTest::test_vectorQueue()
{
    VectorDeque<int> q;
    _test_deque(q);
}

void DequeTest::test_listQueue()
{
    ListDeque<int> q;
    _test_deque(q);
}

void DequeTest::test_dlistQueue()
{
    DListDeque<int> q;
    _test_deque(q);
}

#define ITEM_COUNT 10000


void DequeTest::_test_deque(Deque<int> &q)
{
    for(int i(0); i < ITEM_COUNT; ++i)
    {
        q.PushBack(i);
        QVERIFY(q.CountDequeItems() == (i + 1));

        // Check both front/back functions for correctness
        QVERIFY(q.Front() == 0);
        QVERIFY(q.Back() == i);
        QVERIFY(const_cast<const Deque<int> &>(q).Front() == 0);
        QVERIFY(const_cast<const Deque<int> &>(q).Back() == i);
    }

    for(int i(0); i < ITEM_COUNT; ++i)
    {
        // Check both front/back functions for correctness
        QVERIFY(q.Front() == i);
        QVERIFY(q.Back() == ITEM_COUNT - 1);
        QVERIFY(const_cast<const Deque<int> &>(q).Front() == i);
        QVERIFY(const_cast<const Deque<int> &>(q).Back() == ITEM_COUNT - 1);

        QVERIFY(q.CountDequeItems() == (ITEM_COUNT - i));
        q.PopFront();
    }


    for(int i(0); i < ITEM_COUNT; ++i)
    {
        q.PushFront(i);
        QVERIFY(q.CountDequeItems() == (i + 1));

        // Check both front/back functions for correctness
        QVERIFY(q.Front() == i);
        QVERIFY(q.Back() == 0);
        QVERIFY(const_cast<const Deque<int> &>(q).Front() == i);
        QVERIFY(const_cast<const Deque<int> &>(q).Back() == 0);
    }

    for(int i(0); i < ITEM_COUNT; ++i)
    {
        // Check both front/back functions for correctness
        QVERIFY(q.Front() == ITEM_COUNT - 1);
        QVERIFY(q.Back() == i);
        QVERIFY(const_cast<const Deque<int> &>(q).Front() == ITEM_COUNT - 1);
        QVERIFY(const_cast<const Deque<int> &>(q).Back() == i);

        QVERIFY(q.CountDequeItems() == (ITEM_COUNT - i));
        q.PopBack();
    }


    // Test the clear function
    const GUINT32 cnt( 10 );
    for(GUINT32 i(0); i < cnt; i++)
    {
        q.PushBack(i);
    }
    QVERIFY(q.CountDequeItems() == cnt);

    q.FlushDeque();
    QVERIFY(q.CountDequeItems() == 0);
}

QTEST_APPLESS_MAIN(DequeTest);

#include "tst_dequetest.moc"
