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
#include "Core/DataObjects/slist.h"
GUTIL_USING_CORE_NAMESPACE(DataObjects);

class SListTest : public QObject
{
    Q_OBJECT

public:
    SListTest();

private Q_SLOTS:
    void test_stack();

    void test_iterators();

    void test_removal();

    void test_queue();

};

SListTest::SListTest()
{
}

void SListTest::test_stack()
{
    SList<int> lst;
    SListStack<int> stk(&lst);

    for(int i = 0; i < 10; i++)
        stk.Push(i);

    for(int i = 9; i >= 0; i--)
    {
        QVERIFY(stk.Top() == i);
        QVERIFY(stk.CountStackItems() == i + 1);
        stk.Pop();
    }

    stk.Push(1);
    stk.Push(2);
    QVERIFY(stk.CountStackItems() == 2);

    stk.FlushStack();
    QVERIFY(stk.CountStackItems() == 0);
}

void SListTest::test_iterators()
{
    SList<int> lst;

    for(int i = 0; i < 10; i++)
        lst.PushFront(i);

    int tmp(9);
    for(SList<int>::iterator iter(lst.begin()); iter; ++iter)
    {
        QVERIFY(*iter == tmp);
        tmp--;
    }
}

void SListTest::test_removal()
{
    SList<int> lst;
    for(int i = 0; i < 10; i++)
        lst.PushFront(i);

    QVERIFY(lst.Count() == 10);

    // Test from the top of the stack
    SList<int>::iterator iter(lst.begin());
    lst.Remove(iter);

    QVERIFY(lst.Count() == 9);
    QVERIFY2(*iter == 8, QString("%1").arg(*iter).toAscii());


    // Test removal from the middle of the stack
    iter++; iter++; iter++; iter++; iter++;
    lst.Remove(iter);
    QVERIFY(lst.Count() == 8);

    // Test removal from the end of the stack
    SList<int>::iterator tmp;
    while(++iter != lst.end())
        tmp = iter;
    lst.Remove(tmp);
    QVERIFY(lst.Count() == 7);
}

void SListTest::test_queue()
{
    SList<int> lst;
    SListQueue<int> q( &lst );

    q.Enqueue(0);
    q.Enqueue(1);
    q.Enqueue(2);
    q.Enqueue(3);
    q.Enqueue(4);
    q.Enqueue(5);
    QVERIFY(q.CountQueueItems() == 6);
    for(int i(0); i < 6; ++i)
    {
        QVERIFY2(q.Front() == i, QString("%1 != %2").arg(q.Front()).arg(i).toAscii());
        QVERIFY(q.CountQueueItems() == 6 - i);
        q.Dequeue();
    }

    q.Enqueue(1);
    q.Enqueue(2);
    QVERIFY(q.CountQueueItems() == 2);

    q.FlushQueue();
    QVERIFY(q.CountQueueItems() == 0);
}

QTEST_APPLESS_MAIN(SListTest);

#include "tst_slisttest.moc"
