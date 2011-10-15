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
#include "Core/DataObjects/dlist.h"
#include "Core/DataObjects/slist.h"
GUTIL_USING_CORE_NAMESPACE(DataObjects);

class QueueTest : public QObject
{
    Q_OBJECT

public:
    QueueTest();

private Q_SLOTS:

    void test_vectorQueue();
    void test_listQueue();
    void test_slistQueue();
    void test_dlistQueue();

private:
    void _test_queue(Queue<int> &q);
};

QueueTest::QueueTest()
{
}

void QueueTest::test_vectorQueue()
{
    VectorQueue<int> q;
    _test_queue(q);
}

void QueueTest::test_listQueue()
{
    ListQueue<int> q;
    _test_queue(q);
}

void QueueTest::test_slistQueue()
{
    SListQueue<int> q;
    _test_queue(q);
}

void QueueTest::test_dlistQueue()
{
    DListQueue<int> q;
    _test_queue(q);
}

#define ITEM_COUNT 10000

void QueueTest::_test_queue(Queue<int> &q)
{
    for(int i(0); i < ITEM_COUNT; ++i)
    {
        q.Enqueue(i);
        QVERIFY(q.CountQueueItems() == (i + 1));

        // Check both front functions for correctness
        QVERIFY(q.Front() == 0);
        QVERIFY(const_cast<const Queue<int> &>(q).Front() == 0);
    }

    for(int i(0); i < ITEM_COUNT; ++i)
    {
        // Check both front functions for correctness
        QVERIFY(q.Front() == i);
        QVERIFY(const_cast<const Queue<int> &>(q).Front() == i);

        QVERIFY(q.CountQueueItems() == (ITEM_COUNT - i));
        q.Dequeue();
    }

    const int cnt( 10 );
    for(int i(0); i < cnt; i++)
    {
        q.Enqueue(i);
    }
    QVERIFY(q.CountQueueItems() == cnt);

    q.FlushQueue();
    QVERIFY(q.CountQueueItems() == 0);
}

QTEST_APPLESS_MAIN(QueueTest);

#include "tst_queuetest.moc"
