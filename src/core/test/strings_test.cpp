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

#include "strings_test.h"
#include "stringhelpers.h"
#include <string>
using namespace std;
using namespace GUtil;

strings_test::strings_test(QObject *parent) :
    QObject(parent)
{
}

void strings_test::test_pathname()
{
    string testpath = "C:/users/test";
    string teststring = StringHelpers::pathName(testpath);
    if(teststring != "C:/users/")
    {
        qDebug(teststring.c_str());
        QVERIFY(false);
    }

    testpath = "relative/path///";
    teststring = StringHelpers::pathName(testpath);
    if(teststring != "relative/")
    {
        qDebug(teststring.c_str());
        QVERIFY(false);
    }

    testpath = "nopath";
    teststring = StringHelpers::pathName(testpath);
    if(teststring != "./")
    {
        qDebug(teststring.c_str());
        QVERIFY(false);
    }

    testpath = "D:\\windows\\path";
    teststring = StringHelpers::pathName(testpath);
    if(teststring != "D:/windows/")
    {
        qDebug(teststring.c_str());
        QVERIFY(false);
    }
}

void strings_test::test_basename()
{
    string testpath = "C:/users/test";
    string teststring = StringHelpers::baseName(testpath);
    if(teststring != "test")
    {
        qDebug(teststring.c_str());
        QVERIFY(false);
    }

    testpath = "relative/path///";
    teststring = StringHelpers::baseName(testpath);
    if(teststring != "path")
    {
        qDebug(teststring.c_str());
        QVERIFY(false);
    }

    testpath = "nopath";
    teststring = StringHelpers::baseName(testpath);
    if(teststring != "nopath")
    {
        qDebug(teststring.c_str());
        QVERIFY(false);
    }

    testpath = "D:\\windows\\path";
    teststring = StringHelpers::baseName(testpath);
    if(teststring != "path")
    {
        qDebug(teststring.c_str());
        QVERIFY(false);
    }
}

void strings_test::test_hex()
{
    char t = '0';
    char u = '0';
    char h = 0x00;

    // Verify upper and lower cases
    QVERIFY(StringHelpers::charToHex('B') == StringHelpers::charToHex('b'));

    // Verify that the char comes back upper-case
    QVERIFY(StringHelpers::hexToChar(0x0b) == 'B');

    for(int i = 0; i < 16; i++)
    {
        char probe1 = StringHelpers::charToHex(t);
        char probe2 = StringHelpers::hexToChar(h);
        char probe3 = StringHelpers::charToHex(u);

        QVERIFY(probe1 == h);
        QVERIFY(probe2 == t);
        QVERIFY(probe3 == h);

        h++;
        if(t++ == '9')
            t = 'A';

        // Try lower case too
        if(u++ == '9')
            u = 'a';
    }
}

void strings_test::test_base64()
{
    // First test a simple one that you can work out on paper
    string in = "sur";
    string out = StringHelpers::toBase64(in);
    QVERIFY(out == "c3Vy");

    out = StringHelpers::fromBase64(out);
    QVERIFY(out == in);

    // Test that the function gives the correct padding
    in = "sure.";
    out = StringHelpers::toBase64(in);
    QVERIFY(out == "c3VyZS4=");
    out = StringHelpers::fromBase64(out);
    QVERIFY(out == in);

    in = "sure";
    out = StringHelpers::toBase64(in);
    QVERIFY(out == "c3VyZQ==");
    out = StringHelpers::fromBase64(out);
    QVERIFY(out == in);

    in = "Man is distinguished, not only by his reason, but by this singular passion from other animals, which is a lust of the mind, that by a perseverance of delight in the continued and indefatigable generation of knowledge, exceeds the short vehemence of any carnal pleasure.";
    out = StringHelpers::toBase64(in);
    QVERIFY(out == "TWFuIGlzIGRpc3Rpbmd1aXNoZWQsIG5vdCBvbmx5IGJ5IGhpcyByZWFzb24sIGJ1dCBieSB0aGlzIHNpbmd1bGFyIHBhc3Npb24gZnJvbSBvdGhlciBhbmltYWxzLCB3aGljaCBpcyBhIGx1c3Qgb2YgdGhlIG1pbmQsIHRoYXQgYnkgYSBwZXJzZXZlcmFuY2Ugb2YgZGVsaWdodCBpbiB0aGUgY29udGludWVkIGFuZCBpbmRlZmF0aWdhYmxlIGdlbmVyYXRpb24gb2Yga25vd2xlZGdlLCBleGNlZWRzIHRoZSBzaG9ydCB2ZWhlbWVuY2Ugb2YgYW55IGNhcm5hbCBwbGVhc3VyZS4=");
    out = StringHelpers::fromBase64(out);
    QVERIFY(out == in);

    // Test binary encoding
    char bin_dat[] = { 0x00 };
    in.resize(0);
    in.append(bin_dat, 1);
    out = StringHelpers::toBase64(in);
    QVERIFY(out == "AA==");

    out = StringHelpers::fromBase64(out);
    QVERIFY(out.length() == 1 && out.at(0) == (char)0x00);

    // Test if qstrings can hold binary data
    QString qstr;
    qstr.append((QChar)0x00);
    out = StringHelpers::toBase64(qstr.toStdString());
    QVERIFY(out == "AA==");

    // Test any error cases to make sure they're caught
    // Note: The cryptoPP implementation of base64 does not do validation

    in = "thisbase;64strin";
    QVERIFY(StringHelpers::fromBase64(in) == "");

    in = "this=base64strin";
    QVERIFY(StringHelpers::fromBase64(in) == "");

    in = "thi=sbase64strin";
    QVERIFY(StringHelpers::fromBase64(in) == "");

    in = "thisbase64s=trin";
    QVERIFY(StringHelpers::fromBase64(in) == "");

    in = "th=isbase64strin";
    QVERIFY(StringHelpers::fromBase64(in) == "");

    in = "thisbase64stri=g";
    QVERIFY(StringHelpers::fromBase64(in) == "");

    in = "wrongnumberofchars";
    QVERIFY(StringHelpers::fromBase64(in) == "");
}

void strings_test::test_base16()
{
    // First test a simple one that you can work out on paper
//    string in;
//    char tc = 0x04;
//    in.append(&tc, 1);
//    tc = 0xab;
//    in.append(&tc, 1);
//    tc = 0x00;
//    in.append(&tc, 1);
//    string out = StringHelpers::toBase16(in);
//    QVERIFY(out == "04AB00");
//
//    out = StringHelpers::fromBase16(out);
//    QVERIFY(out == in);
}

//void strings_test::test_chars()
//{
//    char c = 0b00000110;
//    char d = StringHelpers::rotate(c);
//    QVERIFY(d == 0b01100000);
//
//    c = 0b01101001;
//    d = StringHelpers::rotate(c);
//
////    cout<<"Before: "; StringHelpers::showBits(c, cout);
////    cout<<endl;
////    cout<<"After: ";  StringHelpers::showBits(d, cout);
////    cout<<endl;
//    QVERIFY(d == (char)0b10010110);
//
//    c = 0b10001000;
//    d = StringHelpers::rotate(c);
//    QVERIFY(d == (char)0b00010001);
//}
