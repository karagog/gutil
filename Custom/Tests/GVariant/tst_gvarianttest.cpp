#include <QtCore/QString>
#include <QtTest/QtTest>
#include "Custom/gvariant.h"
#include "Logging/debuglogger.h"
using namespace GUtil::Custom;
using namespace GUtil::Logging;

class GVariantTest : public QObject
{
    Q_OBJECT

public:
    GVariantTest();

private Q_SLOTS:
    void test_basic_types();
    void test_simple_qt_types();
    void test_collections();
};

GVariantTest::GVariantTest()
{
}

void GVariantTest::test_basic_types()
{
    GVariant gv1, gv2;

    // Test integers
    gv1 = (int)5;
    gv2.FromXmlQString(gv1.ToXmlQString());

    qDebug(gv1.ToXmlString().c_str());
    QVERIFY(gv1 == gv2);
    QVERIFY(gv1 == 5);

    // Unsigned integers
    gv1 = (uint)5;
    qDebug(gv1.ToXmlString().c_str());

    gv1 = (uint)-2000;
    gv2 = gv1.toInt();
    qDebug(gv1.ToXmlString().c_str());
    qDebug(gv2.ToXmlString().c_str());
    QVERIFY(gv1 != gv2);

    gv2.FromXmlQString(gv1.ToXmlQString());
    QVERIFY(gv1 == gv2);

    // Char
    gv1 = QChar(0x0);
    gv2.FromXmlQString(gv1.ToXmlQString());
    qDebug(gv1.ToXmlString().c_str());
    QVERIFY(gv1 == gv2);
    QVERIFY(gv2 == QChar(0x0));

    // Bool
    gv1 = true;
    gv2.FromXmlQString(gv1.ToXmlQString());
    QVERIFY(gv1 == gv2);
    QVERIFY(gv2 == true);

    qDebug(gv1.ToXmlString().c_str());

    gv1 = false;
    gv2.FromXmlQString(gv1.ToXmlQString());
    QVERIFY(gv1 == gv2);
    QVERIFY(gv2 == false);

    // Double
    gv1 = (double)5.68921;
    gv2.FromXmlQString(gv1.ToXmlQString());
    QVERIFY(gv1 == gv2);
    QVERIFY(gv2 == 5.68921);

    qDebug(gv1.ToXmlString().c_str());

    // Float
    gv1 = (float)5.68921;
    qDebug(gv1.ToXmlString().c_str());
    gv2.FromXmlQString(gv1.ToXmlQString());
    QVERIFY(gv1 == gv2);
    QVERIFY(gv2 == 5.68921);

    qDebug(gv1.ToXmlString().c_str());
}

void GVariantTest::test_simple_qt_types()
{

}

void GVariantTest::test_collections()
{

}

QTEST_APPLESS_MAIN(GVariantTest);

#include "tst_gvarianttest.moc"
