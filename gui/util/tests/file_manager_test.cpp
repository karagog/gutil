#include "file_manager_test.h"
#include "file_manager.h"
#include "exception.h"
#include <QTest>

file_manager_test::file_manager_test(QObject *parent) :
    QObject(parent)
{
    fm = new File_Manager("filemanagertest");
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
    catch(GUtil::Exception)
    {
        exception_hit = true;
    }
    QVERIFY(exception_hit);
}
