#include "file_manager_test.h"
#include <QTest>

file_manager_test::file_manager_test(QObject *parent) :
    QObject(parent)
{
}

void file_manager_test::simple_startup_test()
{
    QVERIFY(true);
}
