/*Copyright 2010 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "Core/Interfaces/icomparer.h"
#include "Core/Interfaces/iequalitycomparer.h"
#include <QtCore/QString>
#include <QtTest/QtTest>
using namespace GUtil::Core::Interfaces;

class ComparableTest : public QObject
{
    Q_OBJECT

public:
    ComparableTest();

private Q_SLOTS:
    void test_default_comparer();
    void test_pointer_comparer();
};

ComparableTest::ComparableTest()
{
}

void ComparableTest::test_default_comparer()
{

}

void ComparableTest::test_pointer_comparer()
{
    int a = 0;
    int b = 1;

    DefaultComparer<int> pc;
    QVERIFY(pc.Compare(&a, &b) < 0);
    QVERIFY(pc.Compare(a, b) < 0);
    QVERIFY(pc.Compare(&b, &a) > 0);
    QVERIFY(pc.Compare(b, a) > 0);

    IComparer<int> *ic = &pc;
    QVERIFY(ic->Compare(&b, &a) > 0);
    QVERIFY(ic->Compare(b, a) > 0);
    QVERIFY(ic->Compare(&a, &b) < 0);
    QVERIFY(ic->Compare(a, b) < 0);


    DefaultEqualityComparer<int> pec;
    QVERIFY(!pec.Equal(&a, &b));
    QVERIFY(!pec.Equal(a, b));

    IEqualityComparer<int> *iec = &pec;
    iec = &pec;
    QVERIFY(!iec->Equal(&a, &b));
    QVERIFY(!iec->Equal(a, b));


    b = 0;
    QVERIFY(pec.Equal(&a, &b));
    QVERIFY(pec.Equal(a, b));
    QVERIFY(iec->Equal(&a, &b));
    QVERIFY(iec->Equal(a, b));
}

QTEST_APPLESS_MAIN(ComparableTest);

#include "tst_comparabletest.moc"

