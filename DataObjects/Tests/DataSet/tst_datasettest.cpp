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
    void testCase1();
    void testCase1_data();
};

DataSetTest::DataSetTest()
{
}

void DataSetTest::testCase1()
{
    DataTable dt(3);
    dt.AddRow();

    dt.Rows()[0][0] = "HI";
    dt.Rows()[0][1] = "2";
    dt.Rows()[0][2] = "3";
}

void DataSetTest::testCase1_data()
{

}

QTEST_APPLESS_MAIN(DataSetTest);

#include "tst_datasettest.moc"
