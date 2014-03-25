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
#include "gutil_core.h"
USING_NAMESPACE_GUTIL;

class ConsoleTest : public QObject
{
    Q_OBJECT

public:
    ConsoleTest();

private Q_SLOTS:
    void test_basics();
};

ConsoleTest::ConsoleTest()
{
}

void ConsoleTest::test_basics()
{
    Console::WriteLine();
    Console::WriteLine("Hello World!");
    Console::WriteLine();

    Console::Write("Please enter the name Bob: ");
    String res = Console::ReadLine();
    QVERIFY2(res.ToLower() == "bob", res.ToLower());

    Console::Write("Please enter the name Sue: ");
    res = Console::Read();
    QVERIFY2(res.ToLower() == "sue", res.ToLower());

    Console::Write("Please enter the number 25: ");
    GINT32 n = Console::ReadInteger();
    QVERIFY2(n == 25, String::FromInt(n));

    Console::Write("Please enter the first 3 digits of pi: ");
    GFLOAT32 f = Console::ReadFloat();
    QVERIFY2(0 == gFuzzyCompare32(f, 3.14), String::FromFloat(f));
}

QTEST_APPLESS_MAIN(ConsoleTest);

#include "tst_consoletest.moc"
