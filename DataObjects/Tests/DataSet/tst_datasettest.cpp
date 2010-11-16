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

#include "DataObjects/dataset.h"
#include "Logging/debuglogger.h"
#include <QtCore/QString>
#include <QtTest/QtTest>
using namespace GUtil::DataObjects;

class DataSetTest : public QObject
{
    Q_OBJECT

public:
    DataSetTest();

private Q_SLOTS:

    // test rows
    void test_dataRows();

    // Test the row error cases
    void test_row_errors();

    // test the data table
    void test_dataTable();

    void test_dataSet();

};

DataSetTest::DataSetTest()
{
}

void DataSetTest::test_dataRows()
{
    try
    {
        DataTable t(1);
        DataRow r1 = t.CreateRow();
        DataRow r2(r1);

        QVERIFY(r1 == r2);

        // Both data rows should be looking at the same data source
        r1[0] = "Value";
        QVERIFY(r1[0] == r2[0]);
        QVERIFY(r1[0] == "Value");
        QVERIFY(r2[0] == "Value");
        QVERIFY(r1["0"] == "Value");

        DataRow r3 = r2.Clone();

        // They're not equal, because after cloning the row identifier is unique
        QVERIFY(r3 != r2);

        // But their data should be the same if we compare it
        QVERIFY(r3.Equals(r2));

        // A change to either of the first two doesn't affect the cloned row
        r2[0] = "1";
        QVERIFY(r2[0] == "1");
        QVERIFY(r1[0] == "1");
        QVERIFY(r3[0] != "1");

        // Make sure the equal operator works the other way around too
        QVERIFY(!r3.Equals(r2));
    }
    catch(Core::Exception &ex)
    {
        dLogException(ex);
        QVERIFY(false);
    }
}

void DataSetTest::test_row_errors()
{
    try
    {
        DataTable t;

        QVERIFY(t.ColumnCount() == 0);

        // If you initialize a row with more data than it has columns, then the extra
        //  data gets thrown out

        DataRow r = t.CreateRow(QVariantList() << "oops!");
        QVERIFY(r.ColumnCount() == 0);

        // But you should be able to initialize it with less data than it has columns
        t.AddColumn("one");
        t.AddColumn("two");

        QVERIFY(t.ColumnCount() == 2);
        r = t.AddNewRow(QVariantList() << "Yay!");

        QVERIFY(r.ColumnCount() == 2);
        QVERIFY(t[0]["one"] == "Yay!");
        QVERIFY(t[0]["two"] == QVariant());
        QVERIFY(r[0] == "Yay!");
        QVERIFY(r[1] == QVariant());


        // Now test what happens when moving rows between data tables
        DataTable t2(2);
        bool exception_hit = false;
        try
        {
            // We shouldn't be able to add a foreign row to the table
            t2.AddRow(r);
        }
        catch(Core::ValidationException &)
        {
            exception_hit = true;
        }

        QVERIFY(exception_hit);

        // But we can 'import' it as a clone
        DataRow r2 = t2.ImportRow(r);
        QVERIFY(r2 != r);
        QVERIFY(r2.Equals(r));


        // We shouldn't be able to add the row twice:
        exception_hit = false;
        try
        {
            t.Rows().Add(r);
        }
        catch(Core::ValidationException &)
        {
            exception_hit = true;
        }
        QVERIFY(exception_hit);
    }
    catch(Core::Exception &ex)
    {
        dLogException(ex);
        QVERIFY(false);
    }
}

void DataSetTest::test_dataTable()
{
    try
    {
        DataTable dt("test_table", 3);
        DataRow dr = dt.CreateRow();

        // The row doesn't have an index yet
        QVERIFY(dr.Index() == -1);

        // Make sure we have the right number of columns
        QVERIFY(dt.ColumnCount() == 3);
        QVERIFY(dr.ColumnCount() == 3);


        // The table doesn't actually have this row in it yet, until you call 'add'
        QVERIFY(dt.Rows().Count() == 0);
        dt.AddRow(dr);
        QVERIFY(dt.Rows().Count() == 1);
        QVERIFY(dr.ColumnCount() == 3);
        QVERIFY2(dr.Index() == 0, QString("%1").arg(dr.Index()).toStdString().c_str());

        // A cloned row has no index, even though the original did
        DataRow new_dr = dr.Clone();
        QVERIFY(new_dr.Index() == -1);
        QVERIFY(dr.Index() == 0);


        // Set some values on the row and make sure they persist
        dt.Rows()[0][0] = "HI";
        dt.Rows()[0][1] = "2";
        dt.Rows()[0][2] = "3";

        QVERIFY(dt.Rows()[0][0].toString() == "HI");
        QVERIFY(dt.Rows()[0][1].toString() == "2");
        QVERIFY(dt.Rows()[0][2].toString() == "3");


        // Resize the columns and check the rows
        dt.SetColumnHeaders(QStringList("OneColumn"));
        QVERIFY(dt.ColumnCount() == 1);
        QVERIFY(dr.ColumnCount() == 1);
        QVERIFY(dt.ColumnKeys()[0] == "OneColumn");

        // The column resize shouldn't affect the data in the first column
        QVERIFY(dr[0] == "HI");

        dt.AddColumn("second", "two");
        QVERIFY(dt.ColumnCount() == 2);
        QVERIFY(dr.ColumnCount() == 2);

        // Make sure the new column is null at first
        QVERIFY(dr["second"] == QVariant());

        dr["second"] = "v";
        QVERIFY(dr["second"] == "v");
        QVERIFY(dr[1] == "v");
        QVERIFY(dt.Rows()[0]["second"] == "v");

        DataRow dr2 = dt.AddNewRow();
        QVERIFY(dr2.ColumnCount() == 2);
        QVERIFY(dt.Rows().Count() == 2);
        QVERIFY(dr2[0] == QVariant());
        QVERIFY(dr2[1] == QVariant());

        dr2[0] = "Hello World!";
        QVERIFY(dr2[0] == "Hello World!");

        // Test the xml import/export
        QString xml = dt.ToXmlQString(true);
        //qDebug(xml.toStdString().c_str());

        DataTable dt2;
        dt2.FromXmlQString(xml);
        QVERIFY(dt2.Name() == "test_table");
        QVERIFY(dt2.Rows().Count() == 2);
        QVERIFY(dt2.ColumnCount() == 2);

        QVERIFY2(dt2.ColumnKeys()[0] == "OneColumn", dt2.ColumnKeys()[0].toStdString().c_str());
        QVERIFY(dt2.ColumnKeys()[1] == "second");
        QVERIFY(dt2.ColumnLabels()[0] == "");
        QVERIFY2(dt2.ColumnLabels()[1] == "two", dt2.ColumnLabels()[1].toStdString().c_str());

        QVERIFY(dt2[0][0] == "HI");
        QVERIFY(dt2[0]["second"] == "v");
        QVERIFY(dt2[1][0] == "Hello World!");
        QVERIFY(dt2[1]["second"] == QVariant());



        // Test cloning
        DataTable dt3 = dt2.Clone();
        QVERIFY(dt3 != dt2);
        QVERIFY(dt3.Equals(dt2));

        // Change in one doesn't affect the other
        dt3[0][0] = "new value";
        QVERIFY(!dt3.Equals(dt2));
    }
    catch(Core::Exception &ex)
    {
        dLogException(ex);
        QVERIFY(false);
    }
}


void DataSetTest::test_dataSet()
{
    try
    {
        DataSet ds;
        DataTable dt1(1);
        DataTable dt2(2);

        ds.Tables().Add(dt1);
        ds.Tables().Add(dt2);

        QVERIFY(ds.TableCount() == 2);

        bool exception_hit = false;
        try
        {
            // Shouldn't be able to add the same table twice
            ds.Tables().Add(dt1);
        }
        catch(Core::ValidationException &)
        {
            exception_hit = true;
        }
        QVERIFY(exception_hit);


        // Add a row and access it through the dataset object
        dt1.AddNewRow(QVariantList() << "one");
        QVERIFY(ds[0][0][0] == "one");

        dt2.AddNewRow(QVariantList() << "1" << "2");
        dt2.AddNewRow(QVariantList() << "3" << "4");
        QVERIFY(ds[1][0][0] == "1");
        QVERIFY(ds[1][0][1] == "2");
        QVERIFY(ds[1][1][0] == "3");
        QVERIFY(ds[1][1][1] == "4");


        // Export it to Xml and import it again
        QString ds_xml = ds.ToXmlQString(true);
        DataSet ds2;
        //qDebug(ds_xml.toStdString().c_str());
        QVERIFY(!ds2.Equals(ds));

        ds2.FromXmlQString(ds_xml);
        //qDebug(ds2.ToXmlQString(true).toStdString().c_str());
        QVERIFY(ds2.TableCount() == 2);
        QVERIFY(ds2 != ds);
        QVERIFY(ds2.Equals(ds));
    }
    catch(Core::Exception &ex)
    {
        dLogException(ex);
        QVERIFY(false);
    }
}





QTEST_APPLESS_MAIN(DataSetTest);

#include "tst_datasettest.moc"
