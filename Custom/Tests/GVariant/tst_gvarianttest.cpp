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

}

QTEST_APPLESS_MAIN(GVariantTest);

#include "tst_gvarianttest.moc"
