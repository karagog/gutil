/*Copyright 2014 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "gutil_globals.h"
#include <QString>
#include <QtTest>
USING_NAMESPACE_GUTIL;

class test_exception_t :public std::exception
{
public:
    virtual const char *what() const noexcept{ return "test_exception_t"; }
};

class TrycatchfinallyTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void test_basics();
    void test_special_exceptions();
};

void TrycatchfinallyTest::test_basics()
{
    // The most basic case is no exceptions
    int sequence = 0;
    int catch_executed = 0;
    int finally_executed = 0;
    TryCatchFinally(
                [&]{ sequence++; },
                [&](std::exception){ catch_executed = sequence++; },
                [&]{ finally_executed = sequence++; }
                );
    QVERIFY(catch_executed == 0);
    QVERIFY(finally_executed == 1);
    QVERIFY(sequence == 2);

    // Check that an exception gets trapped
    sequence = 0;
    catch_executed = 0;
    finally_executed = 0;
    TryCatchFinally(
                [&]{ sequence++; throw std::exception(); },
                [&](std::exception){ catch_executed = sequence++; },
                [&]{ finally_executed = sequence++; }
                );
    QVERIFY(catch_executed == 1);
    QVERIFY(finally_executed == 2);
    QVERIFY(sequence == 3);

    // Check that a different exception type escapes, but the finally is still executed
    bool exception_propagated = false;
    sequence = 0;
    catch_executed = 0;
    finally_executed = 0;
    try{
        TryCatchFinally(
                    [&]{ sequence++; throw "This is not a std::exception"; },
                    [&](std::exception){ catch_executed = sequence++; },
                    [&]{ finally_executed = sequence++; }
                    );
    } catch(...) {
        exception_propagated = true;
    }
    QVERIFY(catch_executed == 0);
    QVERIFY(finally_executed == 1);
    QVERIFY(sequence == 2);
    QVERIFY(exception_propagated);

    // Check that a rethrown exception gets propagated and
    //  the exception instance doesn't change
    exception_propagated = false;
    sequence = 0;
    catch_executed = 0;
    finally_executed = 0;
    try{
        TryCatchFinally(
        // Try
        [&]{
            sequence++;
            throw test_exception_t();
        },

        // Catch
        [&](const std::exception &ex){
            catch_executed = sequence++;

            // Make sure it's the right exception type
            QVERIFY(strcmp(ex.what(), "test_exception_t") == 0);
            throw;
        },

        // Finally
        [&]{
            finally_executed = sequence++;
        });
    }catch(const std::exception &ex){
        exception_propagated = true;

        // This is the same exception instance that was originally thrown
        QVERIFY(strcmp(ex.what(), "test_exception_t") == 0);
    }
    QVERIFY(catch_executed == 1);
    QVERIFY(finally_executed == 2);
    QVERIFY(sequence == 3);
    QVERIFY(exception_propagated);
}

void TrycatchfinallyTest::test_special_exceptions()
{
    int sequence = 0;
    bool exception_propagated = false;
    int catch_executed = 0;
    int finally_executed = 0;
    try{
        TryCatchFinally<GUtil::Exception<> >(
        // Try
        [&]{
            sequence++;
            THROW_NEW_GUTIL_EXCEPTION2(NotImplementedException, "Oops...");
        },

        // Catch
        [&](const GUtil::Exception<> &ex){
            catch_executed = sequence++;

            // Make sure you can cast it to the right exception type
            QVERIFY(strcmp(ex.what(), "GUtil::NotImplementedException<false>") == 0);
            QVERIFY(strcmp(ex.Message(), "Oops...") == 0);
            throw;
        },

        // Finally
        [&]{
            finally_executed = sequence++;
        });
    } catch(const std::exception &ex) {
        exception_propagated = true;
    }
    QVERIFY(catch_executed == 1);
    QVERIFY(finally_executed == 2);
    QVERIFY(sequence == 3);
    QVERIFY(exception_propagated);

    // The std::exception should not be caught here
    sequence = 0;
    exception_propagated = false;
    catch_executed = 0;
    finally_executed = 0;
    try{
        TryCatchFinally<GUtil::Exception<> >(
            [&]{ sequence++; throw std::exception(); },
            [&](const GUtil::Exception<> &){ catch_executed = sequence++; },
            [&]{ finally_executed = sequence++; }
        );
    } catch(const std::exception &ex) {
        exception_propagated = true;
    }
    QVERIFY(catch_executed == 0);
    QVERIFY(finally_executed == 1);
    QVERIFY(sequence == 2);
    QVERIFY(exception_propagated);
}

QTEST_APPLESS_MAIN(TrycatchfinallyTest)

#include "tst_trycatchfinallytest.moc"
