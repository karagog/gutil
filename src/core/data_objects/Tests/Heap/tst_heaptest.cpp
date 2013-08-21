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
#include "Core/DataObjects/heap.h"
#include <iostream>
USING_NAMESPACE_GUTIL1(DataObjects);
using namespace std;

class HeapTest : public QObject
{
    Q_OBJECT

public:
    HeapTest();

private Q_SLOTS:
    void test_minheap();
    void test_maxheap();

    void test_random();

private:

    void show_heap(const Heap<int> &);
};

HeapTest::HeapTest()
{
    qsrand(QDateTime::currentMSecsSinceEpoch());
}

#define ITEM_COUNT 10000

void HeapTest::test_minheap()
{
    MinHeap<int> h;
    for(int i(0); i < ITEM_COUNT; ++i)
    {
        h.Push(i);
        QVERIFY(h.Count() == i + 1);
        QVERIFY(h.Top() == 0);
        QVERIFY(const_cast<const MinHeap<int> &>(h).Top() == 0);
    }

    for(int i(0); i < ITEM_COUNT; ++i)
    {
        h.Pop();
        QVERIFY(h.Count() == ITEM_COUNT - i - 1);
        if(i < ITEM_COUNT - 1)
        {
            QVERIFY2(h.Top() == i + 1, QString("%1 != %2").arg(h.Top()).arg(i + 1).toAscii());
            QVERIFY(const_cast<const MinHeap<int> &>(h).Top() == i + 1);
        }
    }
}

void HeapTest::test_maxheap()
{
    MaxHeap<int> h;
    for(int i(0); i < ITEM_COUNT; ++i)
    {
        h.Push(ITEM_COUNT - i);
        QVERIFY(h.Count() == i + 1);
        QVERIFY(h.Top() == ITEM_COUNT);
        QVERIFY(const_cast<const MaxHeap<int> &>(h).Top() == ITEM_COUNT);
    }

    for(int i(0); i < ITEM_COUNT; ++i)
    {
        h.Pop();
        QVERIFY(h.Count() == ITEM_COUNT - i - 1);
        if(i < ITEM_COUNT - 1)
        {
            QVERIFY2(h.Top() == ITEM_COUNT - i - 1, QString("%1 != %2").arg(h.Top()).arg(ITEM_COUNT - i - 1).toAscii());
            QVERIFY(const_cast<const MaxHeap<int> &>(h).Top() == ITEM_COUNT - i - 1);
        }
    }
}

#define RANDOM_ITEM_COUNT 100000

void HeapTest::test_random()
{
    MinHeap<int> h;
    //qDebug("Inserting items:");
    for(int i(0); i < RANDOM_ITEM_COUNT; ++i)
    {
        const int new_item( qrand() % RANDOM_ITEM_COUNT );
        //qDebug() << new_item;
        h.Push(new_item);
        QVERIFY(h.Count() == i + 1);
    }

    int last(INT_MIN);
    //qDebug("Removing items:");
    for(int i(0); i < RANDOM_ITEM_COUNT; ++i)
    {
        QVERIFY(last <= h.Top());
        QVERIFY(last <= const_cast<const MinHeap<int> &>(h).Top());

        const int top( h.Top() );

        last = top;
        h.Pop();

        QVERIFY(h.Count() == RANDOM_ITEM_COUNT - i - 1);
    }
}

void HeapTest::show_heap(const Heap<int> &h)
{
    const int *begin( h.ConstData() );
    for(int i(0); i < h.Count(); i++)
    {
        cout<<begin[i]<<",";
    }
    cout<<endl;
}

QTEST_APPLESS_MAIN(HeapTest);

#include "tst_heaptest.moc"
