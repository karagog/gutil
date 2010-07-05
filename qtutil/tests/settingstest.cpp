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

#include "settingstest.h"
#include "stringhelpers.h"
#include "filesystemhelpers.h"
#include <string>
#include <QDir>

using namespace GUtil;
using namespace GUtil::QtUtil;

settingsTest::settingsTest(QObject *parent) :
    QObject(parent)
{
    settings = new Settings("GTestLib");
}

settingsTest::~settingsTest()
{
    delete settings;
}

void settingsTest::initTestCase()
{
    if(!settings->IsLoaded())
    {
        qDebug(settings->Error().toStdString().c_str());
        QVERIFY(false);
    }

    qDebug((QString("Settings initialized with file: ") + settings->FileName()).toStdString().c_str());
}

void settingsTest::saving_value()
{
    //qDebug("Adding new key to settings file...");
    if(!settings->SetValue("testkey", "testval"))
    {
        qDebug(settings->Error().toStdString().c_str());
        QVERIFY(false);
    }
}

void settingsTest::reading_same_value()
{
    //create a new settings object and see if it has that key from test case 1
    //qDebug("Reading the value back in...");

    Settings newsettings(settings->Identity());

    //QString probe = settings->Value("testkey");
    QString probe = newsettings.Value("testkey");
    QVERIFY(probe == "testval");
}

void settingsTest::test_no_value()
{
    QVERIFY(settings->Value("novalue.doesn'texist") == "");
}

void settingsTest::null_dat()
{
    settings->SetValue("nulldata", "");

    Settings newsettings(settings->Identity());

    QString probe = newsettings.Value("nulldata");
    QVERIFY(probe == "");
}

void settingsTest::multiple_values()
{
    QString one = "multitest1";
    QString two = "multitest2";
    QMap<QString, QString> vals;
    QStringList keys;

    vals[one] = "fart";
    keys.append(one);
    vals[two] = "poop";
    keys.append(two);

    QVERIFY(settings->SetValues(vals));

    QMap<QString, QString> output = settings->Values(keys);

    QVERIFY(output[one] == settings->Value(one));
    QVERIFY(output[two] == settings->Value(two));

    // Test that a new settings object will have the same values
    Settings newsettings(settings->Identity());
    QVERIFY(settings->Value(one) == newsettings.Value(one));
    QVERIFY(settings->Value(two) == newsettings.Value(two));
}

void settingsTest::test_save_signal()
{
    Settings newsettings(settings->Identity());
    connect(&newsettings, SIGNAL(NotifySaved()), this, SLOT(catch_save_signal_not_test()));

    save_flag = false;
    newsettings.SetValue("test1", "fart");
    QVERIFY(save_flag);

    // Make sure no signal is emitted if no data was changed (it shouldn't
    //  have saved in this situation)
    save_flag = false;
    QMap<QString, QString> temp;
    newsettings.SetValues(temp);
    QVERIFY(!save_flag);

    temp["fart"] = "poop";
    temp["sneeze"] = "boogers";
    newsettings.SetValues(temp);
    QVERIFY(save_flag);
}

void settingsTest::catch_save_signal_not_test()
{
    save_flag = true;
}

void settingsTest::test_reload()
{
    QString one = "reloadtest1";
    Settings newsettings(settings->Identity());

    settings->SetValue(one, "fart");

    // Make sure 'newsettings' doesn't have the new key we just inserted
    QVERIFY(newsettings[one] == "");

    // But after we reload it should be there
    QVERIFY(newsettings.Reload());
    QVERIFY(newsettings[one] == "fart");
}

void settingsTest::test_bin_dat()
{
    char arr[] = {0x00, 0x01, 0x02};
    std::string std(arr, 3);
    QString str = QString::fromStdString(std);

    settings->SetValue("binary", str);

    Settings newset(settings->Identity());
    QVERIFY(newset["binary"] == settings->Value("binary"));
    QVERIFY(newset["binary"] == str);
}

void settingsTest::test_erase_value()
{
    QString tmpkey = "thisisnew";
    QString permkey = "thisstays";
    settings->SetValue(tmpkey, "nothing");
    settings->SetValue(permkey, "value");

    Settings newsettings(settings->Identity());
    QVERIFY(newsettings[tmpkey] == "nothing");
    QVERIFY(newsettings[permkey] == "value");

    // Now erase the value and test if it's still in there
    QVERIFY(settings->Remove(tmpkey));
    QVERIFY(newsettings.Reload());

    QVERIFY(newsettings[tmpkey] == "");
    QVERIFY(newsettings[permkey] == "value");
}

void settingsTest::test_clear_all_values()
{
    QString tmpkey = "thisisnew";
    QString permkey = "thisstays";
    settings->SetValue(tmpkey, "nothing");
    settings->SetValue(permkey, "value");

    Settings newsettings(settings->Identity());
    QVERIFY(newsettings[tmpkey] == "nothing");
    QVERIFY(newsettings[permkey] == "value");

    QVERIFY(settings->Clear());
    QVERIFY(newsettings.Reload());

    QVERIFY(newsettings[tmpkey] == "");
    QVERIFY(newsettings[permkey] == "");
}

void settingsTest::cleanupTestCase()
{
    //QString probe = QString::fromStdString(GUtil::StringHelpers::pathName(settings->FileName().toStdString()));
    QVERIFY(FilesystemHelpers::rmdir(QString::fromStdString(StringHelpers::pathName(settings->FileName().toStdString()))));
}
