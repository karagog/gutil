#include "DataObjects/dataset.h"
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
    DataTable dt(3);
    DataRow dr = dt.CreateRow();

    QVERIFY(dr.Index() == 0);

    // Make sure we have the right number of columns
    QVERIFY(dt.ColumnCount() == 3);
    QVERIFY(dr.ColumnCount() == 3);


    // The table doesn't actually have this row in it yet, until you call 'add'
    QVERIFY(dt.Rows().Count() == 0);
    dt.AddRow(dr);
    QVERIFY(dt.Rows().Count() == 1);
    QVERIFY(dr.ColumnCount() == 3);


    // Set some values on the row and make sure they persist
    dt.Rows()[0][0] = "HI";
    dt.Rows()[0][1] = "2";
    dt.Rows()[0][2] = "3";

    QVERIFY(dt.Rows()[0][0].toString() == "HI");
    QVERIFY(dt.Rows()[0][1].toString() == "2");
    QVERIFY(dt.Rows()[0][2].toString() == "3");
}

void DataSetTest::test_dataRows()
{
    DataTable t(1);
    DataRow r1 = t.CreateRow();
    DataRow r2(r1);

    // Both data rows should be looking at the same data source
    r1[0] = "Value";
    QVERIFY(r1[0] == r2[0]);
    QVERIFY(r1[0] == "Value");
    QVERIFY(r1["0"] == "Value");

    DataRow r3 = r2.Clone();
    QVERIFY(r3[0] == "Value");


}



QTEST_APPLESS_MAIN(DataSetTest);

#include "tst_datasettest.moc"
