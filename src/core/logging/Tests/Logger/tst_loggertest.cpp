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

#include "gutil_filelogger.h"
#include "gutil_consolelogger.h"
#include "gutil_globallogger.h"
#include "gutil_grouplogger.h"
#include "gutil_extendedexception.h"
#include <QtConcurrentRun>
#include <QtCore/QString>
#include <QtTest/QtTest>
using namespace std;
USING_NAMESPACE_GUTIL;

class LoggerTest : public QObject
{
    Q_OBJECT
signals:
    void notify_message(const QString &, const QString &);

protected:
    static void log_repetetive(int id);

private Q_SLOTS:
    void test_normal_logging();
    void test_exception_logging();

    void test_global_logging();
    //void test_concurrent();

    void test_grouplogger();

private:
    void _test_abstract_logger(AbstractLogger &);
    void _test_logger_exception(ILog &);
};

void LoggerTest::log_repetetive(int id)
{
    QVERIFY(false);
//    for(int i = 0; i < 10; i++)
//        GlobalLogger::LogMessage(String::FromInt(id),
//                                 String::Format("Concurrent message #%d", i));
}

void LoggerTest::_test_abstract_logger(AbstractLogger &l)
{
    l.LogInfo("This is a message", "Hello world");
    l.LogError("Bar", "Foo");

    l.SetLogLevel(ILog::LogLevelWarning);
    l.LogInfo("Shouldn't see this info");

    l.SetLogLevel(ILog::LogLevelError);
    l.LogInfo("Shouldn't see this info");
    l.LogInfo("Shouldn't see this warning");

    l.LogError("You should see this error and the following exception");
    l.LogException(std::exception());
}

void LoggerTest::test_normal_logging()
{
    ConsoleLogger clog;
    _test_abstract_logger(clog);

    FileLogger flog("test_logging.log");
    flog.Clear();
    _test_abstract_logger(flog);
}

void LoggerTest::_test_logger_exception(ILog &l)
{
    try
    {
        XmlException<true> ex("May the force be with you");
        ex.Data["firstkey"] = "boo!";
        ex.Data["secondkey"] = "what?";
        ex.Data["thirdkey"] = "ERROR";
        ex.Data["thirdkey"] = "this works";
        ex.Data["fourthkey"] = "";
        THROW_GUTIL_EXCEPTION(ex);
    }
    catch(const exception &ex)
    {
        l.LogException(ex);
    }
}

void LoggerTest::test_exception_logging()
{
    ConsoleLogger clog;
    _test_logger_exception(clog);

    FileLogger flog("test_exceptions.log");
    flog.Clear();
    _test_logger_exception(flog);
}

void LoggerTest::test_global_logging()
{

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
    FileLogger *f1(new FileLogger("group_test1.log"));
    FileLogger *f2(new FileLogger("group_test2.log"));
    FileLogger *f3(new FileLogger("group_test3.log"));
    GroupLogger g(2, f1, f2);
    f1->Clear(), f2->Clear(), f3->Clear();

    f1->LogInfo("This is only in log 1");
    f2->LogInfo("This is only in log 2");
    f3->LogInfo("This is only in log 3");

    g.LogInfo("This message is in both logs 1 and 2");

    g.AddLogger(f3);
    g.LogInfo("This message is in all three logs");
}



QTEST_MAIN(LoggerTest);

#include "tst_loggertest.moc"
