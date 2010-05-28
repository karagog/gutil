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

#include "crypto_test.h"
#include "exception.h"
#include "encryption.h"
#include <string>
#include <QString>
#include <QFile>
#include <QTest>
using namespace GUtil;
using namespace std;

crypto_test::crypto_test(QObject *parent) :
        QObject(parent)
{
}

void crypto_test::test_strings()
{
    string before = "This is the message";
    string pw = "testpassword";

    string after = CryptoHelpers::encryptString(before.c_str(), pw.c_str());

    //qDebug(QString::fromStdString("Input string: " + before).toStdString().c_str());
    //qDebug(QString::fromStdString("Output string: " + after).toStdString().c_str());
    QVERIFY(before != after);

    string finally = CryptoHelpers::decryptString(after, pw);

    QVERIFY(before == finally);
}

void crypto_test::test_files()
{
    QString contents = "This is a file to test my encryption algorithm";
    char *pw = "testingpasswordreallylong";
    QFile f("testfile_temp.txt");

    // Write data to a file
    QVERIFY(f.open(QFile::WriteOnly | QFile::Truncate));
    QVERIFY(f.write(contents.toAscii()) == contents.length());
    f.close();

    try
    {
        // Encrypt it
        CryptoHelpers::encryptFile(f.fileName().toStdString().c_str(),
                                   (f.fileName() + ".encrypted").toStdString().c_str(),
                                   pw);

        // Decrypt it
        CryptoHelpers::decryptFile((f.fileName() + ".encrypted").toStdString().c_str(),
                                   (f.fileName() + ".decrypted").toStdString().c_str(),
                                   pw);
    }
    catch(exception ex)
    {
        //string probe = ex.GetWhat();
        QVERIFY(false);
    }

    // read in the data
    QFile decf(f.fileName() + ".decrypted");
    QFile encf(f.fileName() + ".encrypted");
    QVERIFY(encf.open(QFile::ReadOnly));
    QVERIFY(decf.open(QFile::ReadOnly));

    QByteArray encrypted(encf.readAll());
    QString decrypted(decf.readAll());
    decf.close();
    encf.close();

    // Compare it
    QVERIFY(encrypted != decrypted.toAscii());
    QVERIFY(decrypted == contents);

    // Cleanup
    QVERIFY(QFile::remove(f.fileName() + ".decrypted"));
    QVERIFY(QFile::remove(f.fileName() + ".encrypted"));
    QVERIFY(f.remove());
}

void crypto_test::test_base64()
{
    // First test a simple one that you can work out on paper
    string in = "sur";
    string out = CryptoHelpers::toBase64(in);
    QVERIFY(out == "c3Vy");

    out = CryptoHelpers::fromBase64(out);
    QVERIFY(out == in);

    // Test that the function gives the correct padding
    in = "sure.";
    out = CryptoHelpers::toBase64(in);
    QVERIFY(out == "c3VyZS4=");
    out = CryptoHelpers::fromBase64(out);
    QVERIFY(out == in);

    in = "sure";
    out = CryptoHelpers::toBase64(in);
    QVERIFY(out == "c3VyZQ==");
    out = CryptoHelpers::fromBase64(out);
    QVERIFY(out == in);

    in = "Man is distinguished, not only by his reason, but by this singular passion from other animals, which is a lust of the mind, that by a perseverance of delight in the continued and indefatigable generation of knowledge, exceeds the short vehemence of any carnal pleasure.";
    out = CryptoHelpers::toBase64(in);
    QVERIFY(out == "TWFuIGlzIGRpc3Rpbmd1aXNoZWQsIG5vdCBvbmx5IGJ5IGhpcyByZWFzb24sIGJ1dCBieSB0aGlzIHNpbmd1bGFyIHBhc3Npb24gZnJvbSBvdGhlciBhbmltYWxzLCB3aGljaCBpcyBhIGx1c3Qgb2YgdGhlIG1pbmQsIHRoYXQgYnkgYSBwZXJzZXZlcmFuY2Ugb2YgZGVsaWdodCBpbiB0aGUgY29udGludWVkIGFuZCBpbmRlZmF0aWdhYmxlIGdlbmVyYXRpb24gb2Yga25vd2xlZGdlLCBleGNlZWRzIHRoZSBzaG9ydCB2ZWhlbWVuY2Ugb2YgYW55IGNhcm5hbCBwbGVhc3VyZS4=");
    out = CryptoHelpers::fromBase64(out);
    QVERIFY(out == in);

    // Test binary encoding
    char bin_dat[] = { 0x00 };
    in.resize(0);
    in.append(bin_dat, 1);
    out = CryptoHelpers::toBase64(in);
    QVERIFY(out == "AA==");

    out = CryptoHelpers::fromBase64(out);
    QVERIFY(out.length() == 1 && out.at(0) == (char)0x00);

    // Test if qstrings can hold binary data
    QString qstr;
    qstr.append((QChar)0x00);
    out = CryptoHelpers::toBase64(qstr.toStdString());
    QVERIFY(out == "AA==");

    // Note: The cryptoPP implementation of base64 does not do validation, so no
    //   need to test bad strings
}

void crypto_test::test_base16()
{
    // First test a simple one that you can work out on paper
    string in;
    char tc = 0x04;
    in.append(&tc, 1);
    tc = 0xab;
    in.append(&tc, 1);
    tc = 0x00;
    in.append(&tc, 1);
    string out = CryptoHelpers::toBase16(in);
    QVERIFY(out == "04AB00");

    out = CryptoHelpers::fromBase16(out);
    QVERIFY(out == in);
}

void crypto_test::test_exceptions()
{
    string msg = "Blahbaram";
    string pw = "firstpassword";
    string badpw = "stupidpassword";
    string str1;
    int result = 0;
    bool exception_hit = false;

    str1 = CryptoHelpers::encryptString(msg, pw);

    try
    {
        // Try decrypting with a bad password
        QVERIFY("" == CryptoHelpers::decryptString(msg, badpw));
    }
    catch(GUtil::Exception)
    {
        exception_hit = true;
    }
    QVERIFY(exception_hit);

    // Verify that file exceptions work
    result = 0;
    char *noexist = "thisfiledoesntexist.nothing.here";
    char *noexist2 = "thisfiledoesntexist2.nothing.here";

    exception_hit = false;
    try
    {
        CryptoHelpers::encryptFile(noexist, noexist, "blah");

        // Make sure it works the other way without problems
        CryptoHelpers::decryptFile(noexist, noexist2, "blah");
    }
    catch(GUtil::Exception)
    {
        exception_hit = true;
    }
    QVERIFY(!exception_hit);

    exception_hit = false;
    try
    {
        CryptoHelpers::decryptFile(noexist, noexist, badpw.c_str());
    }
    catch(GUtil::Exception)
    {
        exception_hit = true;
    }
    QVERIFY(exception_hit);

    QVERIFY(QFile::remove(QString(noexist)));
    QVERIFY(QFile::remove(QString(noexist2)));
}

void crypto_test::test_compression()
{
    string in = string(1000, 0x00) + string(1000, 0x02);
    string out = CryptoHelpers::compress(in, CryptoHelpers::MAX_COMPRESSION_LEVEL);
    //qDebug(QString("Size in: %1\nSize out: %2").arg(in.length()).arg(out.length()).toStdString().c_str());
    QVERIFY(out != in);
    QVERIFY(out.length() < in.length());

    out = CryptoHelpers::decompress(out);
    QVERIFY(out == in);
}

void crypto_test::test_random_numbers()
{
    // First test the rand() function
    int prev = CryptoHelpers::rand();
    for(int i = 0; i < 100; i++)
    {
        int rnd = CryptoHelpers::rand();

        QVERIFY(rnd != prev);
        //qDebug(QVariant::fromValue<int>(rnd).toString().toStdString().c_str());

        prev = rnd;
    }

    // Then test the random data function
    string out = CryptoHelpers::randData(25);
    QVERIFY(out.length() == 25);
    //qDebug(CryptoHelpers::toBase16(out).c_str());

    out = CryptoHelpers::randData(25, 456);
    QVERIFY(out.length() == 25);
    //qDebug(CryptoHelpers::toBase16(out).c_str());

    // We should get the same string if we we seed the same
//    string out2 = CryptoHelpers::randData(25, 456);
//    qDebug(CryptoHelpers::toBase16(out2).c_str());
//    QVERIFY(out == out2);
}
