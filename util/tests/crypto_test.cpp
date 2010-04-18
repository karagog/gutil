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
#include "cryptohelpers.h"
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

void crypto_test::test_exceptions()
{
    string msg = "Blahbaram";
    string pw = "firstpassword";
    string badpw = "stupidpassword";
    string str1, str2;
    int result = 0;

    str1 = CryptoHelpers::encryptString(msg, pw);

    // Try decrypting with a bad password
    QVERIFY("" == CryptoHelpers::decryptString(msg, badpw));

    // Verify that file exceptions work
    result = 0;
    char *noexist = "thisfiledoesntexist.nothing.here";
    char *noexist2 = "thisfiledoesntexist2.nothing.here";

    QVERIFY(CryptoHelpers::encryptFile(noexist, noexist, "blah"));

    // Make sure it works the other way without problems
    QVERIFY(CryptoHelpers::decryptFile(noexist, noexist2, "blah"));

    QVERIFY(!CryptoHelpers::decryptFile(noexist, noexist, "wrongpassword"));

    QVERIFY(QFile::remove(QString(noexist)));
    QVERIFY(QFile::remove(QString(noexist2)));
}
