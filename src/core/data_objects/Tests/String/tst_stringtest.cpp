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
#include <iostream>
#include "gutil_strings.h"
#include "gutil_extendedexception.h"
#include "gutil_crypto.h"
#include "gutil_consolelogger.h"
USING_NAMESPACE_GUTIL;
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
    void test_utf8_generation();
    void test_number_conversions();
    void test_split_join();
    void test_base64();
    void test_base16();
    void test_qt_adapters();
    void test_utf8_vectors();

private:
    void test_vector_UTF8basics();
    void test_vector_UTF8indexof();
    void test_vector_UTF8lastindexof();
    void test_vector_UTF8chop();
    void test_vector_UTF8truncate();
    void test_vector_UTF8trim();
    void test_vector_UTF8tolower();
    void test_vector_UTF8toupper();
    void test_vector_UTF8substring();
    void test_vector_UTF8format();
    void test_vector_UTF8split();
    void test_vector_UTF8join();
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
    QVERIFY2(s == "   This string needs to be trimmed    \n", s);

    String trimmed = s.Trimmed();
    QVERIFY2(trimmed == "This string needs to be trimmed", trimmed);
    QVERIFY2(s == "   This string needs to be trimmed    \n", s);


    s = "Chop the string here";
    QVERIFY2(s.Chop(5) == "Chop the string", s.Chop(5));
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

    s = String::Format("%d + %d = %d", 1, 1, 2);
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
    GINT32 ind;
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


    QVERIFY(s.IndexOf("Harry") == -1);
    QVERIFY(s.IndexOf("George", 100) == -1);
    QVERIFY(s.IndexOf("asdfasdfasdfasdfasdfadsfasdfasdfasdfasdfasdfasdfasdfasdfa") == -1);

    QVERIFY(s.LastIndexOf("Harry") == -1);
    QVERIFY(s.LastIndexOf("George", 100) == -1);
    QVERIFY(s.LastIndexOf("asdfasdfasdfasdfasdfadsfasdfasdfasdfasdfasdfasdfasdfasdfa") == -1);
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

    // The character '�' has unicode value 0x00DC, which translates to UTF-8 0xC39C
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

    const char goodchar_1[] = { 0xC3, 0x9C, 0x00 };     // �
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

void StringTest::test_utf8_generation()
{
    // Iterate through the unicode space and make sure we are generating the correct bytes
    GUINT32 cur(1);
    char buf[10];
    while(cur < 0x80000000)
    {
        GINT32 unicode_value = cur - 1;

        String::UTF8CharFromUnicode(buf, unicode_value);

        GINT32 feedback = String::UnicodeValue(buf);
        QVERIFY2(feedback == unicode_value, String::Format("%x != %x", feedback, unicode_value));

        cur = cur << 1;
    }
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
    StringList v( s.Split(',') );
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


    StringList lst;
    lst.Append("George");
    lst.Append("Jim");
    lst.Append("Sue");
    lst.Append("Patty");
    s = String::Join(lst, ',');
    QVERIFY(s == "George,Jim,Sue,Patty");
}

void StringTest::test_base64()
{
    String s("Hello World!");
    String b64 = s.ToBase64();
    QVERIFY2(b64 == "SGVsbG8gV29ybGQh", b64);

    s = String::FromBase64("SGVsbG8gV29ybGQh");
    QVERIFY2(s == "Hello World!", s);

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

void StringTest::test_qt_adapters()
{
    QString qs("HI");
    String s = String::FromQString(qs);
    QVERIFY(s == "HI");

    qs = s.ToQString();
    QVERIFY(qs == "HI");
}


void StringTest::test_utf8_vectors()
{
    try
    {
        test_vector_UTF8basics();
        test_vector_UTF8indexof();
        test_vector_UTF8lastindexof();
        test_vector_UTF8chop();
        test_vector_UTF8truncate();
        test_vector_UTF8trim();
        test_vector_UTF8tolower();
        test_vector_UTF8toupper();
        test_vector_UTF8substring();
        test_vector_UTF8format();
        test_vector_UTF8split();
        test_vector_UTF8join();
    }
    catch(const Exception<> & ex)
    {
        Exception<true> const *ex_ptr( dynamic_cast<Exception<true> const *>(&ex) );
        qDebug() << ex.What();
        if(ex_ptr)
            qDebug() << ex_ptr->Message();
        throw;
    }
}


#include "test/testvectorreader.h"
#define TESTVECTORS_DIRECTORY "TestVectors"
USING_NAMESPACE_GUTIL1(Test);

void StringTest::test_vector_UTF8basics()
{
    TestVectorReader rdr(TESTVECTORS_DIRECTORY "/UTF8_Basics.vec");

    String s;
    QByteArray line;
    while(rdr.ReadNextLine(line))
    {
        int indx = line.indexOf(' ');
        bool ok(false);
        GINT32 size = line.left(indx).toInt(&ok);
        QVERIFY(ok);

        line = line.right(line.length() - (indx + 1));

        s = String(line.constData(), line.length());
        QVERIFY2(s.LengthUTF8() == size, String::Format("%d != %d", s.LengthUTF8(), size));
    }
}

void StringTest::test_vector_UTF8indexof()
{
    TestVectorReader rdr(TESTVECTORS_DIRECTORY "/UTF8_IndexOf.vec");

    String s;
    QByteArray line;
    while(rdr.ReadNextLine(line))
    {
        char srch[10];
        int indx1 = line.indexOf(' ');
        int indx2 = line.indexOf(' ', indx1 + 1);
        QVERIFY(indx1 != -1 && indx2 != -1);

        memcpy(srch, line.constData(), indx1);

        bool ok(false);
        GINT32 indx = line.left(indx2).right(indx2 - (indx1 + 1)).toInt(&ok);
        QVERIFY(ok);

        line = line.right(line.length() - (indx2 + 1));
        s = String(line.constData(), line.length());

        GINT32 val(s.IndexOfUTF8(srch, 0, 1));
        QVERIFY2(val == indx, String::Format("%d != %d", val, indx));
    }
}

void StringTest::test_vector_UTF8lastindexof()
{
    TestVectorReader rdr(TESTVECTORS_DIRECTORY "/UTF8_LastIndexOf.vec");

    String s;
    QByteArray line;
    while(rdr.ReadNextLine(line))
    {
        char srch[10];
        int indx1 = line.indexOf(' ');
        int indx2 = line.indexOf(' ', indx1 + 1);
        QVERIFY(indx1 != -1 && indx2 != -1);

        memcpy(srch, line.constData(), indx1);

        bool ok(false);
        GINT32 indx = line.left(indx2).right(indx2 - (indx1 + 1)).toInt(&ok);
        QVERIFY(ok);

        line = line.right(line.length() - (indx2 + 1));
        s = String(line.constData(), line.length());

        GINT32 val(s.LastIndexOfUTF8(srch, -1, 1));
        QVERIFY2(val == indx, String::Format("%d != %d", val, indx));
    }
}

void StringTest::test_vector_UTF8chop()
{
    TestVectorReader rdr(TESTVECTORS_DIRECTORY "/UTF8_Chop.vec");

    int pair_cnt(0);
    int state(0);
    String s;
    QByteArray line;
    while(rdr.ReadNextLine(line))
    {
        if(state == 0)
        {
            state = 1;
            ++pair_cnt;

            int indx = line.indexOf(' ');
            QVERIFY(indx != -1);

            bool ok(false);
            GINT32 N = line.left(indx).toInt(&ok);
            QVERIFY(ok);

            line = line.right(line.length() - (indx + 1));
            s = String(line.constData(), line.length()).ChopUTF8(N);
        }
        else if(state == 1)
        {
            state = 0;

            String s2(line.constData(), line.length());
            QVERIFY2(s == s2, String::Format("Pair #%d: %s != %s", pair_cnt, s.ConstData(), s2.ConstData()));
        }
    }
}

void StringTest::test_vector_UTF8truncate()
{
    TestVectorReader rdr(TESTVECTORS_DIRECTORY "/UTF8_Truncate.vec");

    int pair_cnt(0);
    int state(0);
    String s;
    QByteArray line;
    while(rdr.ReadNextLine(line))
    {
        if(state == 0)
        {
            state = 1;
            ++pair_cnt;

            int indx = line.indexOf(' ');
            QVERIFY(indx != -1);

            bool ok(false);
            GINT32 N = line.left(indx).toInt(&ok);
            QVERIFY(ok);

            line = line.right(line.length() - (indx + 1));
            s = String(line.constData(), line.length()).TruncateUTF8(N);
        }
        else if(state == 1)
        {
            state = 0;

            String s2(line.constData(), line.length());
            QVERIFY2(s == s2, String::Format("Pair #%d: %s != %s", pair_cnt, s.ConstData(), s2.ConstData()));
        }
    }
}

void StringTest::test_vector_UTF8trim()
{
    TestVectorReader rdr(TESTVECTORS_DIRECTORY "/UTF8_Trim.vec");

    int pair_cnt(0);
    int state(0);
    String s;
    QByteArray line;
    while(rdr.ReadNextLine(line))
    {
        if(state == 0)
        {
            state = 1;
            ++pair_cnt;

            s = String(line.constData(), line.length()).Trimmed();
        }
        else if(state == 1)
        {
            state = 0;

            String s2(line.constData(), line.length());
            QVERIFY2(s == s2, String::Format("Pair #%d: %s != %s", pair_cnt, s.ConstData(), s2.ConstData()));
        }
    }
}

void StringTest::test_vector_UTF8tolower()
{
    TestVectorReader rdr(TESTVECTORS_DIRECTORY "/UTF8_ToLower.vec");

    int pair_cnt(0);
    int tmp_state(0);
    String s;
    QByteArray line;
    while(rdr.ReadNextLine(line))
    {
        if(tmp_state == 0)
        {
            tmp_state = 1;
            pair_cnt++;
            s = String(line.constData(), line.length());
        }
        else if(tmp_state == 1)
        {
            tmp_state = 0;
            String s2(line.constData(), line.length());
            QVERIFY2(s.ToLower() == s2, String::Format("Pair #%d: %s != %s", pair_cnt,
                                                       s.ToLower().ToBase16().ConstData(),
                                                       s2.ToBase16().ConstData()));
        }
    }
}

void StringTest::test_vector_UTF8toupper()
{
    TestVectorReader rdr(TESTVECTORS_DIRECTORY "/UTF8_ToUpper.vec");

    int pair_cnt(0);
    int tmp_state(0);
    String s;
    QByteArray line;
    while(rdr.ReadNextLine(line))
    {
        if(tmp_state == 0)
        {
            tmp_state = 1;
            pair_cnt++;
            s = String(line.constData(), line.length());
        }
        else if(tmp_state == 1)
        {
            tmp_state = 0;
            String s2(line.constData(), line.length());
            QVERIFY2(s.ToUpper() == s2, String::Format("Pair #%d: %s != %s", pair_cnt,
                                                       s.ToUpper().ToBase16().ConstData(),
                                                       s2.ToBase16().ConstData()));
        }
    }
}

void StringTest::test_vector_UTF8substring()
{
    TestVectorReader rdr(TESTVECTORS_DIRECTORY "/UTF8_SubString.vec");

    int pair_cnt(0);
    int state(0);
    String s;
    QByteArray line;
    while(rdr.ReadNextLine(line))
    {
        if(state == 0)
        {
            state = 1;
            ++pair_cnt;

            int indx1 = line.indexOf(' ');
            int indx2 = line.indexOf(' ', indx1 + 1);
            QVERIFY(indx1 != -1 && indx2 != -1);

            bool ok(false);
            GINT32 indx = line.left(indx1).toInt(&ok);
            QVERIFY(ok);

            ok = false;
            GINT32 count = line.left(indx2).right(indx2 - (indx1 + 1)).toInt(&ok);
            QVERIFY(ok);

            line = line.right(line.length() - (indx2 + 1));
            s = String(line.constData(), line.length()).SubStringUTF8(indx, count);
        }
        else if(state == 1)
        {
            state = 0;

            String s2(line.constData(), line.length());
            QVERIFY2(s == s2, String::Format("Pair #%d: %s != %s", pair_cnt, s.ConstData(), s2.ConstData()));
        }
    }
}

void StringTest::test_vector_UTF8format()
{
    TestVectorReader rdr(TESTVECTORS_DIRECTORY "/UTF8_Format.vec");

    int pair_cnt(0);
    int state(0);
    String s;
    QByteArray line;
    StringList args;
    while(rdr.ReadNextLine(line))
    {
        if(state == 0)
        {
            state = 1;
            ++pair_cnt;
            args.Clear();

            int indx1 = line.indexOf('"');
            int indx2 = line.indexOf('"', indx1 + 1);
            QVERIFY(indx1 != -1 && indx2 != -1);

            s = line.left(indx2).right(indx2 - (indx1 + 1)).constData();

            args = String(line.right(line.length() - (indx2 + 2)).constData()).Split(',');
            QVERIFY(args.Length() == 2);
        }
        else if(state == 1)
        {
            state = 0;

            String s2(line.constData(), line.length());
            s = String::Format(s, args[0].ConstData(), args[1].ConstData());
            QVERIFY2(s == s2, String::Format("Pair #%d: %s != %s", pair_cnt, s.ConstData(), s2.ConstData()));
        }
    }
}

void StringTest::test_vector_UTF8split()
{
    TestVectorReader rdr(TESTVECTORS_DIRECTORY "/UTF8_Split.vec");

    int group_cnt(0);
    int state(0);
    char sep[10];
    int cnt(0);
    int line_cnt(0);
    StringList items;
    String s;
    QByteArray line;
    while(rdr.ReadNextLine(line))
    {
        if(state == 0)
        {
            state = 1;
            ++group_cnt;
            line_cnt = 0;
            items.Clear();

            int indx1 = line.indexOf(' ');
            int indx2 = line.indexOf(' ', indx1 + 1);
            QVERIFY(indx1 != -1 && indx2 != -1);

            memcpy(sep, line.constData(), indx1);
            sep[indx1] = '\0';

            bool ok(false);
            cnt = line.left(indx2).right(indx2 - (indx1 + 1)).toInt(&ok);

            line = line.right(line.length() - (indx2 + 1));
            s = String(line.constData(), line.length());
            items = s.Split(sep);
        }
        else if(state == 1)
        {
            if(line_cnt == cnt - 1)
                state = 0;

            String s2(line.constData(), line.length());
            QVERIFY2(items[line_cnt] == s2, String::Format("Group #%d: %s != %s", group_cnt, items[line_cnt].ConstData(), s2.ConstData()));
            line_cnt++;
        }
    }
}

void StringTest::test_vector_UTF8join()
{
    TestVectorReader rdr(TESTVECTORS_DIRECTORY "/UTF8_Join.vec");

    int pair_cnt(0);
    int state(0);
    char sep[10];
    String s;
    QByteArray line;
    while(rdr.ReadNextLine(line))
    {
        if(state == 0)
        {
            state = 1;
            ++pair_cnt;

            int indx = line.indexOf(' ');
            QVERIFY(indx != -1);

            memcpy(sep, line.constData(), indx);

            line = line.right(line.length() - (indx + 1));
            s = String(line.constData(), line.length());

            s = String::Join(s.Split(' '), sep, indx);
        }
        else if(state == 1)
        {
            state = 0;

            String s2(line.constData(), line.length());
            QVERIFY2(s == s2, String::Format("Pair #%d: %s != %s", pair_cnt, s.ConstData(), s2.ConstData()));
        }
    }
}


#include <QCoreApplication>
class test_app : public QCoreApplication
{
public:

    test_app(int argc, char **argv):QCoreApplication(argc, argv) {}

    bool notify(QObject *o, QEvent *e)
    {
        bool ret;
        try
        {
            ret = QCoreApplication::notify(o, e);
        }
        catch(const Exception<> &ex)
        {
            ConsoleLogger().LogException(ex);
        }
        return ret;
    }
};

int main(int argc, char**argv)
{
    test_app app(argc, argv);
    StringTest tc;
    return QTest::qExec(&tc, argc, argv);
}

#include "tst_stringtest.moc"
