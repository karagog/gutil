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

#include "filelogger.h"
#include "consolelogger.h"
#include "pubsubsystem.h"
#include <QtCore/QString>
#include <QtTest/QtTest>
using namespace GQtUtil::Utils;

class LoggerTest : public QObject
{
    Q_OBJECT

public:
    LoggerTest();

private Q_SLOTS:
    void test_normal_logging();
};

LoggerTest::LoggerTest()
{
}

void LoggerTest::test_normal_logging()
{
    ConsoleLogger clog;
    clog.LogMessage("Hello");
}

QTEST_APPLESS_MAIN(LoggerTest);

#include "tst_loggertest.moc"
