/*Copyright 2014 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "gutil_gpsutils.h"
#include "gutil_file.h"
#include <QString>
#include <QtTest>
USING_NAMESPACE_GUTIL;


#define FILENAME1 "testfile.gps"
#define FILENAME2 "testfile2.gps"

class GpsutilsTest : public QObject
{
    Q_OBJECT

public:
    GpsutilsTest();
    ~GpsutilsTest();

private Q_SLOTS:
    void test_export();
    void test_import();

private:
    static void _cleanup();
};

void GpsutilsTest::_cleanup()
{
    if(File::Exists(FILENAME1))
        File::Delete(FILENAME1);
    if(File::Exists(FILENAME2))
        File::Delete(FILENAME2);
}

GpsutilsTest::GpsutilsTest()
{
    _cleanup();
}

GpsutilsTest::~GpsutilsTest()
{
    _cleanup();
}

#define PAYLOAD1 "Hello World!"
#define PAYLOAD2 "George is the greatest!"

#define USERDATA_SIZE 32
#define USERDATA1 "George"
#define USERDATA2 "Karagoulis"

#define PAYLOADFILE1 "payload1.txt"

void GpsutilsTest::test_export()
{
    // Test simple export
    {
        // This should at least create a GPS file with no payloads
        GPSFile_Export fe(FILENAME1, "password");
    }

    {
        GPSFile_Export fe(FILENAME2, "password", NULL, USERDATA_SIZE);
        QVERIFY(fe.UserDataSize() == USERDATA_SIZE);

        fe.AppendPayload((const byte *)PAYLOAD1, sizeof(PAYLOAD1),
                         (const byte *)USERDATA1, sizeof(USERDATA1));
        fe.AppendPayload((const byte *)PAYLOAD2, sizeof(PAYLOAD2),
                         (const byte *)USERDATA2, sizeof(USERDATA2));
        fe.AppendPayloadFile(PAYLOADFILE1);
    }

    File f1(FILENAME1);
    File f2(FILENAME2);
    QVERIFY(f1.Length() > 0);
    QVERIFY(f2.Length() > 0);
}

void GpsutilsTest::test_import()
{
    {
        GPSFile_Import fi(FILENAME1, "password");
        QVERIFY(fi.UserDataSize() == 0);
        QVERIFY(!fi.NextPayload());
        QVERIFY(0 == fi.CurrentPayloadSize());
    }

    // Compare the imported payloads with the known payloads we exported,
    // and check that the user data is zero outside of the data we entered.
    {
        byte zeros[USERDATA_SIZE] = {};
        char buf[100];
        GPSFile_Import fi(FILENAME2, "password");
        QVERIFY(fi.UserDataSize() == USERDATA_SIZE);

        // Get first payload
        QVERIFY(fi.NextPayload());
        QVERIFY(sizeof(PAYLOAD1) == fi.CurrentPayloadSize());
        fi.GetCurrentPayload((byte *)buf);
        QVERIFY(0 == strcmp(buf, PAYLOAD1));
        QVERIFY(0 == strcmp((const char *)fi.CurrentUserData(), USERDATA1));

        // The rest of the user data should be zeros
        QVERIFY(0 == memcmp(zeros, fi.CurrentUserData() + sizeof(USERDATA1), USERDATA_SIZE - sizeof(USERDATA1)));


        // Get second payload
        QVERIFY(fi.NextPayload());
        QVERIFY(sizeof(PAYLOAD2) == fi.CurrentPayloadSize());
        fi.GetCurrentPayload((byte *)buf);
        QVERIFY(0 == strcmp(buf, PAYLOAD2));
        QVERIFY(0 == strcmp((const char *)fi.CurrentUserData(), USERDATA2));

        // The rest of the user data should be zeros
        QVERIFY(0 == memcmp(zeros, fi.CurrentUserData() + sizeof(USERDATA2), USERDATA_SIZE - sizeof(USERDATA2)));


        // Get third payload, which was a file
        byte buf2[100];
        File f(PAYLOADFILE1);
        f.Open(File::OpenRead);
        f.Read(buf2, sizeof(buf2));
        QVERIFY(fi.NextPayload());
        QVERIFY(f.Length() == fi.CurrentPayloadSize());
        fi.GetCurrentPayload((byte *)buf);
        QVERIFY(0 == memcmp(buf, buf2, fi.CurrentPayloadSize()));

        // We didn't set the user data so it should be all zeros
        QVERIFY(0 == memcmp(zeros, fi.CurrentUserData(), USERDATA_SIZE));

        // No more payloads
        QVERIFY(!fi.NextPayload());
    }
}

QTEST_APPLESS_MAIN(GpsutilsTest)

#include "tst_gpsutilstest.moc"
