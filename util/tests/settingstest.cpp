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
#include "exception.h"
//#include "filesystemhelpers.h"
#include <string>
#include <QFile>

using namespace GUtil;
using namespace GQtUtil::DataAccess;

settingsTest::settingsTest(QObject *parent) :
    QObject(parent)
{
    settings = new DA_ConfigFile("GTestLib");
}

settingsTest::~settingsTest()
{
    delete settings;
}

void settingsTest::initTestCase()
{
//    if(!settings->IsLoaded())
//    {
//        qDebug(settings->Error().toStdString().c_str());
//        QVERIFY(false);
//    }

    qDebug((QString("Settings initialized with file: ") + settings->fileName()).toStdString().c_str());
}

void settingsTest::saving_value()
{
    //qDebug("Adding new key to settings file...");
    try
    {
        settings->setValue("testkey", QString("testval").toAscii());
    }
    catch(GUtil::Exception ex)
    {
        qDebug(ex.Message().c_str());
        QVERIFY(false);
    }

    QVERIFY(settings->value("testkey") == "testval");
}

void settingsTest::reading_same_value()
{
    //create a new settings object and see if it has that key from test case 1
    //qDebug("Reading the value back in...");

    DA_ConfigFile newsettings(*settings);

    QString probe = QString::fromAscii(newsettings.value("testkey"));
    QVERIFY(probe == "testval");
}

void settingsTest::test_no_value()
{
    QVERIFY(settings->value("novalue.doesn'texist") == "");
}

void settingsTest::null_dat()
{
    settings->setValue("nulldata", QByteArray());

    DA_ConfigFile newsettings(*settings);

    QString probe = QString::fromAscii(newsettings.value("nulldata"));
    QVERIFY(probe == "");
}

void settingsTest::multiple_values()
{
    QByteArray one("multitest1");
    QByteArray two("multitest2");
    QMap<QString, QByteArray> vals;

    QList<QString> keys;

    vals[one] = QByteArray("fart");
    keys.append(one);
    vals[two] = QByteArray("poop");
    keys.append(two);

    try
    {
        settings->setValues(vals);
    }
    catch(GUtil::Exception)
    {
        QVERIFY(false);
    }

    QMap<QString, QByteArray> output = settings->values(keys);

    QVERIFY(output[one] == settings->value(one));
    QVERIFY(output[two] == settings->value(two));

    // Test that a new settings object will have the same values
    DA_ConfigFile newsettings(*settings);

    QVERIFY(settings->value(one) == newsettings.value(one));
    QVERIFY(settings->value(two) == newsettings.value(two));
}

void settingsTest::test_save_signal()
{
//    QString _identity, _modifier;
//    settings->getIdentity(_identity, _modifier);
//    DA_ConfigFile newsettings(_identity, _modifier);

//    connect(&newsettings, SIGNAL(NotifySaved()), this, SLOT(catch_save_signal_not_test()));

//    save_flag = false;
//    newsettings.setValue("test1", "fart");
//    QVERIFY(save_flag);

//    // Make sure no signal is emitted if no data was changed (it shouldn't
//    //  have saved in this situation)
//    save_flag = false;
//    QMap<QString, QString> temp;
//    newsettings.SetValues(temp);
//    QVERIFY(!save_flag);

//    temp["fart"] = "poop";
//    temp["sneeze"] = "boogers";
//    newsettings.SetValues(temp);
//    QVERIFY(save_flag);
}

void settingsTest::catch_save_signal_not_test()
{
    save_flag = true;
}

void settingsTest::test_reload()
{
    QString one = "reloadtest1";

    DA_ConfigFile newsettings(*settings);

    settings->setValue(one, QByteArray("fart"));

    QVERIFY(newsettings[one] == "");

    newsettings.reload();
    QVERIFY(newsettings[one] == "fart");
}

void settingsTest::test_bin_dat()
{
    char arr[] = {0x00, 0x01, 0x02};
    std::string std(arr, 3);
    QByteArray str = QByteArray(std.c_str(), std.length());

    settings->setValue("binary", str);

    DA_ConfigFile newsettings(*settings);

    QVERIFY(newsettings["binary"] == settings->value("binary"));
    QVERIFY(newsettings["binary"] == str);
}

void settingsTest::test_erase_value()
{
    QString tmpkey = "thisisnew";
    QString permkey = "thisstays";

    settings->setValue(tmpkey, QByteArray("nothing"));
    settings->setValue(permkey, QByteArray("value"));

    DA_ConfigFile newsettings(*settings);

    newsettings.reload();

    QString probe = newsettings[tmpkey];
    probe = settings->value(tmpkey);
    QVERIFY(newsettings[tmpkey] == "nothing");
    QVERIFY(newsettings[permkey] == "value");

    // Now erase the value and test if it's still in there
    try
    {
        settings->remove(tmpkey);
    }
    catch(GUtil::Exception)
    {
        QVERIFY(false);
    }

    newsettings.reload();
    QVERIFY(newsettings[tmpkey] == "");
    QVERIFY(newsettings[permkey] == "value");
}

void settingsTest::test_clear_all_values()
{
    QString tmpkey = "thisisnew";
    QString permkey = "thisstays";

    settings->setValue(tmpkey, "nothing");
    settings->setValue(permkey, "value");

    DA_ConfigFile newsettings(*settings);

    QVERIFY(newsettings[tmpkey] == "nothing");
    QVERIFY(newsettings[permkey] == "value");

    try
    {
        settings->clear();
    }
    catch(GUtil::Exception)
    {
        QVERIFY(false);
    }

    newsettings.reload();
    QVERIFY(newsettings[tmpkey] == "");
    QVERIFY(newsettings[permkey] == "");
}

void settingsTest::cleanupTestCase()
{
    //QString probe = QString::fromStdString(GUtil::StringHelpers::pathName(settings->FileName().toStdString()));
    QFile::remove(QString::fromStdString(StringHelpers::pathName(settings->fileName().toStdString())));
}
