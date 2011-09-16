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
    void test_basic_function();

    void test_iterators();

    void test_removal();

};

SListTest::SListTest()
{
}

void SListTest::test_basic_function()
{
    SList<int> tmpstack;
    for(int i = 0; i < 10; i++)
        tmpstack.Push(i);

    for(int i = 9; i >= 0; i--)
    {
        QVERIFY(tmpstack.Top() == i);
        QVERIFY(tmpstack.Count() == i + 1);
        tmpstack.Pop();
    }
}

void SListTest::test_iterators()
{
    SList<int> stack;
    for(int i = 0; i < 10; i++)
        stack.Push(i);

    int tmp(9);
    for(SList<int>::iterator iter(stack.begin()); iter != stack.end(); iter++)
    {
        QVERIFY(*iter == tmp);
        tmp--;
    }
}

void SListTest::test_removal()
{
    SList<int> stack;
    for(int i = 0; i < 10; i++)
        stack.Push(i);

    QVERIFY(stack.Count() == 10);

    // Test from the top of the stack
    SList<int>::iterator iter(stack.begin());
    stack.Remove(iter);

    QVERIFY(stack.Count() == 9);
    QVERIFY2(*iter == 8, QString("%1").arg(*iter).toAscii());


    // Test removal from the middle of the stack
    iter++; iter++; iter++; iter++; iter++;
    stack.Remove(iter);
    QVERIFY(stack.Count() == 8);

    // Test removal from the end of the stack
    SList<int>::iterator tmp;
    while(++iter != stack.end())
        tmp = iter;
    stack.Remove(tmp);
    QVERIFY(stack.Count() == 7);
}

QTEST_APPLESS_MAIN(SListTest);

#include "tst_stacktest.moc"
