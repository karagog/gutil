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
    void test_dataTable();
    void test_dataRows();
};

DataSetTest::DataSetTest()
{
}

void DataSetTest::test_dataTable()
{
    try
    {
        DataTable dt(3);
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


        // Set some values on the row and make sure they persist
        dt.Rows()[0][0] = "HI";
        dt.Rows()[0][1] = "2";
        dt.Rows()[0][2] = "3";

        QVERIFY(dt.Rows()[0][0].toString() == "HI");
        QVERIFY(dt.Rows()[0][1].toString() == "2");
        QVERIFY(dt.Rows()[0][2].toString() == "3");


        // Resize the columns and check the rows
        dt.SetColumnKeys(QStringList("OneColumn"));
        QVERIFY(dt.ColumnCount() == 1);
        QVERIFY(dr.ColumnCount() == 1);

    }
    catch(Core::Exception &ex)
    {
        dLogException(ex);
        QVERIFY(false);
    }
}

void DataSetTest::test_dataRows()
{
    try
    {
        DataTable t(1);
        DataRow r1 = t.CreateRow();
        DataRow r2;

        // A null row has no index
        QVERIFY(r2.Index() == -1);

        // Setting them equal will tie their shared data pointers together
        r2 = r1;

        // Both data rows should be looking at the same data source
        r1[0] = "Value";
        QVERIFY(r1[0] == r2[0]);
        QVERIFY(r1[0] == "Value");
        QVERIFY(r1["0"] == "Value");

        DataRow r3 = r2.Clone();
        QVERIFY(r3[0] == "Value");

        // They're not equal, because after cloning the row identifier is unique
        QVERIFY(r3 != r2);

        // A change to either of the first two doesn't affect the cloned row
        r2[0] = "1";
        QVERIFY(r2[0] == "1");
        QVERIFY(r1[0] == "1");
        QVERIFY(r3[0] != "1");

    }
    catch(Core::Exception &ex)
    {
        dLogException(ex);
        QVERIFY(false);
    }
}



QTEST_APPLESS_MAIN(DataSetTest);

#include "tst_datasettest.moc"
