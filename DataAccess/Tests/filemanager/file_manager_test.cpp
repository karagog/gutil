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

#include "DataAccess/BinaryDataStore.h"
#include "Core/exception.h"
#include <QTest>
using namespace GUtil;


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
    void test_reset();
    void test_second_object();
    void test_large_files();
    void test_idList();
    void test_file_queuing();
    void test_hasFile();
    void test_remove();

private:
    DataAccess::BinaryDataStore *fm;

};


file_manager_test::file_manager_test()// :
    //QObject(parent)
{
    fm = new DataAccess::BinaryDataStore("filemanagertest");
}

void file_manager_test::simple_startup_test()
{
    QString teststr = "Hello World!";
    int id = fm->addFile(teststr);
    QVERIFY(teststr == fm->getFile(id));

    QString teststr2 = "Next data";
    int id2 = fm->addFile(teststr2);
    QVERIFY(id != id2);
    QVERIFY(teststr2 == fm->getFile(id2));
}

void file_manager_test::test_binary_dat()
{
    QString teststr;
    teststr.append('a');
    teststr.append(0x00);
    teststr.append('a');

    int id = fm->addFile(teststr);
    QVERIFY(teststr == fm->getFile(id));
}

void file_manager_test::test_reset()
{
    fm->reset();

    bool exception_hit = false;
    try
    {
        fm->getFile(0);
    }
    catch(Core::Exception)
    {
        exception_hit = true;
    }
    QVERIFY(exception_hit);
}

void file_manager_test::test_second_object()
{
    fm->reset();
    DataAccess::BinaryDataStore *fm2 = new DataAccess::BinaryDataStore("filemanagertest", false);
    QVERIFY(fm->addFile("test1") == 0);
    QVERIFY(fm2->addFile("test2") == 1);
    QVERIFY(fm->addFile("test3") == 2);
    delete fm2;
}

void file_manager_test::test_large_files()
{
    QString dat(10000000, 'a');
    int id = fm->addFile(dat);
    QVERIFY(dat == fm->getFile(id));
}

void file_manager_test::test_idList()
{
    fm->reset();

    QList<int> l;
    l.append(fm->addFile("HI"));
    l.append(fm->addFile(""));
    l.append(fm->addFile("hi"));

    QList<int> l2 = fm->idList();

    QVERIFY(l2.count() == l.count());
    for(int i = l2.count() - 1; i >= 0; i--)
        QVERIFY(l2[i] == l[i]);
}

void file_manager_test::test_file_queuing()
{
    fm->reset();
    fm->addFile("file1");
    fm->addFile("file2");

    fm->addFile("file3");
    fm->addFile("file4");
    fm->addFile("file5");

    QVERIFY(fm->getFile(0) == "file1");
    QVERIFY(fm->getFile(1) == "file2");
    QVERIFY(fm->getFile(2) == "file3");
    QVERIFY(fm->getFile(3) == "file4");
    QVERIFY(fm->getFile(4) == "file5");

    QList<int> idList = fm->idList();
    QVERIFY(idList.count() == 5);
    QVERIFY(idList[0] == 0);
    QVERIFY(idList[1] == 1);
    QVERIFY(idList[2] == 2);
    QVERIFY(idList[3] == 3);
    QVERIFY(idList[4] == 4);
}

void file_manager_test::test_hasFile()
{
    fm->reset();
    fm->addFile("HI");
    fm->addFile("HI");
    QVERIFY(fm->hasFile(0));
    QVERIFY(fm->hasFile(1));
    QVERIFY(!fm->hasFile(2));
}

void file_manager_test::test_remove()
{
    fm->reset();
    QVERIFY(!fm->hasFile(0));

    fm->addFile("HI");
    fm->addFile("HI");
    QVERIFY(fm->hasFile(0));

    fm->removeFile(0);
    QVERIFY(!fm->hasFile(0));

    QVERIFY(fm->hasFile(1));
    QVERIFY(fm->getFile(1) == "HI");

    fm->removeFile(1);
    QVERIFY(!fm->hasFile(1));
}

QTEST_APPLESS_MAIN(file_manager_test);

#include "file_manager_test.moc"
