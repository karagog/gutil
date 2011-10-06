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
#include "Core/DataObjects/list.h"
GUTIL_USING_CORE_NAMESPACE(DataObjects);


class ListTest : public QObject
{
    Q_OBJECT

public:
    ListTest();

private Q_SLOTS:
    void test_basic_function();

    void test_non_primitive_type();

    void test_lots();
};

ListTest::ListTest()
{
}

void ListTest::test_basic_function()
{
    List<int> lst;
    lst.Append(0);
    lst.Append(1);
    lst.Append(2);
    lst.Append(3);
    lst.Append(4);
    lst.Append(5);
    lst.Append(6);
    lst.Append(7);
    lst.Append(8);
    lst.Append(9);
    QVERIFY(lst.Size() == 10);

    List<int>::const_iterator iter(lst.begin());
    List<int>::const_iterator riter(lst.rbegin());
    for(int i(0); i < 10; ++i, ++iter, --riter)
    {
        QVERIFY2(i == lst[i], QString("%1 != %2").arg(i).arg(lst[i]).toAscii());
        QVERIFY(i == *iter);
        QVERIFY(9 - i == *riter);
    }

    lst.Prepend(-1);
    QVERIFY(lst.Size() == 11);
    for(int i(-1); i < 10; ++i)
        QVERIFY2(i == lst[i + 1], QString("%1 != %2").arg(i).arg(lst[i + 1]).toAscii());

    lst.Remove(0);
    QVERIFY(lst.Size() == 10);
    for(int i(0); i < 10; ++i)
        QVERIFY2(i == lst[i], QString("%1 != %2").arg(i).arg(lst[i]).toAscii());
}


class non_primitive_type
{
public:
    non_primitive_type(int one = 0, int two = 0)
        :Value1(one), Value2(two)
    {}
    int Value1;
    int Value2;
};

void ListTest::test_non_primitive_type()
{
    List<non_primitive_type> lst;
    lst.Append(non_primitive_type(0, 0));
    lst.Append(non_primitive_type(0, 1));
    QVERIFY(lst.Size() == 2);
    QVERIFY(lst[0].Value1 == 0);
    QVERIFY(lst[0].Value2 == 0);
    QVERIFY(lst[1].Value1 == 0);
    QVERIFY(lst[1].Value2 == 1);

    lst.Prepend(non_primitive_type(1, 1));
    QVERIFY(lst[0].Value1 == 1);
    QVERIFY(lst[0].Value2 == 1);
    QVERIFY(lst[1].Value1 == 0);
    QVERIFY(lst[1].Value2 == 0);
    QVERIFY(lst[2].Value1 == 0);
    QVERIFY(lst[2].Value2 == 1);

    lst.Remove(0);
    QVERIFY(lst[0].Value1 == 0);
    QVERIFY(lst[0].Value2 == 0);
    QVERIFY(lst[1].Value1 == 0);
    QVERIFY(lst[1].Value2 == 1);
}


#define NUMBER 20
void ListTest::test_lots()
{
    List<int> lst;
    for(int i(0); i < NUMBER; ++i)
    {
        lst.Push(i);
        QVERIFY(lst.Size() == i + 1);
    }

    for(int i(0); i < NUMBER; ++i)
        QVERIFY2(lst[i] == i, QString("%1 != %2").arg(lst[i]).arg(i).toAscii());

    for(int i(0); i < NUMBER; ++i)
        lst.Pop();
}


QTEST_APPLESS_MAIN(ListTest);

#include "tst_listtest.moc"
