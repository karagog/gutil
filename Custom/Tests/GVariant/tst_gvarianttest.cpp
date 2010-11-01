#include <QtCore/QString>
#include <QtTest/QtTest>
#include "Custom/gvariant.h"
using namespace GUtil::Custom;

class GVariantTest : public QObject
{
    Q_OBJECT

public:
    GVariantTest();

private Q_SLOTS:
    void testGVariant();
};

GVariantTest::GVariantTest()
{
}

void GVariantTest::testGVariant()
{
    GVariant gv1, gv2;

    gv1 = GVariant(5);
    gv2.FromXmlQString(gv1.ToXmlQString());
    QVERIFY(gv1 == gv2 == 5);


}

QTEST_APPLESS_MAIN(GVariantTest);

#include "tst_gvarianttest.moc"
