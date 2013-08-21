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
#include "gutil_flags.h"
USING_NAMESPACE_GUTIL1(DataObjects);

class FlagsTest : public QObject
{
    Q_OBJECT

public:
    FlagsTest();

private Q_SLOTS:
    void test_basics();

    void test_smaller_flags();
};

FlagsTest::FlagsTest()
{
}


enum test_enum
{
    One,
    Two,
    Three,
    Four,
    Five,
    Six
};

GUTIL_DECLARE_FLAGS(TestFlags, test_enum);
GUTIL_DECLARE_FLAG_OPERATORS(TestFlags, test_enum);

void FlagsTest::test_basics()
{
    TestFlags f;
    qDebug(QString("The size of Flags object: %1").arg(sizeof(TestFlags)).toAscii());

    QVERIFY(!f.TestFlag(One));

    f |= One;
    QVERIFY(f.TestFlag(One));

    f.ToggleFlag(One);
    QVERIFY(!f.TestFlag(One));

    f = One | Two | Four | Six;
    QVERIFY(f.TestFlag(One));
    QVERIFY(f.TestFlag(Two));
    QVERIFY(!f.TestFlag(Three));
    QVERIFY(f.TestFlag(Four));
    QVERIFY(!f.TestFlag(Five));
    QVERIFY(f.TestFlag(Six));

    QVERIFY(!f.TestFlag((test_enum)10));
    f |= (test_enum)10;
    QVERIFY(f.TestFlag((test_enum)10));
}


enum test_enum2
{
    one,
    two,
    three,
    four,
    five,
    six
};

GUTIL_DECLARE_FLAGS2(TestFlags16Bit, test_enum2, GINT16);
GUTIL_DECLARE_FLAG_OPERATORS(TestFlags16Bit, test_enum2);

void FlagsTest::test_smaller_flags()
{
    TestFlags16Bit f;
    qDebug(QString("The size of 16-bit Flags object: %1").arg(sizeof(TestFlags16Bit)).toAscii());

    QVERIFY(!f.TestFlag(one));

    f |= one;
    QVERIFY(f.TestFlag(one));

    f.ToggleFlag(one);
    QVERIFY(!f.TestFlag(one));

    f = one | two | four | six;
    QVERIFY(f.TestFlag(one));
    QVERIFY(f.TestFlag(two));
    QVERIFY(!f.TestFlag(three));
    QVERIFY(f.TestFlag(four));
    QVERIFY(!f.TestFlag(five));
    QVERIFY(f.TestFlag(six));

    QVERIFY(!f.TestFlag((test_enum2)10));
    f |= (test_enum2)10;
    QVERIFY(f.TestFlag((test_enum2)10));
}

QTEST_APPLESS_MAIN(FlagsTest);

#include "tst_flagstest.moc"
