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
#include "Core/DataObjects/gstring.h"
GUTIL_USING_CORE_NAMESPACE(DataObjects);

class StringTest : public QObject
{
    Q_OBJECT

public:
    StringTest();

private Q_SLOTS:
    void test_basics();
    void test_upperlowercase();
    void test_format();
    void test_compare();
};

StringTest::StringTest()
{
}

void StringTest::test_basics()
{
    String s;
    QVERIFY(s.Length() == 0);
    QVERIFY(s.IsNull());
    QVERIFY(s.IsEmpty());

    s = "";
    QVERIFY(!s.IsNull());
    QVERIFY(s.IsEmpty());
    QVERIFY(s[0] == '\0');

    s.Clear();
    QVERIFY(s.IsNull());
    QVERIFY(s.IsEmpty());

    s = "Hello World!";
    QVERIFY(s.Length() == strlen("Hello World!"));
    QVERIFY(s.Length() == 12);
    QVERIFY(!s.IsNull());
    QVERIFY2(s == "Hello World!", s);
    QVERIFY2(s != "Hello Frenchie!", s);

    QVERIFY(s[0] == 'H');
    QVERIFY(s[1] == 'e');
    QVERIFY(s[2] == 'l');
    QVERIFY(s[3] == 'l');
    QVERIFY(s[4] == 'o');
    QVERIFY(s[5] == ' ');

    QVERIFY(s.At(6)  == 'W');
    QVERIFY(s.At(7)  == 'o');
    QVERIFY(s.At(8)  == 'r');
    QVERIFY(s.At(9)  == 'l');
    QVERIFY(s.At(10) == 'd');

    QVERIFY(s[11] == '!');
    QVERIFY(s[12] == '\0');
    QVERIFY(s[12] == 0);

    s.Clear();
    QVERIFY(s.Length() == 0);
    QVERIFY(s.Length() == 0);
    QVERIFY(s.Capacity() == 0);
    QVERIFY(s.IsNull());

    s.Append("Hello");
    QVERIFY(s.Length() == 5);
    QVERIFY(s[s.Length()] == '\0');

    s.Append(" George");
    QVERIFY(s.Length() == 12);
    QVERIFY(s[s.Length()] == '\0');

    s.Prepend("Well ");
    QVERIFY(s.Length() == 17);
    QVERIFY(s[s.Length()] == '\0');


    s.Clear();

    s = "One";
    s = s + " plus ";
    QVERIFY2(s == "One plus ", s);
    s += "one equals two";
    QVERIFY2(s == "One plus one equals two", s);
}

void StringTest::test_upperlowercase()
{
    String s("Hello");
    QVERIFY(s == "Hello");

    s.ToUpper();
    QVERIFY(s == "HELLO");

    s.ToLower();
    QVERIFY(s == "hello");

    s = "george.KaraGoulis{[@`";
    QVERIFY(s.ToUpper() == "GEORGE.KARAGOULIS{[@`");
    QVERIFY(s.ToLower() == "george.karagoulis{[@`");
}

void StringTest::test_format()
{
    String s;
    s = String::Format("Hello %s", "George");
    QVERIFY2(s == "Hello George", s);

    s = String::Format("%d + %d = %d", 1, 1, 2);
    QVERIFY2(s == "1 + 1 = 2", s);

    s.Clear();
#define LARGE_STRING "This must have caused the string to resize" \
                        " after this format.  I mean, seriously, this string" \
                        " is HUUUUUUUUUUUUUUUUUUUUUUUGE!!!!"
    s = String::Format("%s", LARGE_STRING);
    QVERIFY2(s == LARGE_STRING, s);
}

void StringTest::test_compare()
{
    String s("B");
    QVERIFY(s > String("A"));
    QVERIFY(s >= String("A"));

    QVERIFY(s < String("C"));
    QVERIFY(s <= String("C"));

    QVERIFY(s == String("B"));
    QVERIFY(s <= String("B"));
    QVERIFY(s >= String("B"));


    QVERIFY(String("Janet") < String("Judith"));
    QVERIFY(String("Janet") <= String("Judith"));
    QVERIFY(String("Judith") > String("Janet"));
    QVERIFY(String("Judith") >= String("Janet"));

    QVERIFY(String("Mark") == String("Mark"));
    QVERIFY(String("Mark") >= String("Mark"));
    QVERIFY(String("Mark") <= String("Mark"));
    QVERIFY(String("Mark") != String("mark"));

    QVERIFY(String("OneString") != String("OneStringLonger"));
    QVERIFY(String("OneString") < String("OneStringLonger"));
    QVERIFY(String("OneString") <= String("OneStringLonger"));
    QVERIFY(String("OneStringLonger") > String("OneString"));
    QVERIFY(String("OneStringLonger") >= String("OneString"));
}

QTEST_APPLESS_MAIN(StringTest);

#include "tst_stringtest.moc"