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
#include "Core/DataObjects/stack.h"
GUTIL_USING_CORE_NAMESPACE(DataObjects);

class StackTest : public QObject
{
    Q_OBJECT

public:
    StackTest();

private Q_SLOTS:
    void test_basic_function();

    void test_iterators();

    void test_removal();

};

StackTest::StackTest()
{
}

void StackTest::test_basic_function()
{
    Stack<int> tmpstack;
    for(int i = 0; i < 10; i++)
        tmpstack.Push(i);

    for(int i = 9; i >= 0; i--)
    {
        QVERIFY(tmpstack.Top() == i);
        QVERIFY(tmpstack.Count() == i + 1);
        tmpstack.Pop();
    }
}

void StackTest::test_iterators()
{
    Stack<int> stack;
    for(int i = 0; i < 10; i++)
        stack.Push(i);

    int tmp(9);
    for(Stack<int>::iterator iter(stack.begin()); iter != stack.end(); iter++)
    {
        QVERIFY(*iter == tmp);
        tmp--;
    }
}

void StackTest::test_removal()
{
    Stack<int> stack;
    for(int i = 0; i < 10; i++)
        stack.Push(i);

    QVERIFY(stack.Count() == 10);

    // Test from the top of the stack
    Stack<int>::iterator iter(stack.begin());
    stack.Remove(iter);

    QVERIFY(stack.Count() == 9);
    QVERIFY(*iter == 8);


    // Test from the middle of the stack
    iter++; iter++; iter++; iter++; iter++;
    stack.Remove(iter);
    QVERIFY(stack.Count() == 8);
}

QTEST_APPLESS_MAIN(StackTest);

#include "tst_stacktest.moc"
