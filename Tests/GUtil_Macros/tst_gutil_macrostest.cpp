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

#include "gutil_macros.h"
#include <QtCore/QString>
#include <QtTest/QtTest>


// Convenient namespace declaration
<<<<<<< HEAD
GUTIL_BEGIN_NAMESPACE( DummyTest )
=======
GUTIL_BEGIN_NAMESPACE(DummyTest);
>>>>>>> Added test project for my macros header, and tweaked some macros so they require

class TestClass
{
public:
<<<<<<< HEAD

    // You can adjust the private variables themselves by using the _p_ prefix
    TestClass() : _p_MyReadOnlyInt(10) {}

    // Convenient property declarations
    PROPERTY( MyInt, int );
    PROPERTY( MyString, QString );

    READONLY_PROPERTY( MyReadOnlyInt, int );


    // Or you can declare the accessors, and define the function bodies like below:
    PROPERTY_ACCESSORS( IsMeInsane, bool );

    READONLY_PROPERTY_ACCESSORS( Sexy, bool );
};

bool TestClass::GetIsMeInsane() const
{
    return true;
}

void TestClass::SetIsMeInsane(const bool &value)
{
    // You can't change this
}

bool TestClass::GetSexy() const
{
    return true;
}

GUTIL_END_NAMESPACE
=======
    TestClass(){
        _p_MyReadOnlyInt = 10;
    }

    // Convenient property declarations
    GUTIL_PROPERTY(int, MyInt);
    GUTIL_PROPERTY(QString, MyString);

    GUTIL_READONLY_PROPERTY(int, MyReadOnlyInt);
};

GUTIL_END_NAMESPACE();
>>>>>>> Added test project for my macros header, and tweaked some macros so they require




class GUtil_MacrosTest : public QObject
{
    Q_OBJECT

public:
    GUtil_MacrosTest();

private Q_SLOTS:
    void test_macros();
};




using namespace GUtil::DummyTest;

GUtil_MacrosTest::GUtil_MacrosTest()
{
}

void GUtil_MacrosTest::test_macros()
{
    TestClass tc;
    tc.SetMyInt(5);
    QVERIFY(tc.GetMyInt() == 5);

    tc.SetMyString("Hello!");
    QVERIFY(tc.GetMyString() != "Hi");
    QVERIFY(tc.GetMyString() == "Hello!");


    // Readonly Property
    QVERIFY(tc.GetMyReadOnlyInt() == 10);
<<<<<<< HEAD

    // Custom getters and setters:
    QVERIFY(tc.GetIsMeInsane());

    // Our setter does nothing
    tc.SetIsMeInsane(false);
    QVERIFY(tc.GetIsMeInsane());


    // A custom readonly property
    QVERIFY(tc.GetSexy());
=======
>>>>>>> Added test project for my macros header, and tweaked some macros so they require
}

QTEST_APPLESS_MAIN(GUtil_MacrosTest);

#include "tst_gutil_macrostest.moc"
