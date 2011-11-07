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
#include "Core/exception.h"
#include <iostream>
GUTIL_USING_CORE_NAMESPACE(DataObjects);
using namespace std;

class StringTest : public QObject
{
    Q_OBJECT

public:
    StringTest();

private Q_SLOTS:
    void test_basics();
    void test_upperlowercase();
    void test_format();
    void test_replace();
    void test_compare();
    void test_indexof();
    void test_utf8();
    void test_utf8_validation();
    void test_number_conversions();
    void test_split_join();
    void test_base64();
    void test_base16();
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

    s.RemoveBytesAt(5, 6);
    QVERIFY(s == "Hello!");
    QVERIFY(s.Length() == 6);
    QVERIFY(s[6] == '\0');
    QVERIFY(strlen(s) == 6);

    // This clobbers the null from before, so we test that it is definitely overwritten
    //  on the following insertion
    s[12] = -1;

    s.Insert(" World", 5);
    QVERIFY2(s == "Hello World!", s);
    QVERIFY(s.Length() == 12);
    QVERIFY(s[12] == '\0');

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


    s.Clear();
    s = "   This string needs to be trimmed    \n";
    s.Trim();
    QVERIFY2(s == "This string needs to be trimmed", s);

    s = "   This string needs to be trimmed    \n";
    QVERIFY2(s.Trimmed() == "This string needs to be trimmed", s.Trimmed());
    QVERIFY2(s == "   This string needs to be trimmed    \n", s);
}

void StringTest::test_upperlowercase()
{
    String s("Hello");
    QVERIFY(s == "Hello");

    s.ToUpper();
    QVERIFY(s == "Hello");
    QVERIFY(s.ToUpper() == "HELLO");

    s.ToLower();
    QVERIFY(s == "Hello");
    QVERIFY(s.ToLower() == "hello");

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

    s = String("%d + %d = %d").Format(1, 1, 2);
    QVERIFY2(s == "1 + 1 = 2", s);

    s.Clear();
#define LARGE_STRING "This must have caused the string to resize" \
                        " after this format.  I mean, seriously, this string" \
                        " is HUUUUUUUUUUUUUUUUUUUUUUUGE!!!!"
    s = String::Format("%s", LARGE_STRING);
    QVERIFY2(s == LARGE_STRING, s);
}

void StringTest::test_replace()
{
    String s("Hello George, my name is George");
    QVERIFY(s == "Hello George, my name is George");

    // First replace with a smaller word
    s = s.Replace("George", "Sue");
    QVERIFY2(s == "Hello Sue, my name is Sue", s);
    QVERIFY(s[s.Length()] == '\0');

    // Now replace with a larger word
    s = s.Replace("Sue", "George");
    QVERIFY2(s == "Hello George, my name is George", s);
    QVERIFY(s[s.Length()] == '\0');

    // Test case sensitivity
    s = s.Replace("george", "Sue", false);
    QVERIFY2(s == "Hello Sue, my name is Sue", s);
    QVERIFY(s[s.Length()] == '\0');

    s = s.Replace("sue", "George", false);
    QVERIFY2(s == "Hello George, my name is George", s);
    QVERIFY(s[s.Length()] == '\0');


    s = String("George GeorgeGeorge George").Replace("George", "Sue");
    QVERIFY2(s == "Sue SueSue Sue", s);
    QVERIFY(s[s.Length()] == '\0');

    s = s.Replace("Sue", "George");
    QVERIFY2(s == "George GeorgeGeorge George", s);
    QVERIFY(s[s.Length()] == '\0');
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

void StringTest::test_indexof()
{
    GUINT32 ind;
    String s("George and George and George");

    ind = s.IndexOf("George");
    QVERIFY(ind == 0);

    ind = s.IndexOf("George", ind + 6);
    QVERIFY(ind == 11);

    ind = s.IndexOf("George", ind + 6);
    QVERIFY(ind == 22);

    ind = s.LastIndexOf("George");
    QVERIFY2(ind == 22, String::Format("%d", ind));

    ind = s.LastIndexOf("George", ind - 1);
    QVERIFY2(ind == 11, String::Format("%d", ind));

    ind = s.LastIndexOf("George", ind - 1);
    QVERIFY(ind == 0);


    QVERIFY(s.IndexOf("Harry") == UINT_MAX);
    QVERIFY(s.IndexOf("George", 100) == UINT_MAX);
    QVERIFY(s.IndexOf("asdfasdfasdfasdfasdfadsfasdfasdfasdfasdfasdfasdfasdfasdfa") == UINT_MAX);

    QVERIFY(s.LastIndexOf("Harry") == UINT_MAX);
    QVERIFY(s.LastIndexOf("George", 100) == UINT_MAX);
    QVERIFY(s.LastIndexOf("asdfasdfasdfasdfasdfadsfasdfasdfasdfasdfasdfasdfasdfasdfa") == UINT_MAX);
}

void StringTest::test_utf8()
{
    QString qs;
    String s;
    const wchar_t *wc = L"Hello World!";

    // Test normal ascii characters
    qs = QString::fromWCharArray(wc);
    s = qs.toUtf8().constData();
    QVERIFY(s.LengthUTF8() == 12);
    QVERIFY2(s.Length() == 12, String::FromInt(s.Length()));

    // Test a 2-byte utf-8 character

    // The character 'Ü' has unicode value 0x00DC, which translates to UTF-8 0xC39C
    const char c1[] = { 0xC3, 0x9C, 0x00 };
    qs = QString::fromUtf8(c1);
    s = qs.toUtf8().constData();
    QVERIFY2(s.LengthUTF8() == 1, String::FromInt(s.LengthUTF8()));
    QVERIFY2(s.Length() == 2, String::FromInt(s.Length()));

    //qDebug() << String::Format("%x", s.beginUTF8().UnicodeValue());
    QVERIFY(s.beginUTF8().UnicodeValue() == 0x00DC);

    // Test a 3-byte UTF-8 character
    // The character '?' has unicode value 0x0B82, which is UTF-8 0xE0AE82
    const char c2[] = { 0xE0, 0xAE, 0x82, 0x00 };
    qs = QString::fromUtf8(c2);
    s = qs.toUtf8().constData();
    QVERIFY2(s.LengthUTF8() == 1, String::FromInt(s.LengthUTF8()));
    QVERIFY2(s.Length() == 3, String::FromInt(s.Length()));

    QVERIFY2(s.beginUTF8().UnicodeValue() == 0x0B82, String::FromInt(s.beginUTF8().UnicodeValue()));


    // Test a combination of ascii and utf-8 characters from above
    qs = QString::fromUtf8(c1);
    qs.append("A");
    qs.append(QString::fromUtf8(c2));
    s = qs.toUtf8().constData();
    QVERIFY2(s.Length() == 6, String::FromInt(s.Length()));
    QVERIFY2(s.LengthUTF8() == 3, String::FromInt(s.LengthUTF8()));
}


void StringTest::test_utf8_validation()
{
    QString qs;
    String s;

    const char goodchar_1[] = { 0xC3, 0x9C, 0x00 };     // Ü
    const char goodchar_2[] = { 0xE0, 0xAE, 0x82, 0x00 };

    const char badchar_1[] = { 0x8F, 0x00 };    // Invalid UTF-8 (starts with continuation byte)
    const char badchar_2[] = { 0xE0, 0xAE, 0x80, 0x82, 0x00 };    // Invalid UTF-8 (extra continuation byte)
    const char badchar_3[] = { 0xE0, 0x00, 0x82, 0x00 };    // Invalid UTF-8 (premature null)

    s = "HI";
    s.Append(badchar_1);
    QVERIFY2(s.LengthUTF8() == 3, String::FromInt(s.LengthUTF8()));
    QVERIFY2(s.Length() == 3, String::FromInt(s.Length()));
    QVERIFY(!s.IsValidUTF8());

    s.Append(goodchar_1);
    QVERIFY2(s.LengthUTF8() == 4, String::FromInt(s.LengthUTF8()));
    QVERIFY2(s.Length() == 5, String::FromInt(s.Length()));
    QVERIFY(!s.IsValidUTF8());

    // Try removing the invalid character (should be valid afterwards)
    s.RemoveBytesAt(2, 1);
    QVERIFY2(s.LengthUTF8() == 3, String::FromInt(s.LengthUTF8()));
    QVERIFY2(s.Length() == 4, String::FromInt(s.Length()));
    QVERIFY(s.IsValidUTF8());




}


void StringTest::test_number_conversions()
{
    String s;

    s = String::FromInt(5);
    QVERIFY(s == "5");
    QVERIFY(s.ToInt() == 5);

    s = String::FromInt(-5);
    QVERIFY(s == "-5");
    QVERIFY(s.ToInt() == -5);

    s = String::FromInt(101);
    QVERIFY(s == "101");
    QVERIFY(s.ToInt() == 101);

    s = String::FromFloat(1);
    QVERIFY(s.ToFloat() == 1);

    s = String::FromFloat(1.5);
    QVERIFY(s.ToFloat() == 1.5);

    float f( 741.58465 );
    s = String::FromFloat(f);
    QVERIFY2(s.ToFloat() == f, s);


    s = String::FromBool(true);
    QVERIFY(s.ToBool() == true);

    s = String::FromBool(false);
    QVERIFY(s.ToBool() == false);


    bool ok = true;
    s = "Not a number";
    s.ToInt(&ok);
    QVERIFY(ok == false);

    ok = true;
    s.ToFloat(&ok);
    QVERIFY(ok == false);

    ok = true;
    s.ToBool(&ok);
    QVERIFY(ok == false);

    ok = true;
    s = "B48";
    s.ToInt(&ok);
    QVERIFY2(ok == false, String::FromInt(s.ToInt()));
}

void StringTest::test_split_join()
{
    String s("George,Jim,Sue,Patty,");
    Vector<String> v( s.Split(',') );
    QVERIFY(v.Length() == 5);
    QVERIFY2(v[0] == "George", v[0]);
    QVERIFY2(v[1] == "Jim", v[1]);
    QVERIFY(v[2] == "Sue");
    QVERIFY2(v[3] == "Patty", v[3]);
    QVERIFY(v[4] == "");


    v = s.Split(',', false);
    QVERIFY(v.Length() == 4);
    QVERIFY2(v[0] == "George", v[0]);
    QVERIFY2(v[1] == "Jim", v[1]);
    QVERIFY(v[2] == "Sue");
    QVERIFY2(v[3] == "Patty", v[3]);


    // Now try with unicode characters
    const char c[] = { 0xE0, 0xAE, 0x82, 0x00 };
    QString qs("George");
    qs.append(QString::fromUtf8(c));
    qs.append("Jim");
    qs.append(QString::fromUtf8(c));
    qs.append("Sue");
    qs.append(QString::fromUtf8(c));
    qs.append("Patty");
    qs.append(QString::fromUtf8(c));
    s = qs.toUtf8().constData();

    v = s.Split(c);
    QVERIFY2(v.Length() == 5, String::FromInt(v.Length()));
    QVERIFY2(v[0] == "George", v[0]);
    QVERIFY(v[1] == "Jim");
    QVERIFY(v[2] == "Sue");
    QVERIFY(v[3] == "Patty");
    QVERIFY(v[4] == "");


    Vector<String> lst;
    lst.PushBack("George");
    lst.PushBack("Jim");
    lst.PushBack("Sue");
    lst.PushBack("Patty");
    s = String::Join(lst, ',');
    QVERIFY(s == "George,Jim,Sue,Patty");
}

void StringTest::test_base64()
{
    String s("Hello World!");
    String b64 = s.ToBase64();
    QVERIFY2(b64 == "SGVsbG8gV29ybGQh", b64);

    s = String::FromBase64("TXkgbmFtZSBpcyBKb25hcw==");
    QVERIFY2(s == "My name is Jonas", s);
}

void StringTest::test_base16()
{
    String s("Hello World!");
    String b16 = s.ToBase16();
    QVERIFY2(b16 == "48656C6C6F20576F726C6421", b16);

    s = b16.FromBase16();
    QVERIFY2(s == "Hello World!", s);
}

QTEST_APPLESS_MAIN(StringTest);

#include "tst_stringtest.moc"
