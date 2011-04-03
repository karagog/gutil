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
        params.FilterValues()["one"] = "1";

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
        values.Values()["two"] = 3;

        // The same selection parameters should select out the first row for update
        dbio.Update(params, values);

        tbl = dbio.Select(params);
        QVERIFY(tbl.RowCount() == 1);
        QVERIFY2(tbl[0][0] == 1, tbl[0][0].toString().toStdString().c_str());
        QVERIFY2(tbl[0][1] == 3, tbl[0][1].toString().toStdString().c_str());


        // Try something illegal, like violating the primary key
        bool exception_hit(false);
        try
        {
            dbio.Insert(tbl);
        }
        catch(Core::Exception &ex)
        {
//            qDebug(ex.GetMessage().c_str());
//            qDebug(ex.GetData("error").c_str());
            exception_hit = true;
        }
        QVERIFY(exception_hit);


        // How many records are over 100?
        DatabaseSelectionParameters new_params(
                dbio.GetBlankSelectionParameters("test"));
        new_params.FilterValues()["two"] = 100;
        new_params.ColumnOptions()[1] |= GDatabaseIODevice::CompareGreaterThan;
        QVERIFY(dbio.Count(new_params) == 10);

        // How many are not greater than 100 ?
        new_params.ColumnOptions()[1] |= GDatabaseIODevice::Not;
        QVERIFY(dbio.Count(new_params) == 1);


        // Now delete the first row
        dbio.Delete(params);
        tbl = dbio.Select("test");
        QVERIFY(tbl.RowCount() == 10);

        // Make sure it doesn't exist anymore
        QVERIFY(tbl.FindRows(0, 1).Count() == 0);
        QVERIFY(tbl.FindRows(0, 2).Count() == 1);  // Dummy check


        // Test blob data

        // Create a simple blob table
        Custom::GPairList<QString, QString> pl;
        pl<< QPair<QString, QString>("o", "BLOB");
        dbio.CreateTable("blob", pl);

        tbl = dbio.GetBlankTable("blob");
        values = dbio.GetBlankValueParameters("blob");
        //values.ColumnOptions()[0] |= dbio.Binary;

        tbl.AddNewRow(DataObjects::GVariantCollection() << "Hello");

        dbio.Insert(tbl);

        tbl = dbio.Select("blob");
        QVERIFY(tbl.RowCount() == 1);
        QVERIFY(tbl[0][0] == "Hello");

        // Now try actual binary data
        QString tmps;
        tmps.append(QChar((char)0x02));
        tmps.append(QChar((char)0x01));
        tmps.append(QChar((char)0x00));
        tmps.append(QChar((char)0x01));
        QVERIFY(tmps.length() == 4);

        tbl = dbio.GetBlankTable("blob");
        tbl.AddNewRow(Custom::GVariantList() << tmps);
        dbio.Insert(tbl);
        tbl = dbio.Select("blob");

        QVERIFY(tbl.RowCount() == 2);
        QVERIFY(tbl[0][0] == "Hello");
        QVERIFY(tbl[1][0] == tmps);
        QVERIFY(tbl[1][0].toString().length() == tmps.length());
    }
    catch(Core::Exception &ex)
    {
        dLogException(ex);
        QFAIL("Exception caught");
    }
}

QTEST_MAIN(TransportsTest);

#include "tst_transportstest.moc"
