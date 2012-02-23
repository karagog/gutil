/*Copyright 2010-2012 George Karagoulis


Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include <QtCore/QString>
#include <QtTest/QtTest>
#include "Core/DataObjects/datatable.h"
USING_NAMESPACE_GUTIL1(DataObjects);
USING_NAMESPACE_GUTIL1(Utils);

class DataTableTest : public QObject
{
    Q_OBJECT

public:
    DataTableTest();

private Q_SLOTS:
    void test_basics();
    void test_columns();
    void test_update_functionality();
    void test_copies();
};

DataTableTest::DataTableTest()
{
}

void DataTableTest::test_basics()
{
    DataTable<int> dt;
    QVERIFY(dt.RowCount() == 0);
    QVERIFY(dt.ColumnCount() == 0);

    dt.AddColumn("first");
    QVERIFY(dt.ColumnCount() == 1);
    QVERIFY(dt.Columns()[0] == "first");

    SharedSmartPointer< DataRow<int> > dr = dt.CreateRow();
    QVERIFY(dt.RowCount() == 0);
    QVERIFY(dr->ColumnCount() == 1);

    dt.AddRow(dr);
    QVERIFY(dt.RowCount() == 1);

    dr->operator [](0) = 1;
    QVERIFY(dr->operator [](0) == 1);
    QVERIFY(dr->operator [](0) != 5);

    dr->operator [](0) = 5;
    QVERIFY(dr->operator [](0) == 5);
    QVERIFY(dr->operator []("first") == 5);
    QVERIFY(dt[0][0] == 5);
    QVERIFY(dt[0]["first"] == 5);
}

void DataTableTest::test_columns()
{
    DataTable<int> dt;
    dt.AddColumn("test");
    QVERIFY(dt.ColumnCount() == 1);

    bool ex_hit = false;
    try
    {
        // Can't insert duplicate columns
        dt.AddColumn("test");
    }
    catch(const GUtil::ValidationException<> &)
    {
        ex_hit = true;
    }
    QVERIFY(ex_hit);
    QVERIFY(dt.ColumnCount() == 1);


    ex_hit = false;
    try
    {
        // Can't insert duplicate columns
        dt.Columns().Add("test");
    }
    catch(const GUtil::ValidationException<> &)
    {
        ex_hit = true;
    }
    QVERIFY(ex_hit);
    QVERIFY(dt.ColumnCount() == 1);


    dt.AddColumn("second_column");
    ex_hit = false;
    try
    {
        // Can't insert duplicate columns
        dt.Columns()[1] = "test";
    }
    catch(const GUtil::ValidationException<> &)
    {
        ex_hit = true;
    }
    QVERIFY(ex_hit);
    QVERIFY(dt.ColumnCount() == 2);


    DataTable<int> dt2(5);
    QVERIFY(dt2.ColumnCount() == 5);
    QVERIFY(dt2.Columns()[0].Value().GetKey() == "0");
    QVERIFY(dt2.Columns()[1].Value().GetKey() == "1");
    QVERIFY(dt2.Columns()[2].Value().GetKey() == "2");
    QVERIFY(dt2.Columns()[3].Value().GetKey() == "3");
    QVERIFY(dt2.Columns()[4].Value().GetKey() == "4");
}

void DataTableTest::test_update_functionality()
{
    DataTable<int> dt(10);
    QVERIFY(!dt.IsDirty());

    dt.AddRow();
    QVERIFY(dt.IsDirty());

    dt.CommitChanges();
    QVERIFY(!dt.IsDirty());

    if(dt[0][0] == 5);
    QVERIFY(!dt.IsDirty());

    dt[0][0] = 5;
    QVERIFY(dt.IsDirty());
    QVERIFY(dt[0].IsDirty());

    dt.CommitChanges();
    QVERIFY(!dt.IsDirty());
    QVERIFY(!dt[0].IsDirty());
}

void DataTableTest::test_copies()
{
    DataTable<int> dt(2);
    dt.SetName("My Table");
    dt.AddRow();
    dt.AddRow();

    dt[0][0] = 0;
    dt[0][1] = 1;
    dt[1][0] = 2;
    dt[1][1] = 3;
    QVERIFY(dt[0][0] == 0);
    QVERIFY(dt[0][1] == 1);
    QVERIFY(dt[1][0] == 2);
    QVERIFY(dt[1][1] == 3);
    QVERIFY(dt.Name() == "My Table");

    DataTable<int> dt2(dt);
    QVERIFY(dt2.RowCount() == 2);
    QVERIFY(dt2.ColumnCount() == 2);
    QVERIFY(dt2[0][0] == 0);
    QVERIFY(dt2[0][1] == 1);
    QVERIFY(dt2[1][0] == 2);
    QVERIFY(dt2[1][1] == 3);
    QVERIFY(dt2.Name() == "My Table");

    // Changes to table 1 don't affect table 2
    dt[0][0] = 555;
    QVERIFY(dt[0][0] == 555);
    QVERIFY(dt2[0][0] == 0);

    dt.SetName("Changed");
    QVERIFY(dt.Name() == "Changed");
    QVERIFY(dt2.Name() == "My Table");

    dt.Columns()[1].SetValue("new_key");
    QVERIFY(dt.Columns()[1].Value().GetKey() == "new_key");
    QVERIFY(dt2.Columns()[1].Value().GetKey() != "new_key");
}

QTEST_APPLESS_MAIN(DataTableTest);

#include "tst_datatabletest.moc"
