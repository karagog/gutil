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

#include "gutil_binarydatastore.h"
#include "gutil_exception.h"
#include "gutil_consolelogger.h"
#include <QTest>
USING_NAMESPACE_GUTIL;
USING_NAMESPACE_GUTIL1(QT);


class file_manager_test : public QObject
{
    Q_OBJECT
public:
    explicit file_manager_test();
    virtual ~file_manager_test(){
        QString filename( fm.GetFileName() );
        fm.Uninitialize();
        QFile::remove(filename);
    }

private Q_SLOTS:
    void simple_startup_test();
    void test_binary_dat();
    void test_second_object();
    //void test_large_files();
    void test_GetIds();
    void test_file_queuing();
    void test_HasFile();
    void test_remove();

private:
    BinaryDataStore fm;

};


#define BINARYDATASTORE_TEST_FILENAME   "test_binarydatastore.sqlite"

file_manager_test::file_manager_test()// :
    //QObject(parent)
{
    fm.Initialize(BINARYDATASTORE_TEST_FILENAME);
}

void file_manager_test::simple_startup_test()
{
    qDebug(fm.GetFileName().toStdString().c_str());

    QString probe;
    try
    {
        QString teststr = "Hello World!";
        int id = fm.AddData(teststr.toAscii());
        probe = fm.GetData(id);
        QVERIFY(teststr == probe);

        QString teststr2 = "Next data";
        int id2 = fm.AddData(teststr2.toAscii());
        QVERIFY(id != id2);
        probe = fm.GetData(id2);
        QVERIFY(teststr2 == probe);
    }
    catch(const Exception<> &ex)
    {
        GUtil::ConsoleLogger().LogException(ex);
        QFAIL("Exception");
    }
}

void file_manager_test::test_binary_dat()
{
    try
    {
        QByteArray teststr;
        teststr.append('a');
        teststr.append(QChar(0x00));
        teststr.append('a');

        int id = fm.AddData(teststr);
        QVERIFY(teststr == fm.GetData(id));
    }
    catch(const Exception<> &ex)
    {
        ConsoleLogger().LogException(ex);
        QFAIL("Exception");
    }
}

void file_manager_test::test_second_object()
{
    try
    {
        fm.Clear();
        BinaryDataStore fm2;
        fm2.Initialize(BINARYDATASTORE_TEST_FILENAME);

        QVERIFY(fm.AddData("test1") == 1);
        QVERIFY(fm2.AddData("test2") == 2);
        QVERIFY(fm.AddData("test3") == 3);
    }
    catch(const Exception<> &ex)
    {
        ConsoleLogger().LogException(ex);
        QFAIL("Exception");
    }
}

//void file_manager_test::test_large_files()
//{
//    try
//    {
//        QString dat(10000000, 'a');
//        int id = fm.AddData(dat);
//        QVERIFY(dat == fm.GetData(id));
//    }
//    catch(const Exception<> &ex)
//    {
//        ConsoleLogger().LogException(ex);
//        QFAIL("Exception");
//    }
//}

void file_manager_test::test_GetIds()
{
    try
    {
        fm.Clear();

        Vector<int> l;
        l.PushBack(fm.AddData("HI"));
        l.PushBack(fm.AddData(""));
        l.PushBack(fm.AddData("hi"));

        Vector<int> l2 = fm.GetIds();

        QVERIFY(l2.Count() == l.Count());
        for(int i = l2.Count() - 1; i >= 0; i--)
            QVERIFY(l2.Contains(l[i]));
    }
    catch(const Exception<> &ex)
    {
        ConsoleLogger().LogException(ex);
        QFAIL("Exception");
    }
}

void file_manager_test::test_file_queuing()
{
    try
    {
        fm.Clear();
        fm.AddData("file1");
        fm.AddData("file2");

        fm.AddData("file3");
        fm.AddData("file4");
        fm.AddData("file5");

        QVERIFY(fm.GetData(1) == "file1");
        QVERIFY(fm.GetData(2) == "file2");
        QVERIFY(fm.GetData(3) == "file3");
        QVERIFY(fm.GetData(4) == "file4");
        QVERIFY(fm.GetData(5) == "file5");

        Vector<int> idList = fm.GetIds();
        QVERIFY(idList.Count() == 5);
        QVERIFY(idList.Contains(1));
        QVERIFY(idList.Contains(2));
        QVERIFY(idList.Contains(3));
        QVERIFY(idList.Contains(4));
        QVERIFY(idList.Contains(5));
    }
    catch(const Exception<> &ex)
    {
        ConsoleLogger().LogException(ex);
        QFAIL("Exception");
    }
}

void file_manager_test::test_HasFile()
{
    try
    {
        fm.Clear();
        fm.AddData("HI");
        fm.AddData("HI");
        QVERIFY(fm.HasData(1));
        QVERIFY(fm.HasData(2));
        QVERIFY(!fm.HasData(3));
    }
    catch(const Exception<> &ex)
    {
        ConsoleLogger().LogException(ex);
        QFAIL("Exception");
    }
}

void file_manager_test::test_remove()
{
    try
    {
        fm.Clear();
        QVERIFY(!fm.HasData(1));

        fm.AddData("HI");
        fm.AddData("HI2");
        QVERIFY(fm.HasData(1));

        fm.RemoveData(1);
        QVERIFY(!fm.HasData(1));

        QVERIFY(fm.HasData(2));
        QVERIFY(fm.GetData(2) == "HI2");

        fm.RemoveData(2);
        QVERIFY(!fm.HasData(2));
    }
    catch(const Exception<> &ex)
    {
        ConsoleLogger().LogException(ex);
        QFAIL("Exception");
    }
}

QTEST_APPLESS_MAIN(file_manager_test);

#include "file_manager_test.moc"
