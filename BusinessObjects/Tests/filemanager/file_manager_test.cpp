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

#include "BusinessObjects/BinaryDataStore.h"
#include "Core/exception.h"
#include "Logging/debuglogger.h"
#include <QTest>
using namespace GUtil;
GUTIL_USING_NAMESPACE( BusinessObjects );


class file_manager_test : public QObject
{
    Q_OBJECT
public:
    explicit file_manager_test();
    virtual ~file_manager_test(){}

signals:

//public slots:

private Q_SLOTS:
    void simple_startup_test();
    void test_binary_dat();
    void test_Reset();
    void test_second_object();
    //void test_large_files();
    void test_GetIds();
    void test_file_queuing();
    void test_HasFile();
    void test_remove();

private:
    BinaryDataStore *fm;

};


file_manager_test::file_manager_test()// :
    //QObject(parent)
{
    fm = new BinaryDataStore("filemanagertest");
    fm->SetAsyncWrite(false);
}

void file_manager_test::simple_startup_test()
{
    qDebug() << fm->FileName();

    QString probe;
    try
    {
        QString teststr = "Hello World!";
        int id = fm->AddFile(teststr);
        probe = fm->GetFile(id);
        QVERIFY(teststr == probe);

        QString teststr2 = "Next data";
        int id2 = fm->AddFile(teststr2);
        QVERIFY(id != id2);
        probe = fm->GetFile(id2);
        QVERIFY(teststr2 == probe);
    }
    catch(Core::Exception &ex)
    {
        dLogException(ex);
        QFAIL("Exception");
    }
}

void file_manager_test::test_binary_dat()
{
    try
    {
        QString teststr;
        teststr.append('a');
        teststr.append(0x00);
        teststr.append('a');

        int id = fm->AddFile(teststr);
        QVERIFY(teststr == fm->GetFile(id));
    }
    catch(Core::Exception &ex)
    {
        dLogException(ex);
        QFAIL("Exception");
    }
}

void file_manager_test::test_Reset()
{
    try
    {
        QVERIFY(fm->HasFile(1));

        fm->Reset();

        bool exception_hit = false;
        try
        {
            fm->GetFile(1);
        }
        catch(Core::Exception)
        {
            exception_hit = true;
        }
        QVERIFY(exception_hit);
    }
    catch(Core::Exception &ex)
    {
        dLogException(ex);
        QFAIL("Exception");
    }
}

void file_manager_test::test_second_object()
{
    try
    {
        fm->Reset();
        BinaryDataStore *fm2 = new BinaryDataStore("filemanagertest");
        QVERIFY(fm->AddFile("test1") == 1);
        QVERIFY(fm2->AddFile("test2") == 2);
        QVERIFY(fm->AddFile("test3") == 3);
        delete fm2;
    }
    catch(Core::Exception &ex)
    {
        dLogException(ex);
        QFAIL("Exception");
    }
}

//void file_manager_test::test_large_files()
//{
//    try
//    {
//        QString dat(10000000, 'a');
//        int id = fm->AddFile(dat);
//        QVERIFY(dat == fm->GetFile(id));
//    }
//    catch(Core::Exception &ex)
//    {
//        dLogException(ex);
//        QFAIL("Exception");
//    }
//}

void file_manager_test::test_GetIds()
{
    try
    {
        fm->Reset();

        QList<int> l;
        l.append(fm->AddFile("HI"));
        l.append(fm->AddFile(""));
        l.append(fm->AddFile("hi"));

        QSet<int> l2 = fm->GetIds();

        QVERIFY(l2.count() == l.count());
        for(int i = l2.count() - 1; i >= 0; i--)
            QVERIFY(l2.contains(l[i]));
    }
    catch(Core::Exception &ex)
    {
        dLogException(ex);
        QFAIL("Exception");
    }
}

void file_manager_test::test_file_queuing()
{
    try
    {
        fm->Reset();
        fm->AddFile("file1");
        fm->AddFile("file2");

        fm->AddFile("file3");
        fm->AddFile("file4");
        fm->AddFile("file5");

        QVERIFY(fm->GetFile(1) == "file1");
        QVERIFY(fm->GetFile(2) == "file2");
        QVERIFY(fm->GetFile(3) == "file3");
        QVERIFY(fm->GetFile(4) == "file4");
        QVERIFY(fm->GetFile(5) == "file5");

        QSet<int> idList = fm->GetIds();
        QVERIFY(idList.count() == 5);
        QVERIFY(idList.contains(1));
        QVERIFY(idList.contains(2));
        QVERIFY(idList.contains(3));
        QVERIFY(idList.contains(4));
        QVERIFY(idList.contains(5));
    }
    catch(Core::Exception &ex)
    {
        dLogException(ex);
        QFAIL("Exception");
    }
}

void file_manager_test::test_HasFile()
{
    try
    {
        fm->Reset();
        fm->AddFile("HI");
        fm->AddFile("HI");
        QVERIFY(fm->HasFile(1));
        QVERIFY(fm->HasFile(2));
        QVERIFY(!fm->HasFile(3));
    }
    catch(Core::Exception &ex)
    {
        dLogException(ex);
        QFAIL("Exception");
    }
}

void file_manager_test::test_remove()
{
    try
    {
        fm->Reset();
        QVERIFY(!fm->HasFile(1));

        fm->AddFile("HI");
        fm->AddFile("HI2");
        QVERIFY(fm->HasFile(1));

        fm->RemoveFile(1);
        QVERIFY(!fm->HasFile(1));

        QVERIFY(fm->HasFile(2));
        QVERIFY(fm->GetFile(2) == "HI2");

        fm->RemoveFile(2);
        QVERIFY(!fm->HasFile(2));
    }
    catch(Core::Exception &ex)
    {
        dLogException(ex);
        QFAIL("Exception");
    }
}

QTEST_APPLESS_MAIN(file_manager_test);

#include "file_manager_test.moc"
