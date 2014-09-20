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
#include "gutil_extendedexception.h"
#include "gutil_consolelogger.h"
USING_NAMESPACE_GUTIL;

class ExceptionTest : public QObject
{
    Q_OBJECT

public:
    ExceptionTest();

private Q_SLOTS:
    void test_basic();

    void test_other_exceptions();
    void test_polymorphic_cast();

    void test_data_exceptions();
    void test_inner_exceptions();
};

ExceptionTest::ExceptionTest()
{
}

void ExceptionTest::test_basic()
{
    qDebug() << "Size of normal Exception:" << sizeof(Exception<false>);
    qDebug() << "Size of customized Exception:" << sizeof(XmlException<false>);
    qDebug() << "Size of extended Exception:" << sizeof(Exception<true>);
    qDebug() << "Size of customized Extended Exception:" << sizeof(XmlException<true>);

    try
    {
        bool exception_hit(false);
        try
        {
            throw Exception<false>();
        }
        catch(const Exception<false> &e)
        {
            exception_hit = true;
        }
        QVERIFY(exception_hit);


        // The exact same test as above, only with cooler syntax
        exception_hit = false;
        try
        {
            throw Exception<>();
        }
        catch(const Exception<> &e)
        {
            exception_hit = true;
        }
        QVERIFY(exception_hit);



        // Try catching extended exceptions
        exception_hit = false;
        try
        {
            throw Exception<true>();
        }
        catch(const Exception<true> &e)
        {
            exception_hit = true;
        }
        QVERIFY(exception_hit);

        // Try out the cool initializer list syntax
        exception_hit = false;
        try
        {
            std::string s1 = "Hi";
            throw Exception<true>{
                {"Hello", "World"},
                {"More", "Data"},
                {s1, "George"}
            };
        }
        catch(const Exception<true> &e)
        {
            exception_hit = true;
        }
        QVERIFY(exception_hit);

        // Try out the cool initializer list syntax
        exception_hit = false;
        try
        {
            std::string s1 = "Hi";
            throw Exception<true>("This is the message",
                                  XmlException<>("This is the inner exception"),
            {{"Hello", "World"},
             {"More", "Data"}});
        }
        catch(const Exception<true> &e)
        {
            exception_hit = true;
            //ConsoleLogger().LogException(e);
        }
        QVERIFY(exception_hit);


        // An extended exception should also be caught as a non-extended exception.
        exception_hit = false;
        try
        {
            throw Exception<true>();
        }
        catch(const Exception<false> &e)
        {
            exception_hit = true;
        }
        QVERIFY(exception_hit);
    }
    catch(const Exception<false> &e)
    {
        QVERIFY(false);
    }
    catch(...)
    {
        QVERIFY(false);
    }
}

void ExceptionTest::test_other_exceptions()
{
    try
    {
        bool exception_caught(false);
        try
        {
            throw XmlException<>();
        }
        catch(XmlException<> &)
        {
            exception_caught = true;
        }
        QVERIFY(exception_caught);


        exception_caught = false;
        try
        {
            throw XmlException<true>();
        }
        catch(const XmlException<false> &)
        {
            exception_caught = true;
        }
        QVERIFY(exception_caught);


        exception_caught = false;
        try
        {
            throw XmlException<true>();
        }
        catch(const Exception<true> &)
        {
            // It should not be caught here, because XmlException is not a subclass
            //  of Exception<true>
        }
        catch(XmlException<false> &)
        {
            exception_caught = true;
        }
        QVERIFY(exception_caught);



        // It should, however, be caught by the base Exception<false> class
        exception_caught = false;
        try
        {
            throw XmlException<true>();
        }
        catch(const Exception<> &)
        {
            exception_caught = true;
        }
        QVERIFY(exception_caught);
    }
    catch(const Exception<false> &e)
    {
        QVERIFY(false);
    }
    catch(...)
    {
        QVERIFY(false);
    }
}

void ExceptionTest::test_data_exceptions()
{
    try
    {
        throw Exception<>();
    }
    catch(const Exception<> &ex)
    {
        QVERIFY(ex.what());
        QVERIFY2(0 == strcmp(ex.what(), "GUtil::Exception<false>"), ex.what());
    }

    try
    {
        throw Exception<true>();
    }
    catch(const Exception<false> &ex)
    {
        QVERIFY(ex.what());
        QVERIFY2(0 == strcmp(ex.what(), "GUtil::Exception<true>"), ex.what());
    }



    try
    {
        throw XmlException<>();
    }
    catch(const XmlException<> &ex)
    {
        QVERIFY(ex.what());
        QVERIFY2(0 == strcmp(ex.what(), "GUtil::XmlException<false>"), ex.what());
    }

    try
    {
        throw XmlException<true>();
    }
    catch(const XmlException<> &ex)
    {
        QVERIFY(ex.what());
        QVERIFY2(0 == strcmp(ex.what(), "GUtil::XmlException<true>"), ex.what());
    }

    try
    {
        throw XmlException<>();
    }
    catch(const Exception<> &ex)
    {
        QVERIFY(ex.what());
        QVERIFY2(0 == strcmp(ex.what(), "GUtil::XmlException<false>"), ex.what());
    }

    Exception<false> e("Hello");
    QVERIFY2(0 == strcmp(e.Message(), "Hello"), e.Message());
}

void ExceptionTest::test_inner_exceptions()
{
    XmlException<false> xex_false;
    XmlException<true> xex_true;
    Exception<true> ex1("", xex_false);
    Exception<true> ex2("", xex_true);
    QVERIFY(ex1.GetInnerException());
    QVERIFY(ex2.GetInnerException());
    QVERIFY(0 == strcmp(ex1.GetInnerException()->what(), "GUtil::XmlException<false>"));
    QVERIFY(0 == strcmp(ex2.GetInnerException()->what(), "GUtil::XmlException<true>"));

    // You should be able to dynamic cast it as the proper type
    QVERIFY(NULL != dynamic_cast< const XmlException<false> *>(ex1.GetInnerException()));
    QVERIFY(NULL != dynamic_cast< const Exception<false> *>(ex1.GetInnerException()));
    QVERIFY(NULL == dynamic_cast< const XmlException<true> *>(ex1.GetInnerException()));
    QVERIFY(NULL != dynamic_cast< const XmlException<true> *>(ex2.GetInnerException()));
    QVERIFY(NULL == dynamic_cast< const Exception<true> *>(ex2.GetInnerException()));

    // Copying should have no effect on the inner exceptions
    Exception<true> ex1_cpy( ex1 );
    Exception<true> ex2_cpy( ex2 );
    QVERIFY(ex1_cpy.GetInnerException());
    QVERIFY(ex2_cpy.GetInnerException());
    QVERIFY(0 == strcmp(ex1_cpy.GetInnerException()->what(), "GUtil::XmlException<false>"));
    QVERIFY(0 == strcmp(ex2_cpy.GetInnerException()->what(), "GUtil::XmlException<true>"));
    QVERIFY(NULL != dynamic_cast< const XmlException<false> *>(ex1_cpy.GetInnerException()));
    QVERIFY(NULL != dynamic_cast< const Exception<false> *>(ex1_cpy.GetInnerException()));
    QVERIFY(NULL == dynamic_cast< const XmlException<true> *>(ex1_cpy.GetInnerException()));
    QVERIFY(NULL != dynamic_cast< const XmlException<true> *>(ex2_cpy.GetInnerException()));
    QVERIFY(NULL == dynamic_cast< const Exception<true> *>(ex2_cpy.GetInnerException()));
}

void ExceptionTest::test_polymorphic_cast()
{
    try
    {
        throw XmlException<true>();
    }
    catch(const Exception<> &ex)
    {
        QVERIFY(dynamic_cast< const XmlException<false> *>(&ex));
        QVERIFY(dynamic_cast< const XmlException<true> *>(&ex));
        QVERIFY(!dynamic_cast< const Exception<true> *>(&ex));
    }
}

QTEST_APPLESS_MAIN(ExceptionTest);

#include "tst_exceptiontest.moc"
