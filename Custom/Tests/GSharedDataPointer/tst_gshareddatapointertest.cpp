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

#include "Custom/gshareddatapointer.h"
#include <QtCore/QString>
#include <QtTest/QtTest>
using namespace GUtil::Custom;

class GSharedDataPointerTest : public QObject
{
    Q_OBJECT

public:
    GSharedDataPointerTest();

private Q_SLOTS:
    void test_Idiot();
    void test_Equality();
};

class SharedData : public QSharedData
{
public:
    SharedData(){}
    SharedData(const SharedData &){}

    char value;
};

GSharedDataPointerTest::GSharedDataPointerTest()
{
}

void GSharedDataPointerTest::test_Idiot()
{
    GSharedDataPointer<SharedData> p1 = new SharedData;
    GSharedDataPointer<SharedData> p2(p1);

    p1.Lock();
    QVERIFY(!p1.TryLock());
    QVERIFY(!p2.TryLock());


    p2.detach();

    // Now that we're detached we should have our own lock
    QVERIFY(p2.TryLock());
    p2.Unlock();

    // You can detach even while holding the lock, but chances are it's a bug in
    //  your code
    p1 = p2;
    QVERIFY(p1.TryLock());
}

void GSharedDataPointerTest::test_Equality()
{
    GSharedDataPointer<SharedData> p1 = new SharedData;
    p1->value = '1';

    GSharedDataPointer<SharedData> p2 = p1;

    QVERIFY(p2->value = '1');

    p1->value = '2';
    QVERIFY(p2->value = '2');

    p1.Lock();
    QVERIFY(!p2.TryLock());
}

QTEST_APPLESS_MAIN(GSharedDataPointerTest);

#include "tst_gshareddatapointertest.moc"
