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

#include "DataAccess/gconsoleiodevice.h"
#include "DataAccess/gfileiodevice.h"
#include "DataAccess/gdatabaseiodevice.h"
#include "DataObjects/DataSet/datatable.h"
#include "Logging/filelogger.h"
#include "Logging/globallogger.h"
#include "Logging/debuglogger.h"
#include "Core/exception.h"
#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QtCore/QCoreApplication>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlQuery>
#include <QSqlError>
using namespace GUtil::DataAccess;
using namespace GUtil::Logging;
using namespace GUtil;

class TransportsTest : public QObject
{
    Q_OBJECT

public:
    TransportsTest();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    //void test_console_transport();
    void test_file_transport();
    void test_database_transport();
};

TransportsTest::TransportsTest()
{
    GlobalLogger::SetupDefaultLogger(new FileLogger("transports.log"));
}

void TransportsTest::initTestCase()
{
}

void TransportsTest::cleanupTestCase()
{
}

//void TransportsTest::test_console_transport()
//{
//    try
//    {
//        GConsoleIODevice ct;
//        std::string inval;

//        ct<<"Hello World!\n\nYou must type an h: ";
//        ct>>inval;

//        QVERIFY2(inval == "h", inval.c_str());

//        ct<<"Now type a k: ";
//        ct>>inval;
//        QVERIFY2(inval == "k", inval.c_str());
//    }
//    catch(Core::Exception &ex)
//    {
//        GlobalLogger::LogException(ex);
//        QVERIFY2(false, "Check log for details");
//    }
//}

void TransportsTest::test_file_transport()
{
    try
    {
        GFileIODevice ft("Testfile.txt");

        ft.SetWriteMode(GFileIODevice::WriteOver);
        ft.Write("Bogus data");
        QVERIFY2(ft.FileData() == "Bogus data", QString(ft.FileData()).toStdString().c_str());

        ft.Write("Hello World!");
        QVERIFY2(ft.FileData() == "Hello World!", QString(ft.FileData()).toStdString().c_str());

        ft.SetWriteMode(GFileIODevice::WriteAppend);
        ft.Write("7");
        QVERIFY(ft.FileData() == "Hello World!7");

        ft.Write("\nNew line");

        GFileIODevice ft2("Testfile.txt");
        QVERIFY2(ft.FileData() == ft2.FileData(), QString("(%1, %2)")
                 .arg(QString(ft.FileData()))
                 .arg(QString(ft2.FileData())).toStdString().c_str()
                 );
    }
    catch(Core::Exception &ex)
    {
        GlobalLogger::LogException(ex);
        throw;
    }
}

void TransportsTest::test_database_transport()
{
    QSqlDatabase db(QSqlDatabase::addDatabase("QSQLITE"));
    db.setDatabaseName("temp.sqlite");

    // Prepare the database
    db.open();
    {
        QSqlQuery q(db);
        q.prepare("DROP TABLE IF EXISTS test");
        bool res = q.exec();

        res = q.exec("CREATE TABLE test (one, two)");
        QVERIFY2(res, q.lastError().text().toStdString().c_str());
    }
    db.close();


    GDatabaseIODevice d(db, this);
    d.SetWriteCommand(GDatabaseIODevice::Insert);

    DataObjects::DataTable tbl("test");
    tbl.SetColumnHeaders(QStringList() << "one" << "two");

    // Insert the row
    tbl.AddNewRow(Custom::GVariantList() << "1" << 2);
    try
    {
        d.SendData(tbl);
    }
    catch(Core::Exception &ex)
    {
        dLogException(ex);
        QFAIL(ex.GetMessage().c_str());
    }


    // Select the row we just inserted

}

QTEST_MAIN(TransportsTest);

#include "tst_transportstest.moc"
