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
#include "Core/DataObjects/dlist.h"
GUTIL_USING_CORE_NAMESPACE(DataObjects);

class DListTest : public QObject
{
    Q_OBJECT

public:
    DListTest();

private Q_SLOTS:
    void testCase1();
};

DListTest::DListTest()
{
}

void DListTest::testCase1()
{
    DList<int> list;
    DList<int>::iterator iter;

    list.PushBack(0);
    list.PushBack(1);
    list.PushBack(2);
    list.PushBack(3);
    list.PushBack(4);
    QVERIFY(list.Count() == 5);
    iter = list.begin();
    for(int i(0); i < 5; ++i, ++iter)
    {
        int cur(*iter);
        QVERIFY2(cur == i, QString("%1 != %2").arg(cur).arg(i).toAscii());
    }

    list.Clear();
    QVERIFY(list.Count() == 0);

    // Test the stack interface
    DListStack<int> stk( &list );
    stk.Push(4);
    stk.Push(3);
    stk.Push(2);
    stk.Push(1);
    stk.Push(0);
    for(int i(0); i < 5; ++i, ++iter)
    {
        int cur(stk.Top());
        QVERIFY2(cur == i, QString("%1 != %2").arg(cur).arg(i).toAscii());
        stk.Pop();
    }
    QVERIFY(stk.CountStackItems() == 0);

    // Test the queue interface.
    DListQueue<int> q(&list);
    q.Enqueue(0);
    q.Enqueue(1);
    q.Enqueue(2);
    q.Enqueue(3);
    q.Enqueue(4);
    iter = list.begin();
    for(int i(0); i < 5; ++i)
    {
        int cur(q.Front());
        QVERIFY2(cur == i, QString("%1 != %2").arg(cur).arg(i).toAscii());
        q.Dequeue();
    }
    QVERIFY(q.CountQueueItems() == 0);


    // Try iterating backwards
    q.Enqueue(0);
    q.Enqueue(1);
    q.Enqueue(2);
    q.Enqueue(3);
    q.Enqueue(4);
    iter = list.rbegin();
    for(int i(0); i < 5; ++i, --iter)
    {
        int cur(*iter);
        QVERIFY2(cur == 4 - i, QString("%1 != %2").arg(cur).arg(i).toAscii());
    }
}

QTEST_APPLESS_MAIN(DListTest);

#include "tst_dlisttest.moc"
