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
#include "Core/DataObjects/flags.h"
USING_NAMESPACE_GUTIL1(DataObjects);

class FlagsTest : public QObject
{
    Q_OBJECT

public:
    FlagsTest();

private Q_SLOTS:
    void testCase1();
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

void FlagsTest::testCase1()
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

QTEST_APPLESS_MAIN(FlagsTest);

#include "tst_flagstest.moc"
