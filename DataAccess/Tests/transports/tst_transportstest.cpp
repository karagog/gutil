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
    try
    {
        QString connection_name("MyConnection");
        QSqlDatabase::addDatabase("QSQLITE", connection_name)
                .setDatabaseName("temp.sqlite");

        GDatabaseIODevice dbio(connection_name, this);

        // Create a table with two columns, the first an integer, the second
        //  is text, and make the first column a primary key
        dbio.CreateTable("test",
                         Custom::GPairList<QString, QString>() <<
                         QPair<QString, QString>("one", "INTEGER") <<
                         QPair<QString, QString>("two", "TEXT"),
                         0,
                         true);

        // Insert a new row
        DataObjects::DataTable tbl(dbio.GetBlankTable("test"));

        // The new row is blank for the first column, because it will be assigned an auto-incrementing id
        tbl.AddNewRow(Custom::GVariantList() << Custom::GVariant() << "2");

        // To start we shouldn't have any data in the table
        QVERIFY(dbio.Count("test") == 0);
        QVERIFY(dbio.LastInsertId() == 0);

        dbio.Insert(tbl);

        // Verify that the auto-incrementing key is working
        QVERIFY2(dbio.LastInsertId() == 1,
                 QString("%1").arg(dbio.LastInsertId()).toStdString().c_str());

        // Now we have one row in the database
        QVERIFY(dbio.Count("test") == 1);


        // Select the row we just inserted
        DatabaseSelectionParameters params(dbio.GetBlankSelectionParameters("test"));
        params["one"] = "1";

        tbl = dbio.Select(params);

        QVERIFY2(tbl[0][0] == 1, tbl[0][0].toString().toStdString().c_str());
        QVERIFY2(tbl[0][1] == 2, tbl[0][1].toString().toStdString().c_str());


        // Insert a few more rows
        tbl = dbio.GetBlankTable("test");
        for(int i = 0; i < 10; i++)
            tbl.AddNewRow(DataObjects::GVariantCollection() <<
                          Custom::GVariant() << (i + 100));
        dbio.Insert(tbl);

        int cnt = dbio.Count("test");
        QVERIFY2(cnt == 11, QString("%1").arg(cnt).toStdString().c_str());


        // Update the second column to a 3
        DatabaseValueParameters values(dbio.GetBlankValueParameters("test"));
        values["two"] = 3;

        // The same selection parameters should select out the first row
        dbio.Update(params, values);
        tbl = dbio.Select(params);
        QVERIFY2(tbl[0][0] == 1, tbl[0][0].toString().toStdString().c_str());
        QVERIFY2(tbl[0][1] == 3, tbl[0][1].toString().toStdString().c_str());
    }
    catch(Core::Exception &ex)
    {
        dLogException(ex);
        QFAIL("Exception caught");
    }
}

QTEST_MAIN(TransportsTest);

#include "tst_transportstest.moc"
