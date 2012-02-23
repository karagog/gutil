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
#include "Core/DataObjects/dlist.h"
#include "Core/DataObjects/gstring.h"
USING_NAMESPACE_GUTIL1(DataObjects);

class DListTest : public QObject
{
    Q_OBJECT

public:
    DListTest();

private Q_SLOTS:

    void test_basics();

    void test_iterators();

    void test_insert();
    void test_remove();

    void test_sorting();


private:

    static void validate_list(const DList<int> &);

};

DListTest::DListTest()
{
}

void DListTest::test_basics()
{
    DList<int> list;

    list.PushBack(0);
    list.PushBack(1);
    list.PushBack(2);
    list.PushBack(3);
    list.PushBack(4);
    QVERIFY2(list.Count() == 5, String::FromInt(list.Count()));
    DList<int>::iterator iter( list.begin() );
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

    validate_list(list);
}

void DListTest::test_iterators()
{
    DList<int> d;

    // Test insertions do not move the iterator (stays in the same position)
    d.PushBack(0);
    d.PushBack(1);
    d.PushBack(4);
    d.PushBack(5);

    DList<int>::iterator iter( d.begin() + 2 );
    d.Insert(3, iter);
    d.Insert(2, iter);
    iter = d.begin();
    for(int i(0); i < 5; ++i, ++iter){
        int cur(*iter);
        QVERIFY2(cur == i, QString("%1 != %2").arg(cur).arg(i).toAscii());
    }

    validate_list(d);


    // You should be able to decrement the "end" iterator to get back on the list
    iter = d.end();
    --iter;
    QVERIFY(*iter == 5);

    // Vice-versa with the "rend" iterator
    iter = d.rend();
    ++iter;
    QVERIFY(*iter == 0);
}

void DListTest::test_insert()
{
    DList<int> d;
    d.PushBack(1);
    d.PushBack(2);
    d.PushBack(3);
    d.PushBack(5);

    // Insert somewhere in the middle
    DList<int>::const_iterator iter( d.begin() + 3 );
    d.Insert(4, iter);

    // Verify that the iterator is still valid and points to the correct item
    QVERIFY(*iter == 4);
    QVERIFY(*(iter + 1) == 5);
    QVERIFY(*(iter - 1) == 3);

    iter = d.begin();
    for(int i(1); i <= 5; ++i, ++iter)
        QVERIFY(*iter == i);

    // Try inserting on the front
    d.Insert(0, (iter = d.begin()));
    for(int i(0); i <= 5; ++i, ++iter)
        QVERIFY(*iter == i);

    // Try inserting on the back
    d.Insert(6, (iter = d.end()));
    QVERIFY(iter == d.end());
    QVERIFY(*(iter - 1) == 6);

    iter = d.begin();
    for(int i(0); i <= 6; ++i, ++iter)
        QVERIFY(*iter == i);
}

void DListTest::test_remove()
{
    DList<int> d;
    d.PushBack(0);
    d.PushBack(1);
    d.PushBack(1);
    d.PushBack(2);
    d.PushBack(3);
    d.PushBack(4);

    DList<int>::const_iterator iter(d.begin() + 2);
    d.Remove(iter);

    // Verify that removing doesn't invalidate the iterator
    QVERIFY(iter);
    QVERIFY(*iter == 2);
    QVERIFY(*(iter + 1) == 3);
    QVERIFY(*(iter - 1) == 1);

    // Verify that the rest of the list is still in order
    iter = d.begin();
    for(int i(0); i < 5; ++i, ++iter)
        QVERIFY(*iter == i);


    // Remove from the front of the list
    d.Remove((iter = d.begin()));
    QVERIFY(iter);
    QVERIFY(*iter == 1);
    QVERIFY(*(iter + 1) == 2);
    QVERIFY((iter - 1) == d.rend());

    iter = d.begin();
    for(int i(1); i < 5; ++i, ++iter)
        QVERIFY(*iter == i);


    // Remove from the end of the list
    d.Remove((iter = d.rbegin()));
    QVERIFY(iter == d.end());
    QVERIFY(*(iter - 1) == 3);

    iter = d.begin();
    for(int i(1); i < 4; ++i, ++iter)
        QVERIFY(*iter == i);


    // Test the function that removes a range of values
    d.Clear();
    d.PushBack(0);
    d.PushBack(1);
    d.PushBack(2);
    d.PushBack(3);
    d.PushBack(4);

    d.Remove(d.begin() + 1, d.begin() + 3);
    QVERIFY(d.Count() == 3);
    QVERIFY(*(d.begin() + 0) == 0);
    QVERIFY(*(d.begin() + 1) == 3);
    QVERIFY(*(d.begin() + 2) == 4);
}

void DListTest::test_sorting()
{
    const int max(10), min(0);

    DList<int> d;
    for(int i(max); i >= min; --i)
        d.PushBack(i);
    d.Sort(true, GUtil::MergeSort);

    int i = min;
    for(DList<int>::iterator iter(d.begin()); iter != d.end(); ++iter, ++i)
    {
        //qDebug() << *iter;
        QVERIFY2(*iter == i, QString("%1 != %2").arg(*iter).arg(i).toUtf8());
    }

    // Iterate backwards through the list to make sure the reverse pointers are correct
    i = max;
    for(DList<int>::iterator iter(d.rbegin()); iter != d.rend(); --iter, --i)
    {
        //qDebug() << *iter;
        QVERIFY2(*iter == i, QString("%1 != %2").arg(*iter).arg(i).toUtf8());
    }

    // Try sorting backwards
    d.Sort(false);
    i = max;
    for(DList<int>::iterator iter(d.begin()); iter != d.end(); ++iter, --i)
    {
        //qDebug() << *iter;
        QVERIFY2(*iter == i, QString("%1 != %2").arg(*iter).arg(i).toUtf8());
    }


    d.Clear();

    const int cnt(10000);
    for(int i = 0; i < cnt; ++i)
        d.PushBack(qrand());

    d.Sort();
//    for(int i = 0; i < cnt; ++i)
//        qDebug() << v[i];

    QVERIFY(d.Length() == cnt);
    int mem = INT_MIN;
    for(DList<int>::iterator iter(d.begin()); iter != d.end(); ++iter)
    {
        QVERIFY2(mem <= *iter, QString("%1 > %2").arg(mem).arg(*iter).toUtf8());
        mem = *iter;
    }
}





void DListTest::validate_list(const DList<int> &dl)
{
    const GUINT32 sz(dl.Size());
    GUINT32 cnt = 0;
    for(DList<int>::const_iterator iter(dl.begin()); iter != dl.end(); ++iter, ++cnt){

    }
    QVERIFY(sz == cnt);

    cnt = 0;
    for(DList<int>::const_iterator iter(dl.rbegin()); iter != dl.rend(); --iter, ++cnt){

    }
    QVERIFY(sz == cnt);
}


QTEST_APPLESS_MAIN(DListTest);

#include "tst_dlisttest.moc"
