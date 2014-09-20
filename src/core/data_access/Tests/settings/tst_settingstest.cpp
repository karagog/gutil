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

#include <QString>
#include <QtTest>
#include "gutil_settings.h"
#include "gutil_file.h"
#include "gutil_globallogger.h"
#include "gutil_consolelogger.h"
USING_NAMESPACE_GUTIL;
using namespace std;

class SettingsTest : public QObject
{
    Q_OBJECT
public:
    SettingsTest();
    ~SettingsTest();
private Q_SLOTS:
    void test_basics();
};

#define SETTINGS_FILENAME "settingstest.txt"

SettingsTest::SettingsTest()
{
    SetGlobalLogger(new ConsoleLogger);
    if(File::Exists(SETTINGS_FILENAME))
        File::Delete(SETTINGS_FILENAME);
}

SettingsTest::~SettingsTest()
{
    SetGlobalLogger(NULL);
}

void SettingsTest::test_basics()
{
    // Start with an empty settings
    {
        Settings s(SETTINGS_FILENAME);
        QVERIFY(s.Keys().Length() == 0);
        QVERIFY(!s.IsDirty());

        // Add a value
        s.SetValue("first", "value1");
        QVERIFY(s.IsDirty());
        QVERIFY(s.Keys().Length() == 1);
        QVERIFY(s.GetData("first") == "value1");

        // Add another value
        s.SetValue("second", "value2");
        QVERIFY(s.IsDirty());
        QVERIFY(s.Keys().Length() == 2);
        QVERIFY(s.GetData("first") == "value1");
        QVERIFY(s.GetData("second") == "value2");

        s.CommitChanges();
        QVERIFY(!s.IsDirty());
        QVERIFY(s.Keys().Length() == 2);
        QVERIFY(s.GetData("first") == "value1");
        QVERIFY(s.GetData("second") == "value2");

        // Check that AllData works
        auto alldata = s.AllData();
        QVERIFY(alldata["first"] == "value1");
        QVERIFY(alldata["second"] == "value2");
    }

    {
        // Check that a new one reads the same values
        Settings s2(SETTINGS_FILENAME);
        QVERIFY(!s2.IsDirty());
        QVERIFY(s2.Keys().Length() == 2);
        QVERIFY(s2.GetData("first") == "value1");
        QVERIFY(s2.GetData("second") == "value2");
    }
}

QTEST_APPLESS_MAIN(SettingsTest)

#include "tst_settingstest.moc"
