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
#include "Core/Utils/atomic.h"
#include "gutil_strings.h"
USING_NAMESPACE_GUTIL1(Utils);
USING_NAMESPACE_GUTIL1(DataObjects);

class AtomicTest : public QObject
{
    Q_OBJECT

public:
    AtomicTest();

private Q_SLOTS:
    void testCase1();
};

AtomicTest::AtomicTest()
{
}

void AtomicTest::testCase1()
{
    AtomicInt i;

    int ret = i.FetchAndAdd(1);
    QVERIFY2(ret == 0, String::FromInt(ret));
    QVERIFY2(i == 1, String::FromInt(i.Value()));

    ret = i.FetchAndAdd(4);
    QVERIFY2(i == 5, String::FromInt(i.Value()));
    QVERIFY(ret == 1);

    i = 0;
    ret = i.AddAndFetch(1);
    QVERIFY2(i == 1, String::FromInt(i.Value()));
    QVERIFY(ret == 1);


    // Test inc/dec
    i = 0;
    bool b = i.Increment();
    QVERIFY(b);

    b = i.Decrement();
    QVERIFY(!b);
}

QTEST_APPLESS_MAIN(AtomicTest);

#include "tst_atomictest.moc"
