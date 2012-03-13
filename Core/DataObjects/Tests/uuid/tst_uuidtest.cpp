/*Copyright 2012 George Karagoulis
  
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
#include "gutil_uuid.h"
USING_NAMESPACE_GUTIL1(DataObjects);

class UuidTest : public QObject
{
    Q_OBJECT
    
public:
    UuidTest();
    
private Q_SLOTS:
    void test_basic_function();
};

UuidTest::UuidTest()
{
}

void UuidTest::test_basic_function()
{
    Uuid<> uid;

    QVERIFY(uid.IsNull());
    QVERIFY(!uid.IsValid());

    uid.Initialize();
    QVERIFY(!uid.IsNull());
    QVERIFY(uid.IsValid());

    qDebug(uid.ToString());


    Uuid<4> uid_short(true);
    qDebug(uid_short.ToString());
}

QTEST_APPLESS_MAIN(UuidTest)

#include "tst_uuidtest.moc"
