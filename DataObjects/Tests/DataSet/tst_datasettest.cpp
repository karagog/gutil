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

#include "persondatarow.h"
#include "peopletable.h"
#include "DataObjects/dataset.h"
#include "Logging/debuglogger.h"
#include <QtCore/QString>
#include <QtTest/QtTest>

GUTIL_USING_NAMESPACE( DataObjects );
GUTIL_USING_NAMESPACE( Core );
GUTIL_USING_NAMESPACE( Custom );

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

    void test_table_errors();

    void test_dataSet();

    void test_derived_classes();

};

DataSetTest::DataSetTest()
{
}

void DataSetTest::test_dataRows()
{
    try
    {
        DataTable t(1);
        t.AddNewRow();
        DataRow r1 = t[0];
        DataRow r2(r1);

        QVERIFY(r1 == r2);

        // Both data rows should be looking at the same data source
        r1[0] = "Value";
        QVERIFY(r1[0].Equals(r2[0]));
        QVERIFY(r1[0].Equals("Value"));
        QVERIFY(r2[0].Equals("Value"));
        QVERIFY(r1["0"].Equals("Value"));

        DataRow r3(r2);
        r2.CloneTo(r3);

        // They're not equal, because after cloning the row identifier is unique
        QVERIFY(r3 != r2);

        // But their data should be the same if we compare it
        QVERIFY(r3.Equals(r2));

        // A change to either of the first two doesn't affect the cloned row
        r2[0] = "1";
        QVERIFY(r2[0].Equals("1"));
        QVERIFY(r1[0].Equals("1"));
        QVERIFY(!r3[0].Equals("1"));

        // Make sure the equal operator works the other way around too
        QVERIFY(!r3.Equals(r2));


        // Test removing rows:
        t.AddRow(r3);
        QVERIFY(t.RowCount() == 2);

        // Test that removing a row actually deletes the pointer held in the table:

        // You can set a break point in the row collection 'on_remove' method; it should
        //   get hit here:
        t.RemoveRow(1);
        QVERIFY(t.RowCount() == 1);

        t.RemoveRow(r2);
        QVERIFY(t.RowCount() == 0);
    }
    catch(Exception &ex)
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

        t.AddNewRow( GVariantList() << "oops!");
        DataRow r = t[0];
        QVERIFY(r.ColumnCount() == 0);

        // But you should be able to initialize it with less data than it has columns
        t.AddColumn("one");
        t.AddColumn("two");

        QVERIFY(t.ColumnCount() == 2);
        t.AddNewRow( GVariantList() << "Yay!");
        r = t[1];

        QVERIFY(r.ColumnCount() == 2);
        QVERIFY(t[1]["one"].Equals("Yay!"));
        QVERIFY(t[1]["two"].Equals(QVariant()));
        QVERIFY(r[0].Equals("Yay!"));
        QVERIFY(r[1].Equals(QVariant()));


        // Now test what happens when moving rows between data tables
        DataTable t2(2);
        bool exception_hit = false;
        try
        {
            // We shouldn't be able to add a foreign row to the table
            t2.AddRow(r);
        }
        catch(ValidationException &)
        {
            exception_hit = true;
        }

        QVERIFY(exception_hit);

        // But we can 'import' it as a clone
        t2.ImportRow(r);
        DataRow r2 = t2[t2.RowCount() - 1];
        QVERIFY(r2 != r);
        QVERIFY(r2.Equals(r));


        // We shouldn't be able to add the row twice:
        exception_hit = false;
        try
        {
            t.Rows().Add(r);
        }
        catch(ValidationException &)
        {
            exception_hit = true;
        }
        QVERIFY(exception_hit);
    }
    catch(Exception &ex)
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
        dt.AddNewRow();
        DataRow dr = dt[0];

        QVERIFY(dr.Index() == 0);

        // Make sure we have the right number of columns and rows
        QVERIFY(dt.ColumnCount() == 3);
        QVERIFY(dr.ColumnCount() == 3);
        QVERIFY(dt.Rows().Count() == 1);

        QVERIFY2(dr.Index() == 0, QString("%1").arg(dr.Index()).toStdString().c_str());

        // A cloned row has no index, even though the original did
        DataRow new_dr(dr);
        dr.CloneTo(new_dr);
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
        QVERIFY(dr[0].Equals("HI"));

        dt.AddColumn("second", "two");
        QVERIFY(dt.ColumnCount() == 2);
        QVERIFY(dr.ColumnCount() == 2);

        // Make sure the new column is null at first
        QVERIFY(dr["second"].Equals(QVariant()));

        dr["second"] = "v";
        QVERIFY(dr["second"].Equals("v"));
        QVERIFY(dr[1].Equals("v"));
        QVERIFY(dt.Rows()[0]["second"].Equals("v"));

        dt.AddNewRow();
        DataRow dr2 = dt[dt.RowCount() - 1];
        QVERIFY(dr2.ColumnCount() == 2);
        QVERIFY(dt.Rows().Count() == 2);
        QVERIFY(dr2[0].Equals(QVariant()));
        QVERIFY(dr2[1].Equals(QVariant()));

        dr2[0] = "Hello World!";
        QVERIFY(dr2[0].Equals("Hello World!"));

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

        QVERIFY(dt2[0][0].Equals("HI"));
        QVERIFY(dt2[0]["second"].Equals("v"));
        QVERIFY(dt2[1][0].Equals("Hello World!"));
        QVERIFY(dt2[1]["second"].Equals(QVariant()));



        // Test cloning
        DataTable dt3 = dt2.Clone();
        QVERIFY(dt3 != dt2);
        QVERIFY(dt3.Equals(dt2));

        // Change in one doesn't affect the other
        dt3[0][0] = "new value";
        QVERIFY(!dt3.Equals(dt2));
    }
    catch(Exception &ex)
    {
        dLogException(ex);
        QVERIFY(false);
    }
}

void DataSetTest::test_table_errors()
{
    DataSet ds(5);
    QVERIFY(ds.TableCount() == 5);
    QVERIFY(ds.Tables().Count() == 5);
    QVERIFY(ds.Tables()[0].ColumnCount() == 0);

    ds.Tables().Insert(DataTable(4), 0);
    QVERIFY(ds.TableCount() == 6);
    QVERIFY(ds.Tables().Count() == 6);
    QVERIFY(ds.Tables()[0].ColumnCount() == 4);

    bool exception_hit = false;
    try
    {
        // Can't add the same table twice
        ds.AddTable(ds.Tables()[0]);
    }
    catch(ValidationException)
    {
        exception_hit = true;
    }
    QVERIFY(exception_hit);


    DataTable tmp = ds[0];
    DataSet ds2;
    QVERIFY(!ds2.Contains(tmp));
    QVERIFY(ds.Contains(tmp));

    // Switch the table from ds to ds2
    ds2.AddTable(tmp);
    QVERIFY(ds2.Contains(tmp));
    QVERIFY(!ds.Contains(tmp));
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
        catch(ValidationException &)
        {
            exception_hit = true;
        }
        QVERIFY(exception_hit);


        // Add a row and access it through the dataset object
        dt1.AddNewRow( GVariantList() << "one");
        QVERIFY(ds[0][0][0].Equals("one"));

        dt2.AddNewRow( GVariantList() << "1" << "2");
        dt2.AddNewRow( GVariantList() << "3" << "4");
        QVERIFY(ds[1][0][0].Equals("1"));
        QVERIFY(ds[1][0][1].Equals("2"));
        QVERIFY(ds[1][1][0].Equals("3"));
        QVERIFY(ds[1][1][1].Equals("4"));


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
    catch(Exception &ex)
    {
        dLogException(ex);
        QVERIFY(false);
    }
}

void DataSetTest::test_derived_classes()
{
    PeopleTable pt;
    pt.AddNewRow();

    PersonDataRow pdr = pt[0];
    QVERIFY(pt.RowCount() == 1);
    QVERIFY(pt.Rows().Contains(pdr));

    pt[0].SetName("Julian");
    QVERIFY(pdr.GetName() == "Julian");
    QVERIFY(pdr[0].Equals("Julian"));

    QUuid id(pdr.GetId());
    pt[0].CloneTo(pdr);
    pt.AddRow(pdr);

    QVERIFY(pt[0].GetId() == pt[1].GetId());
    QVERIFY(pt[0].GetId() == id.toString());


    // You can even use a normal data row to reference the person data row:
    DataRow dr(pdr);
    QVERIFY(dr == pdr);
    QVERIFY(dr == pt[0]);

    pdr.CloneTo(dr);
    QVERIFY(dr[0].Equals("Julian"));
    QVERIFY(dr[1].Equals(id.toString()));

    // You can't get to the id though, unless you cast the data row as a persondatarow
    QVERIFY(((PersonDataRow &)dr).GetId() == id.toString());
}





QTEST_APPLESS_MAIN(DataSetTest);

#include "tst_datasettest.moc"
