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
#include "Core/Utils/atomic.h"
GUTIL_USING_CORE_NAMESPACE(Utils);

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
    int i(0);
    int ret( GAtomic::FetchAndAdd(&i, 1) );
    QVERIFY(ret == 1);
}

QTEST_APPLESS_MAIN(AtomicTest);

#include "tst_atomictest.moc"