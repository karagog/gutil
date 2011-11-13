#include <QtCore/QString>
#include <QtTest/QtTest>
#include <QTime>
#include <QRegExp>
#include <QSize>
#include <QVariantList>
#include <QVariantMap>
#include "Custom/gvariant.h"
#include "Custom/updatablegvariant.h"
#include "Core/exception.h"
using namespace GUtil;
GUTIL_USING_NAMESPACE(Custom);
GUTIL_USING_CORE_NAMESPACE(DataObjects);


class GVariantTest :
        public QObject,
        public UpdatableGVariant::Observer
{
    Q_OBJECT

public:
    GVariantTest();

private Q_SLOTS:
    void test_basic_types();
    void test_simple_qt_types();
    void test_collections();
    void test_callbacks();

private:

    void value_about_to_change(const GVariant &old, const GVariant &newval);

};

GVariantTest::GVariantTest()
{
}

void GVariantTest::test_basic_types()
{
    GVariant gv1, gv2;
    QString tmps;

    // Test integers
    gv1 = (int)5;
    gv2.FromXmlQString(gv1.ToXmlQString());

    qDebug(gv1.ToXmlString());
    QVERIFY(gv1 == gv2);
    QVERIFY(gv1 == 5);

    // Unsigned integers
    gv1 = (uint)5;
    qDebug(gv1.ToXmlString());

    gv1 = (uint)-2000;
    gv2 = gv1.toInt();
    qDebug(gv1.ToXmlString());
    qDebug(gv2.ToXmlString());
    QVERIFY(gv1 != gv2);

    gv2.FromXmlQString(gv1.ToXmlQString());
    QVERIFY(gv1 == gv2);

    // Char
    gv1 = QChar(0x0);
    tmps = gv1.ToXmlQString();
    gv2.FromXmlQString(tmps);
    qDebug(gv1.ToXmlString());
    QVERIFY2(gv1 == gv2, gv2.toGString().ToBase16());
    QVERIFY(gv2 == QChar(0x0));

    // Bool
    gv1 = true;
    gv2.FromXmlQString(gv1.ToXmlQString());
    QVERIFY(gv1 == gv2);
    QVERIFY(gv2 == true);

    qDebug(gv1.ToXmlString());

    gv1 = false;
    gv2.FromXmlQString(gv1.ToXmlQString());
    QVERIFY(gv1 == gv2);
    QVERIFY(gv2 == false);

    // Double
    gv1 = (double)5.68921;
    gv2.FromXmlQString(gv1.ToXmlQString());
    QVERIFY(gv1 == gv2);
    QVERIFY(gv2 == 5.68921);

    qDebug(gv1.ToXmlString());

    // Float
    gv1 = (float)5.68921;
    gv2.FromXmlQString(gv1.ToXmlQString());
    qDebug(gv1.ToXmlString());
    QVERIFY(gv2 == (float)5.68921);
    QVERIFY(gv1 == (float)5.68921);

    qDebug(gv1.ToXmlString());

    // GString
    gv1 = String("Hello World!");
    tmps = gv1.ToXmlQString();
    gv2.FromXmlQString(tmps);
    qDebug(gv1.ToXmlString());
    QVERIFY2(gv1 == gv2, gv1.toGString());
    QVERIFY2(gv2 == "Hello World!", gv2.toGString());
    QVERIFY2(gv1 == "Hello World!", gv1.toGString());
}

void GVariantTest::test_simple_qt_types()
{
    // Strings
    GVariant gv1 = "Hello!";
    GVariant gv2;
    gv2.FromXmlQString(gv1.ToXmlQString());
    qDebug(gv1.ToXmlString());
    QVERIFY2(gv1 == gv2, gv2.toGString());
    QVERIFY2(gv2 == "Hello!", gv2.toGString());

    QString tmps;
    tmps.append((char)0x00);
    tmps.append((char)0x01);
    tmps.append((char)0x02);
    gv1 = tmps;
    gv2.FromXmlQString(gv1.ToXmlQString());
    qDebug(gv1.ToXmlString());
    QVERIFY(gv1 == gv2);
    QVERIFY(gv2 == tmps);

    // Byte Arrays
    QByteArray ba;
    ba.append((char)0x02);
    ba.append((char)0x01);
    ba.append((char)0x00);
    gv1 = ba;
    gv2.FromXmlQString(gv1.ToXmlQString());
    qDebug(gv1.ToXmlString());
    QVERIFY(gv1 == gv2);
    QVERIFY(gv2 == ba);

    // QDates
    QDate dt(2005, 4, 20);
    gv1 = dt;
    gv2.FromXmlQString(gv1.ToXmlQString());
    qDebug(gv1.ToXmlString());
    QVERIFY(gv1 == gv2);
    QVERIFY(gv2 == dt);

    // QTimes
    QTime tm(1, 2, 3, 4);
    gv1 = tm;
    gv2.FromXmlQString(gv1.ToXmlQString());
    qDebug(gv1.ToXmlString());
    QVERIFY(gv1 == gv2);
    QVERIFY(gv2 == tm);

    tm = QTime(1, 2);
    gv1 = tm;
    gv2.FromXmlQString(gv1.ToXmlQString());
    qDebug(gv1.ToXmlString());
    QVERIFY(gv1 == gv2);
    QVERIFY(gv2 == tm);

    // QDateTimes
    QDateTime dtt(dt, tm);
    gv1 = dtt;
    gv2.FromXmlQString(gv1.ToXmlQString());
    qDebug(gv1.ToXmlString());
    QVERIFY(gv1 == gv2);
    QVERIFY(gv2 == dtt);

    // Bit arrays
    QBitArray b(4);
    b.setBit(0, 1);
    b.setBit(1, 1);
    b.setBit(2, 0);
    b.setBit(3, 1);
    gv1 = b;
    gv2.FromXmlQString(gv1.ToXmlQString());
    qDebug(gv1.ToXmlString());
    QVERIFY(gv1 == gv2);
    QVERIFY(gv2 == b);

    // QRegExp
    QRegExp exp("&pattern/>", Qt::CaseInsensitive, QRegExp::Wildcard);
    QRegExp wrong("&pattern/>", Qt::CaseSensitive, QRegExp::Wildcard);
    gv1 = exp;
    gv2.FromXmlQString(gv1.ToXmlQString());
    qDebug(gv1.ToXmlString());
    QVERIFY(gv1 == gv2);
    QVERIFY(gv2 == exp);
    QVERIFY(gv1 != wrong);

    // QUrl
    QUrl u("http://tempzone.com");
    gv1 = u;
    gv2.FromXmlQString(gv1.ToXmlQString());
    qDebug(gv1.ToXmlString());
    QVERIFY(gv1 == gv2);
    QVERIFY(gv2 == u);

    // QRect
    QRect rect(1,2,3,4);
    gv1 = rect;
    gv2.FromXmlQString(gv1.ToXmlQString());
    qDebug(gv1.ToXmlString());
    QVERIFY(gv1 == gv2);
    QVERIFY(gv2 == rect);

    // QSize
    QSize sz(1, 2);
    gv1 = sz;
    gv2.FromXmlQString(gv1.ToXmlQString());
    qDebug(gv1.ToXmlString());
    QVERIFY(gv1 == gv2);
    QVERIFY(gv2 == sz);

    // QUuid
    QUuid id(QUuid::createUuid());
    gv1 = id;
    gv2.FromXmlQString(gv1.ToXmlQString());
    qDebug(gv1.ToXmlString());
    QVERIFY(gv1 == gv2);
    QVERIFY(gv2 == id);
}

void GVariantTest::test_collections()
{
    GVariant gv1, gv2;

    // Stringlist
    QStringList slbad("one");
    QStringList sl("one");
    sl<<"two"<<"three"<<"four";
    gv1 = sl;
    gv2.FromXmlQString(gv1.ToXmlQString());
    //qDebug(gv1.ToXmlString(true).c_str());
    QVERIFY(gv1 == gv2);
    QVERIFY(gv2 == sl);
    QVERIFY(gv2 != slbad);

    // QVariantMap
    QVariantMap vm;
    vm.insert("one", slbad);
    vm.insert("two", sl);
    gv1 = vm;
    gv2.FromXmlQString(gv1.ToXmlQString());
    //qDebug(gv1.ToXmlString(true).c_str());
    QVERIFY(gv1 == gv2);
    QVERIFY(gv2 == vm);

    // VariantList
    QVariantList vl1, vl2;
    vl1.append(sl);
    vl1.append(slbad);
    vl2.append(vl1);
    vl2.append(5);
    vl2.append("Hello!");
    gv1 = vl2;
    gv2.FromXmlQString(gv1.ToXmlQString());
    //qDebug(gv1.ToXmlString(true));
    QVERIFY(gv1 == gv2);
    QVERIFY(gv2 == vl2);
}


void GVariantTest::value_about_to_change(const GVariant &, const GVariant &newval)
{
    if(newval == 5)
        THROW_NEW_GUTIL_EXCEPTION2(Core::ValidationException,
                                  "No fives!");
}

void GVariantTest::test_callbacks()
{
    try
    {
        UpdatableGVariant v;
        v.SetValueObserver(this);

        bool exception_hit = false;
        try
        {
            v.setValue(5);
        }
        catch(Core::ValidationException<> &)
        {
            exception_hit = true;
        }
        QVERIFY(exception_hit);


        exception_hit = false;
        try
        {
            v = 5;
        }
        catch(Core::ValidationException<> &)
        {
            exception_hit = true;
        }
        QVERIFY(exception_hit);


        // Conversion from "6" is fine
        v = "6";
        v.convert(GVariant::Int);

        // But if it's a 5 it fails
        exception_hit = false;
        try
        {
            v = "5";
        }
        catch(Core::ValidationException<> &)
        {
            exception_hit = true;
        }
        QVERIFY(exception_hit);
    }
    catch(Core::Exception<> &ex)
    {
        QFAIL("Exception Hit");
    }
}

QTEST_APPLESS_MAIN(GVariantTest);

#include "tst_gvarianttest.moc"
