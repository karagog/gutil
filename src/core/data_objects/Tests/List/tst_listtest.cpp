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
#include <gutil/list.h>
USING_NAMESPACE_GUTIL;


class ListTest : public QObject
{
    Q_OBJECT

public:
    ListTest();

private Q_SLOTS:
    void test_basic_function();

    void test_non_primitive_type();

    void test_lots();

    void test_sorting();
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
    QVERIFY(lst.Length() == 10);

    List<int>::const_iterator iter(lst.begin());
    List<int>::const_iterator riter(lst.rbegin());
    for(int i(0); i < 10; ++i, ++iter, --riter)
    {
        QVERIFY2(i == lst[i], QString("%1 != %2").arg(i).arg(lst[i]).toLatin1());
        QVERIFY(i == *iter);
        QVERIFY2(9 - i == *riter, QString("%1 != %2").arg(9 - i).arg(*riter).toLatin1());
    }

    lst.Prepend(-1);
    QVERIFY(lst.Length() == 11);
    for(int i(-1); i < 10; ++i)
        QVERIFY2(i == lst[i + 1], QString("%1 != %2").arg(i).arg(lst[i + 1]).toLatin1());

    lst.RemoveAt(0);
    QVERIFY(lst.Length() == 10);
    for(int i(0); i < 10; ++i)
        QVERIFY2(i == lst[i], QString("%1 != %2").arg(i).arg(lst[i]).toLatin1());
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
    QVERIFY(lst.Length() == 2);
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

    lst.RemoveAt(0);
    QVERIFY(lst[0].Value1 == 0);
    QVERIFY(lst[0].Value2 == 0);
    QVERIFY(lst[1].Value1 == 0);
    QVERIFY(lst[1].Value2 == 1);
}


#define NUMBER 10000
void ListTest::test_lots()
{
    List<int> lst;
    for(int i(0); i < NUMBER; ++i)
    {
        lst.Append(i);
        QVERIFY(lst.Length() == i + 1);
    }

    for(int i(0); i < NUMBER; ++i)
        QVERIFY2(lst[i] == i, QString("%1 != %2").arg(lst[i]).arg(i).toLatin1());

    for(int i(0); i < NUMBER; ++i)
        lst.RemoveAt(lst.Length() - 1);
}

void ListTest::test_sorting()
{
    List<int> l;
    l.Append(10);
    l.Append(9);
    l.Append(8);
    l.Append(7);
    l.Append(6);
    l.Append(5);
    l.Append(4);
    l.Append(3);
    l.Append(2);
    l.Append(1);
    l.Append(0);
    l.Sort(true, GUtil::MergeSort);

    //    for(int i = 0; i < 11; ++i)
    //        qDebug() << v[i];

    for(int i = 0; i < 11; ++i)
        QVERIFY2(l[i] == i, QString("%1 != %2").arg(l[i]).arg(i).toUtf8());


    l.Sort(false);
    for(int i = 10; i >= 0; --i)
        QVERIFY2(l[10 - i] == i, QString("%1 != %2").arg(l[10 - i]).arg(i).toUtf8());



    l.Clear();

    const int cnt(10000);
    for(int i = 0; i < cnt; ++i)
        l.Append(qrand());

    l.Sort();
    //    for(int i = 0; i < cnt; ++i)
    //        qDebug() << v[i];

    QVERIFY(l.Length() == cnt);
    int mem(INT_MIN);
    for(int i = 0; i < cnt; ++i)
    {
        QVERIFY2(mem <= l[i], QString("%1 > %2").arg(mem).arg(l[i]).toUtf8());
        mem = l[i];
    }


    // Try sorting 1 item
    l = {1};
    l.Sort();
    QVERIFY(l.Length() == 1);
    QVERIFY(l[0] == 1);
}


QTEST_APPLESS_MAIN(ListTest);

#include "tst_listtest.moc"
