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
#include "gutil_id.h"

USING_NAMESPACE_GUTIL1(DataObjects);

class IdTest : public QObject
{
    Q_OBJECT
    
public:
    IdTest();
    
private Q_SLOTS:
    void test_basic_function();
};

IdTest::IdTest()
{
}

void IdTest::test_basic_function()
{
    Id<32> id;
    QVERIFY(id.IsNull());
    qDebug(id.ToString());

    id.Generate();
    QVERIFY(!id.IsNull());
    qDebug(id.ToString());

    Id<8> id_short(true);
    qDebug(id_short.ToString());
}

QTEST_APPLESS_MAIN(IdTest)

#include "tst_idtest.moc"
