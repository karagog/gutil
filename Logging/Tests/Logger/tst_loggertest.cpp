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

#include "Logging/filelogger.h"
#include "Logging/consolelogger.h"
#include "Logging/globallogger.h"
#include "Utils/pubsubsystem.h"
#include "Core/exception.h"
#include <QtConcurrentRun>
#include <QtCore/QString>
#include <QtTest/QtTest>
using namespace GUtil::Core;
using namespace GUtil::Utils;
using namespace GUtil::Logging;

class LoggerTest : public QObject
{
    Q_OBJECT

public:
    LoggerTest();

signals:
    void notify_message(const QString &, const QString &);

protected:
    static void log_repetetive(int id);

private Q_SLOTS:
    void initTestCase();
    void test_normal_logging();
    void test_exception_logging();

    void test_global_logging();
    void test_concurrent();

    void cleanupTestCase();
};

LoggerTest::LoggerTest()
{
}

void LoggerTest::initTestCase()
{
    GlobalLogger::SetupDefaultLogger(new FileLogger("global.log"));
    GlobalLogger::ClearLog();
}

void LoggerTest::cleanupTestCase()
{
    GlobalLogger::TakeDownLogger();
}

void LoggerTest::test_normal_logging()
{
    ConsoleLogger clog;
    FileLogger flog("test_logging.log");

    try
    {
        flog.ClearLog();

        clog.LogMessage("This is a message", "Hello world");
        flog.LogMessage("This is a message", "Hello world");

        clog.LogError("Bar", "Foo");
        flog.LogError("Bar", "Foo");

        clog.SetMessageLevel(ConsoleLogger::Error);
        flog.SetMessageLevel(ConsoleLogger::Error);

        flog.LogMessage("Shouldn't see this message");
        flog.LogWarning("Shouldn't see this warning");
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
    ConsoleLogger clog;
    FileLogger flog("test_exceptions.log");

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

void LoggerTest::test_global_logging()
{
    GlobalLogger::LogMessage("Hello World!", "Called as a function");


    // Now test a secondary log file and log to it:
    int id = GlobalLogger::SetupLogger(new FileLogger("global.2.log"));
    GlobalLogger::ClearLog(id);

    GlobalLogger::LogMessage(QVariant(id).toString(), "Hello second log!", id);

    // Test that the log doesn't exist after we take it down
    GlobalLogger::TakeDownLogger(id);
    GlobalLogger::LogMessage("You shouldn't see this message!", "", id);

    // Verify that the next logger gets the same id as the last one we took down
    QVERIFY(GlobalLogger::SetupLogger(new ConsoleLogger()) == id);
    GlobalLogger::TakeDownLogger(id);
}

void LoggerTest::test_concurrent()
{
    QFuture<void> f1 = QtConcurrent::run(log_repetetive, 1);
    QFuture<void> f2 = QtConcurrent::run(log_repetetive, 2);
    QFuture<void> f3 = QtConcurrent::run(log_repetetive, 3);
    QFuture<void> f4 = QtConcurrent::run(log_repetetive, 4);

    f1.waitForFinished();
    f2.waitForFinished();
    f3.waitForFinished();
    f4.waitForFinished();
}

void LoggerTest::log_repetetive(int id)
{
    for(int i = 0; i < 10; i++)
        GlobalLogger::LogMessage(QVariant(id).toString(), QString("Concurrent message #%1").arg(i));
}





QTEST_APPLESS_MAIN(LoggerTest);

#include "tst_loggertest.moc"