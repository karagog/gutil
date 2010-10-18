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

#include "BusinessObjects/filelogger.h"
#include "BusinessObjects/consolelogger.h"
#include "Utils/pubsubsystem.h"
#include "Core/exception.h"
#include <QtCore/QString>
#include <QtTest/QtTest>
using namespace GUtil::Core;
using namespace GUtil::Utils;
using namespace GUtil::BusinessObjects;

class LoggerTest : public QObject
{
    Q_OBJECT

public:
    LoggerTest();

private Q_SLOTS:
    void test_normal_logging();
    void test_exception_logging();
};

LoggerTest::LoggerTest()
{
}

void LoggerTest::test_normal_logging()
{
    PubSubSystem pss;
    ConsoleLogger clog(&pss, this);
    FileLogger flog("test_logging.log", &pss, this);

    try
    {
        flog.ClearLog();

        clog.LogMessage("This is a message", "Hello world");
        flog.LogMessage("This is a message", "Hello world");

        clog.LogError("Bar", "Foo");
        flog.LogError("Bar", "Foo");

        pss.PublishMessage("Message", "Title");
        pss.PublishWarning("Warning");
        pss.PublishError("Error");

        clog.SetMessageLevel(ConsoleLogger::Error);
        flog.SetMessageLevel(ConsoleLogger::Error);

        pss.PublishMessage("Shouldn't see this message");
        pss.PublishWarning("Shouldn't see this warning");
    }
    catch(GUtil::Core::Exception &ex)
    {
        clog.LogException(ex);
        flog.LogException(ex);
        QVERIFY(false);
    }
}

void LoggerTest::test_exception_logging()
{
    ConsoleLogger clog(0, this);
    FileLogger flog("test_exceptions.log", 0, this);

    flog.ClearLog();

    try
    {
        Exception ex("May the force be with you");
        ex.SetData("firstkey", "boo!");
        ex.SetData("secondkey", "what?");
        ex.SetData("thirdkey", "ERROR");
        ex.SetData("thirdkey", "this works");
        ex.SetData("fourthkey", "");
        throw ex;
    }
    catch(Exception &ex)
    {
        clog.LogException(ex);
        flog.LogException(ex);
    }


    // Important: you must catch the exception as a reference, otherwise
    //  it will cast as a lower exception and you won't know what actual
    //  type it is.  The next two try-catches demonstrate that behavior.
    try
    {
        try
        {
            throw NotImplementedException();
        }
        catch(Exception &ex)
        {
            // It appears as a NotImplementedException here
            clog.LogException(ex);
            flog.LogException(ex);
            throw;
        }
    }
    catch(Exception ex)
    {
        // It appears like a normal exception in here
        clog.LogException(ex);
        flog.LogException(ex);
    }


    try
    {
        throw std::exception();
    }
    catch(std::exception ex)
    {
        clog.LogException(ex);
        flog.LogException(ex);
    }
}

QTEST_APPLESS_MAIN(LoggerTest);

#include "tst_loggertest.moc"
