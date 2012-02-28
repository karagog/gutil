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
#include "Logging/grouplogger.h"
#include "Logging/debuglogger.h"
#include "gutil_extendedexception.h"
#include <QtConcurrentRun>
#include <QtCore/QString>
#include <QtTest/QtTest>
USING_NAMESPACE_GUTIL;
USING_NAMESPACE_GUTIL1(Utils);
USING_NAMESPACE_GUTIL1(Logging);
USING_NAMESPACE_GUTIL1(DataObjects);

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
    //void test_concurrent();

    void test_grouplogger();

    void cleanupTestCase();
};

void LoggerTest::log_repetetive(int id)
{
    for(int i = 0; i < 10; i++)
        GlobalLogger::LogMessage(String::FromInt(id),
                                 String::Format("Concurrent message #%d", i));
}

LoggerTest::LoggerTest()
{
}

void LoggerTest::initTestCase()
{

}

void LoggerTest::cleanupTestCase()
{

}

void LoggerTest::test_normal_logging()
{
    ConsoleLogger clog;
    FileLogger flog("test_logging.log");

    try
    {
        flog.Clear();

        clog.Log("This is a message", "Hello world");
        flog.Log("This is a message", "Hello world");

        clog.Log("Bar", "Foo", flog.Error);
        flog.Log("Bar", "Foo", flog.Error);

        clog.SetMessageLevelTolerance(ConsoleLogger::Error);
        flog.SetMessageLevelTolerance(ConsoleLogger::Error);

        flog.Log("Shouldn't see this message");
        flog.Log("Shouldn't see this warning", "", flog.Warning);
    }
    catch(const Exception<> &ex)
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

    flog.Clear();

    try
    {
        Exception<true> ex("May the force be with you");
        ex.SetData("firstkey", "boo!");
        ex.SetData("secondkey", "what?");
        ex.SetData("thirdkey", "ERROR");
        ex.SetData("thirdkey", "this works");
        ex.SetData("fourthkey", "");

        THROW_GUTIL_EXCEPTION(ex);
    }
    catch(const Exception<> &ex)
    {
        clog.LogException(ex);
        flog.LogException(ex);
    }


    // Important: you must catch the exception as a reference, otherwise
    //  it will cast as a lower exception and you won't know what actual
    //  type it is.  The next try-catche demonstrate that behavior.
    try
    {
        try
        {
            THROW_NEW_GUTIL_EXCEPTION2(NotImplementedException, "Boo!");
        }
        catch(const Exception<> &ex)
        {
            // It appears as a NotImplementedException here
            clog.LogException(ex);
            flog.LogException(ex);
            throw;
        }
    }
    catch(Exception<> ex)
    {
        // It appears like a normal exception in here
        clog.LogException(ex);
        flog.LogException(ex);
    }
}

void LoggerTest::test_global_logging()
{
    // Note: You can customize your own logfile like this, otherwise accept the default one
    //dSetupLogFile("newdebug.log");

    // Log to the debug log
    dLogMessage("Hello World!");

    // Test logging a variable
//    int test_variable = 77;
//    dLogVariable(test_variable);

//    // Try different kinds of variables
//    QString tmps("Hello!");
//    dLogVariable(tmps.toAscii());

//    double tmpd = .4524123;
//    float tmpf = tmpd;
//    dLogVariable(tmpd);
//    dLogVariable(tmpf);

//    dLogVariable("Hello World! ");

//    bool tmpb = true;
//    dLogVariable(tmpb);


    // Setup the default logger
    FileLogger *flogger;
    int id = GlobalLogger::SetupDefaultLogger(flogger = new FileLogger("global.log"));
    flogger->Clear();
    GlobalLogger::LogMessage(String::FromInt(id), "Hello global log!");

    // Now test a secondary log file and log to it:
    id = GlobalLogger::SetupLogger(flogger = new FileLogger("global2.log"));
    flogger->Clear();
    GlobalLogger::LogMessage(String::FromInt(id), "Hello global log!", id);

    // Test that the log doesn't exist after we take it down
    GlobalLogger::TakeDownLogger(id);
    GlobalLogger::LogMessage("You shouldn't see this message!", "", id);

    // Verify that the next logger gets the same id as the last one we took down
    QVERIFY(GlobalLogger::SetupLogger(new ConsoleLogger()) == id);
    GlobalLogger::TakeDownLogger(id);
}

//void LoggerTest::test_concurrent()
//{
//    QFuture<void> f1 = QtConcurrent::run(log_repetetive, 1);
//    QFuture<void> f2 = QtConcurrent::run(log_repetetive, 2);
//    QFuture<void> f3 = QtConcurrent::run(log_repetetive, 3);
//    QFuture<void> f4 = QtConcurrent::run(log_repetetive, 4);

//    f1.waitForFinished();
//    f2.waitForFinished();
//    f3.waitForFinished();
//    f4.waitForFinished();
//}

void LoggerTest::test_grouplogger()
{
    GroupLogger g;
    FileLogger *f1(new FileLogger("group_test1.log"));
    FileLogger *f2(new FileLogger("group_test2.log"));
    f1->Clear(); f2->Clear();
    g.AddLogger(f1);
    g.AddLogger(f2);

    f1->Log("This is only in log 1");
    f2->Log("This is only in log 2");

    g.Log("This message is in both logs");
}



QTEST_MAIN(LoggerTest);

#include "tst_loggertest.moc"
