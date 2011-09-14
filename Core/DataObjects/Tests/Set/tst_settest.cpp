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
    }
}

QTEST_APPLESS_MAIN(SetTest);

#include "tst_settest.moc"
