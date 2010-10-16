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

#include "Core/Tools/stringhelpers.h"
#include "DataAccess/ConfigFile.h"
#include "Utils/filelogger.h"
#include "Core/exception.h"
#include <string>
#include <QFile>
#include <QTest>
using namespace GUtil;

class settingsTest : public QObject
{
    Q_OBJECT
public:
    explicit settingsTest(QObject *parent = 0);
    ~settingsTest();

signals:

public slots:

private slots:
    void initTestCase();

    void saving_value();
    void reading_same_value();
    void null_dat();
    void test_no_value();
    void multiple_values();
    void test_save_signal();
    void test_reload();
    void test_bin_dat();
    void test_erase_value();
    void test_clear_all_values();

    void cleanupTestCase();


    void catch_save_signal_not_test();

private:
    DataAccess::ConfigFile *settings;

    bool save_flag;
};

settingsTest::settingsTest(QObject *parent) :
    QObject(parent)
{
    Utils::FileLogger *l = new Utils::FileLogger("SettingsTest.log", 0, this);
    l->ClearLog();
    settings = new DataAccess::ConfigFile("GTestLib", "", l, this);
    settings->Clear();
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
        settings->SetValue("testkey", QString("testval").toAscii());
    }
    catch(Core::Exception &ex)
    {
        qDebug(ex.Message().c_str());
        QVERIFY(false);
    }

    QVERIFY(settings->Value("testkey") == "testval");
}

void settingsTest::reading_same_value()
{
    //create a new settings object and see if it has that key from test case 1
    //qDebug("Reading the value back in...");

    DataAccess::ConfigFile newsettings(*settings);

    QString probe = QString::fromAscii(newsettings.Value("testkey"));
    QVERIFY(probe == "testval");
}

void settingsTest::test_no_value()
{
    QVERIFY(settings->Value("novalue.doesn'texist") == "");
}

void settingsTest::null_dat()
{
    settings->SetValue("nulldata", QByteArray());

    DataAccess::ConfigFile newsettings(*settings);

    QString probe = QString::fromAscii(newsettings.Value("nulldata"));
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
        settings->SetValues(vals);
    }
    catch(Core::Exception &)
    {
        QVERIFY(false);
    }

    QMap<QString, QByteArray> output = settings->Values(keys);

    QVERIFY(output[one] == settings->Value(one));
    QVERIFY(output[two] == settings->Value(two));

    // Test that a new settings object will have the same values
    DataAccess::ConfigFile newsettings(*settings);

    QVERIFY(settings->Value(one) == newsettings.Value(one));
    QVERIFY(settings->Value(two) == newsettings.Value(two));
}

void settingsTest::test_save_signal()
{
//    QString _identity, _modifier;
//    settings->getIdentity(_identity, _modifier);
//    ConfigFile newsettings(_identity, _modifier);

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

    DataAccess::ConfigFile newsettings(*settings);

    settings->SetValue(one, QByteArray("fart"));

    //QVERIFY(newsettings.value(one) == "");

    //newsettings.();
    QVERIFY(newsettings.Value(one) == "fart");
}

void settingsTest::test_bin_dat()
{
    char arr[] = {0x00, 0x01, 0x02};
    std::string std(arr, 3);
    QByteArray str = QByteArray(std.c_str(), std.length());

    settings->SetValue("binary", str);

    DataAccess::ConfigFile newsettings(*settings);

    QVERIFY(newsettings.Value("binary") == settings->Value("binary"));
    QVERIFY(newsettings.Value("binary") == str);
}

void settingsTest::test_erase_value()
{
    QString tmpkey = "thisisnew";
    QString permkey = "thisstays";

    settings->SetValue(tmpkey, QByteArray("nothing"));
    settings->SetValue(permkey, QByteArray("value"));

    DataAccess::ConfigFile newsettings(*settings);

    QString probe = newsettings.Value(tmpkey);
    probe = settings->Value(tmpkey);
    QVERIFY(newsettings.Value(tmpkey) == "nothing");
    QVERIFY(newsettings.Value(permkey) == "value");

    // Now erase the value and test if it's still in there
    try
    {
        settings->RemoveValue(tmpkey);
    }
    catch(Core::Exception &)
    {
        QVERIFY(false);
    }

    //newsettings.reload();
    QVERIFY(newsettings.Value(tmpkey) == "");
    QVERIFY(newsettings.Value(permkey) == "value");
}

void settingsTest::test_clear_all_values()
{
    QString tmpkey = "thisisnew";
    QString permkey = "thisstays";

    settings->SetValue(tmpkey, "nothing");
    settings->SetValue(permkey, "value");

    DataAccess::ConfigFile newsettings(*settings);

    QVERIFY(newsettings.Value(tmpkey) == "nothing");
    QVERIFY(newsettings.Value(permkey) == "value");

    try
    {
        settings->Clear();
    }
    catch(Core::Exception &)
    {
        QVERIFY(false);
    }

    //newsettings.reload();
    QVERIFY(newsettings.Value(tmpkey) == "");
    QVERIFY(newsettings.Value(permkey) == "");
}

void settingsTest::cleanupTestCase()
{
    //QString probe = QString::fromStdString(GUtil::StringHelpers::pathName(settings->FileName().toStdString()));
    QFile::remove(QString::fromStdString(Core::Tools::StringHelpers::pathName(settings->fileName().toStdString())));
}

QTEST_APPLESS_MAIN(settingsTest);

#include "settingstest.moc"
