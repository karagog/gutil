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
#include "gutil_file.h"
USING_NAMESPACE_GUTIL;


class FileTest : public QObject
{
    Q_OBJECT

public:
    FileTest();
    ~FileTest();

private Q_SLOTS:
    void test_basics();
    void test_errors();
};

#define TEST_FILENAME "testfile.txt"

FileTest::FileTest()
{
    if(File::Exists(TEST_FILENAME))
        File::Delete(TEST_FILENAME);
}

FileTest::~FileTest()
{
    if(File::Exists(TEST_FILENAME))
        File::Delete(TEST_FILENAME);
}

void FileTest::test_basics()
{
    File f(TEST_FILENAME);
    QVERIFY(f.Filename() == TEST_FILENAME);
    QVERIFY(!f.IsOpen());
    QVERIFY(!f.Exists());

    // Test that we can create a file
    f.Open(File::OpenWrite);
    QVERIFY(f.IsOpen());
    QVERIFY(f.Exists());

    // Test our close function
    f.Close();
    QVERIFY(!f.IsOpen());
    QVERIFY(f.Exists());

    // Test that we can open for reading
    f.Open(File::OpenRead);
    QVERIFY(f.IsOpen());

    f.Close();
    QVERIFY(!f.IsOpen());

    // Try writing to a file
    f.Open(File::OpenWrite);
    f.Write("Hello World!");
    f.Close();

    // Try to read it back with another file object
    File f2(TEST_FILENAME);
    f2.Open(File::OpenRead);
    String data( f2.Read() );
    f2.Close();
    QVERIFY(data == "Hello World!");

    // Truncate the file and overwrite with something else
    f.Open(File::OpenWrite);
    f.Write("02345");
    f.Close();

    f2.Open(File::OpenRead);
    data = f2.Read();
    f2.Close();
    QVERIFY(data == "02345");

    // Overwrite only the first byte
    f.Open(File::OpenReadWrite);
    f.Write("1");
    f.Close();

    f2.Open(File::OpenRead);
    data = f2.Read();
    f2.Close();
    QVERIFY2(data == "12345", data);

    // Append data, after reading in the first 3 bytes
    f.Open(File::OpenReadAndAppend);
    data = f.Read(3);
    f.Write("6789");
    f.Close();
    QVERIFY(data == "123");

    f2.Open(File::OpenRead);
    data = f2.Read();
    f2.Close();
    QVERIFY(data == "123456789");

    // Test the read-and-truncate open mode
    f.Open(File::OpenReadWriteTruncate);
    f.Write("Hello World!");
    //data = f.Read();
    f.Close();

    f2.Open(File::OpenRead);
    data = f2.Read();
    f2.Close();
    QVERIFY(data == "Hello World!");

    // Test the append-only open mode
    f.Open(File::OpenAppend);
    f.Write("  My name is George.");
    f.Close();

    f2.Open(File::OpenRead);
    data = f2.Read();
    f2.Close();
    QVERIFY(data == "Hello World!  My name is George.");

    // Test the delete function
    f.Delete();
    QVERIFY(!f.IsOpen());
    QVERIFY(!f.Exists());
}

void FileTest::test_errors()
{
    File f(TEST_FILENAME);
    if(f.Exists())
        f.Delete();

    bool ex_hit = false;
    try
    {
        // Can't open a non-existent file read-only
        f.Open(File::OpenRead);
    }
    catch(const GUtil::Exception<> &)
    {
        ex_hit = true;
    }
    QVERIFY(ex_hit);


    // Prepare some initial file data
    f.Open(File::OpenWrite);
    f.Write("Test Data");
    f.Close();


    ex_hit = false;
    f.Open(File::OpenWrite);
    try
    {
        // Can't read a file while we're in write mode
        f.Read();
    }
    catch(const GUtil::Exception<> &)
    {
        ex_hit = true;
    }
    f.Close();
    QVERIFY(ex_hit);


    ex_hit = false;
    f.Open(File::OpenWrite);
    try
    {
        // Can't delete a file that is open elsewhere
        File::Delete(TEST_FILENAME);
    }
    catch(const GUtil::Exception<> &)
    {
        ex_hit = true;
    }
    f.Close();
    QVERIFY(ex_hit);
}

QTEST_APPLESS_MAIN(FileTest);

#include "tst_filetest.moc"
