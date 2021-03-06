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
USING_NAMESPACE_GUTIL;

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
    Uuid uid;

    QVERIFY(uid.IsNull());

    uid.Generate();
    QVERIFY(!uid.IsNull());

    qDebug(uid.ToString());


    Uuid uid_short(true);
    qDebug(uid_short.ToString());

    // Can it parse capitals and lower case, and strip the leading and ending brackets?
    uid_short = Uuid::FromString("{abcdef12-3456-4123-a212-ABCDEFABABAB}");
    qDebug(uid_short.ToString());
}

QTEST_APPLESS_MAIN(UuidTest)

#include "tst_uuidtest.moc"
