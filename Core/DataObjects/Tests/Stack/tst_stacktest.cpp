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
#include "gutil_interfaces.h"
#include "gutil_vector.h"
#include "gutil_list.h"
#include "gutil_dlist.h"
#include "gutil_slist.h"
USING_NAMESPACE_GUTIL1(DataObjects);

class StackTest : public QObject
{
    Q_OBJECT

public:
    StackTest();

private Q_SLOTS:

    void test_vectorStack();
    void test_listStack();
    void test_slistStack();
    void test_dlistStack();

private:

    void _test_stack(Stack<int> &);

};

StackTest::StackTest()
{
}

void StackTest::test_vectorStack()
{
    VectorStack<int> s;
    _test_stack(s);
}

void StackTest::test_listStack()
{
    ListStack<int> s;
    _test_stack(s);
}

void StackTest::test_slistStack()
{
    SListStack<int> s;
    _test_stack(s);
}

void StackTest::test_dlistStack()
{
    DListStack<int> s;
    _test_stack(s);
}

#define ITEM_COUNT 100000

void StackTest::_test_stack(Stack<int> &stk)
{
    for(int i(0); i < ITEM_COUNT; ++i)
    {
        stk.Push(i);
        QVERIFY(stk.CountStackItems() == (i + 1));

        // Check both top functions for correctness
        QVERIFY(stk.Top() == i);
        QVERIFY(const_cast<const Stack<int> &>(stk).Top() == i);
    }

    for(int i(0); i < ITEM_COUNT; ++i)
    {
        // Check both top functions for correctness
        QVERIFY(stk.Top() == (ITEM_COUNT - i - 1));
        QVERIFY(const_cast<const Stack<int> &>(stk).Top() == (ITEM_COUNT - i - 1));

        QVERIFY(stk.CountStackItems() == (ITEM_COUNT - i));
        stk.Pop();
    }

    const int cnt( 10 );
    for(int i(0); i < cnt; i++)
    {
        stk.Push(i);
    }
    QVERIFY(stk.CountStackItems() == cnt);

    stk.FlushStack();
    QVERIFY(stk.CountStackItems() == 0);
}

QTEST_APPLESS_MAIN(StackTest);

#include "tst_stacktest.moc"
