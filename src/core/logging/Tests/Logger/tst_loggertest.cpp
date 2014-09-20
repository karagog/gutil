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
#include "gutil_queuedlogger.h"
#include <QtCore/QString>
#include <QtTest/QtTest>
#include <thread>
using namespace std;
USING_NAMESPACE_GUTIL;

class LoggerTest : public QObject
{
    Q_OBJECT
signals:
    void notify_message(const QString &, const QString &);

private Q_SLOTS:
    void test_normal_logging();
    void test_exception_logging();
    void test_queuedlogger();
    void test_global_logging();
    void test_grouplogger();

private:
    void _test_abstract_logger(ILog &);
    void _test_logger_exception(ILog &);
};

void LoggerTest::_test_abstract_logger(ILog &l)
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
        // Pretty cool syntax, eh?
        throw XmlException<true>("May the force be with you",
                                 DataTransportException<true>{{"OMG", "Nested exceptions!"}},
        {{"firstkey", "boo!"},
         {"secondkey", "what?"},
         {"thirdkey", "this works"},
         {"fourthkey", ""}});
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
    // There is no way to test if we have set a global logger or not
    GlobalLogger().LogInfo("This does not get logged anywhere, because there is no global logger");

    // Check setting
    FileLogger *flog = new FileLogger("global.log");
    flog->Clear();
    SetGlobalLogger(flog);
    GlobalLogger().LogInfo("This is logged to the global logger");

    // The true logger type is obscured; This is an opaque utility.
    QVERIFY(NULL == dynamic_cast<FileLogger*>(&GlobalLogger()));
    QVERIFY(NULL == dynamic_cast<QueuedLogger*>(&GlobalLogger()));

    try{
        throw Exception<true>("This is a test of the global logger's exception handling");
    } catch(exception &ex) {
        GlobalLogger().LogException(ex);
    }

    // Clear the global logger so we don't have a memory leak
    SetGlobalLogger(NULL);
}

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

static void __log_repetitive(ILog *l, int thread_no)
{
    const int repetitions = 1000;
    //qDebug("Thread %d started", thread_no);
    const String title = String::Format("Thread Number %d", thread_no);
    for(int i = 0; i < repetitions; ++i)
        l->LogInfo("This is a test", title);
    //qDebug("Thread %d finished", thread_no);
}

void LoggerTest::test_queuedlogger()
{
    const int no_threads = 50;
    vector<std::thread *> threads;
    FileLogger *flog = new FileLogger("queued.log");
    flog->Clear();
    QueuedLogger log(flog);

    // Create a bunch of threads to pound the queued logger
    qDebug("Starting %d background threads...", no_threads);
    for(int i = 0; i < no_threads; ++i)
        threads.push_back(new std::thread(&__log_repetitive, &log, i + 1));

    // Let all the threads finish
    qDebug("Waiting for the threads to finish...");
    for(auto t : threads){
        t->join();
        delete t;
    }

    qDebug("Done logging, now waiting for the logger to finish...");
}



QTEST_MAIN(LoggerTest);

#include "tst_loggertest.moc"
