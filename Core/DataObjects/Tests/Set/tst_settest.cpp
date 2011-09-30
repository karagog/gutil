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
#include "Core/DataObjects/set.h"
GUTIL_USING_CORE_NAMESPACE(DataObjects);

class SetTest : public QObject
{
    Q_OBJECT

public:
    SetTest();

private Q_SLOTS:
    void test_basic_function();

    void test_set();

    void test_multi_set();


private:

    static int backwards_compare(const int &lhs, const int &rhs){
        if(lhs < rhs)
            return 1;
        if(rhs < lhs)
            return -1;
        return 0;
    }
};

SetTest::SetTest()
{
}

void SetTest::test_basic_function()
{
    {
        Set<int> set;
        set.Insert(0);
        set.Insert(1);
        set.Insert(2);
        set.Insert(3);
        set.Insert(4);
        set.Insert(5);
        QVERIFY(set.Size() == 6);
        QVERIFY2(set.begin(), "The first iterator is not valid?");

        int cnt(0);
        for(Set<int>::const_iterator iter( set.begin() ); iter; ++iter)
        {
            QVERIFY2(*iter == cnt, QString("%1 != %2").arg(*iter).arg(cnt).toAscii());
            cnt++;
        }

        set.Clear();
        QVERIFY2(!set.begin(), "There is a valid iterator on an empty set?");
        QVERIFY(set.Size() == 0);

        // Check that values inserted backwards come out sorted when we iterate


        set.Insert(5);
        set.Insert(4);
        set.Insert(3);
        set.Insert(2);
        set.Insert(1);
        set.Insert(0);
        cnt = 0;
        for(Set<int>::const_iterator iter( set.begin() ); iter; ++iter)
        {
            QVERIFY2(*iter == cnt, QString("%1 != %2").arg(*iter).arg(cnt).toAscii());
            cnt++;
        }
    }

    {
        Set<int> set(&backwards_compare);
        set.Insert(5);
        set.Insert(4);
        set.Insert(3);
        set.Insert(2);
        set.Insert(1);
        set.Insert(0);
        QVERIFY(set.Size() == 6);
        QVERIFY2(set.begin(), "The first iterator is not valid?");

        int cnt(5);
        for(Set<int>::const_iterator iter( set.begin() ); iter; ++iter)
        {
            QVERIFY2(*iter == cnt, QString("%1 != %2").arg(*iter).arg(cnt).toAscii());
            cnt--;
        }

        // Make sure a copy of the set has the same backwards compare behavior
        Set<int> cpy( set );
        cnt = 5;
        for(Set<int>::const_iterator iter( cpy.begin() ); iter; ++iter)
        {
            QVERIFY2(*iter == cnt, QString("%1 != %2").arg(*iter).arg(cnt).toAscii());
            cnt--;
        }
    }
}

void SetTest::test_set()
{
    // Test no duplicates allowed
    Set<int> set;
    set.Insert(2);
    set.Insert(1);
    set.Insert(1);
    set.Insert(1);
    set.Insert(0);
    QVERIFY(set.Size() == 3);
    QVERIFY(set.Count(1) == 1);

    int cnt(0);
    for(Set<int>::const_iterator iter( set.begin() ); iter; ++iter)
    {
        QVERIFY2(*iter == cnt, QString("%1 != %2").arg(*iter).arg(cnt).toAscii());
        cnt++;
    }

    // Test inserting duplicates
    set.InsertMulti(1);
    set.InsertMulti(1);
    set.InsertMulti(1);
    QVERIFY(set.Size() == 6);
    QVERIFY(set.Count(1) == 4);

    // Inserting overwrites the multiple values
    set.Insert(1);
    QVERIFY(set.Size() == 3);
    QVERIFY(set.Count(1) == 1);

    // Test the remove functions
    QVERIFY(set.Contains(1));
    set.RemoveOne(1);
    QVERIFY(!set.Contains(1));
    QVERIFY(set.Count(1) == 0);

    QVERIFY(set.Contains(0));
    set.RemoveAll(0);
    QVERIFY(!set.Contains(0));
    QVERIFY(set.Count(0) == 0);

    // Test the copy constructor, and the syntax of a foreach loop
    // Note: not a good idea to use the foreach, because cloning the set is expensive.
    set.InsertMulti(1);
    set.InsertMulti(2);
    set.InsertMulti(3);
    set.InsertMulti(4);
    set.InsertMulti(5);
    set.InsertMulti(6);
    foreach(int i, set)
    {
        GDEBUG(std::endl << i);
    }
}

void SetTest::test_multi_set()
{
    MultiSet<int> set;

    // Test duplicates allowed
    set.Insert(2);
    set.Insert(1);
    set.Insert(1);
    set.Insert(1);
    set.Insert(0);
    QVERIFY(set.Size() == 5);
    QVERIFY(set.Count(1) == 3);

    // Test the copy constructor
    MultiSet<int> cpy( set );
    QVERIFY2(cpy.Size() == 5, QString("Size = %1").arg(cpy.Size()).toAscii());
    QVERIFY(cpy.Count(1) == 3);


    set.InsertMulti(1);
    QVERIFY(set.Size() == 6);
    QVERIFY(set.Count(1) == 4);

    set.RemoveOne(1);
    QVERIFY(set.Size() == 5);
    QVERIFY(set.Count(1) == 3);

    set.RemoveAll(1);
    QVERIFY(set.Size() == 2);
    QVERIFY(set.Count(1) == 0);

    // Test the assignment operator
    cpy = set;
    QVERIFY(set.Size() == 2);
    QVERIFY(set.Count(1) == 0);

}


QTEST_APPLESS_MAIN(SetTest);

#include "tst_settest.moc"
