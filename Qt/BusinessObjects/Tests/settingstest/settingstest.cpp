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

#include "BusinessObjects/configfile.h"
#include "Core/Logging/debuglogger.h"
#include "gutil_exception.h"
#include <string>
#include <QFile>
#include <QTest>
USING_NAMESPACE_GUTIL;
USING_NAMESPACE_GUTIL1(DataObjects);
USING_NAMESPACE_GUTIL2(QT, BusinessObjects);

class settingsTest : public QObject
{
    Q_OBJECT
public:
    explicit settingsTest(QObject *parent = 0);

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
    void test_with_compression();

    void cleanupTestCase();


    void catch_save_signal_not_test();

private:
    Utils::SmartPointer<ConfigFile> settings;

    bool save_flag;
};

settingsTest::settingsTest(QObject *parent)
    :QObject(parent),
      settings(new ConfigFile("GTestLib", "", this))
{}

void settingsTest::initTestCase()
{
//    if(!settings->IsLoaded())
//    {
//        qDebug(settings->Error().toStdString().c_str());
//        QVERIFY(false);
//    }

    settings->Initialize();
    settings->Clear();
    qDebug((QString("Settings initialized with file: %1")
            .arg(settings->FileName())).toStdString().c_str());
    //qDebug(QString("%1 items to begin with").arg(settings->Values().count()).toStdString().c_str());
}

void settingsTest::saving_value()
{
    //qDebug("Adding new key to settings file...");
    try
    {
        settings->SetValue("testkey", QString("testval").toAscii());

        QVERIFY(settings->Value("testkey") == "testval");
    }
    catch(const Exception<> &ex)
    {
        dLogException(ex);
        QFAIL("Unhandled exception");
    }
}

void settingsTest::reading_same_value()
{
    try
    {
        //create a new settings object and see if it has that key from test case 1
        //qDebug("Reading the value back in...");

        ConfigFile newsettings(*settings);
        newsettings.Initialize();

        QString probe = newsettings.Value("testkey").toString();
        QVERIFY2(probe == "testval", probe.toUtf8());
    }
    catch(const Exception<> &ex)
    {
        dLogException(ex);
        QFAIL("Unhandled exception");
    }
}

void settingsTest::test_no_value()
{
    try
    {
        QVERIFY(settings->Value("novalue.doesn'texist").isNull());
    }
    catch(const Exception<> &ex)
    {
        dLogException(ex);
        QFAIL(QString("Unhandled exception: %1").arg(ex.What).toUtf8());
    }
}

void settingsTest::null_dat()
{
    try
    {
        settings->SetValue("nulldata", QByteArray());

        ConfigFile newsettings(*settings);
        newsettings.Initialize();

        QString probe = newsettings.Value("nulldata").toString();
        QVERIFY(probe.isEmpty());
    }
    catch(const Exception<> &ex)
    {
        dLogException(ex);
        QFAIL(QString("Unhandled exception: %1").arg(ex.What).toUtf8());
    }
}

void settingsTest::multiple_values()
{
    try
    {
        String one("multitest1");
        String two("multitest2");
        Map<String, QVariant> vals;

        StringList keys;

        vals[one] = "fart";
        keys.Append(one);
        vals[two] = "poop";
        keys.Append(two);

        try
        {
            settings->SetValues(vals);
        }
        catch(const Exception<> &)
        {
            QVERIFY(false);
        }

        Map<String, QVariant> output = settings->Values(keys);

        QVERIFY(output[one] == settings->Value(one));
        QVERIFY(output[two] == settings->Value(two));

        // Test that a new settings object will have the same values
        ConfigFile newsettings(*settings);
        newsettings.Initialize();

        QVERIFY(settings->Value(one) == newsettings.Value(one));
        QVERIFY(settings->Value(two) == newsettings.Value(two));
    }
    catch(const Exception<> &ex)
    {
        dLogException(ex);
        QFAIL("Unhandled exception");
    }
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
    try
    {
        String one = "reloadtest1";

        ConfigFile newsettings(*settings);
        newsettings.Initialize();

        settings->SetValue(one, QByteArray("fart"));

        QVERIFY(newsettings.Value(one).isNull());

        newsettings.Reload();
        QVERIFY(newsettings.Value(one) == "fart");
    }
    catch(const Exception<> &ex)
    {
        dLogException(ex);
        QFAIL("Unhandled exception");
    }
}

void settingsTest::test_bin_dat()
{
    try
    {
        char arr[] = {0x00, 0x01, 0x02};
        std::string std(arr, 3);
        QByteArray str = QByteArray(std.c_str(), std.length());

        settings->SetValue("binary", str);

        ConfigFile newsettings(*settings);
        newsettings.Initialize();

        QVERIFY(newsettings.Value("binary") == settings->Value("binary"));
        QVERIFY(newsettings.Value("binary") == str);
    }
    catch(const Exception<> &ex)
    {
        dLogException(ex);
        QFAIL("Unhandled exception");
    }
}

void settingsTest::test_erase_value()
{
    try
    {
        String tmpkey = "thisisnew";
        String permkey = "thisstays";

        settings->SetValue(tmpkey, QByteArray("nothing"));
        settings->SetValue(permkey, QByteArray("value"));

        ConfigFile newsettings(*settings);
        newsettings.Initialize();

        QString probe = newsettings.Value(tmpkey).toString();
        probe = settings->Value(tmpkey).toString();
        QVERIFY(newsettings.Value(tmpkey) == "nothing");
        QVERIFY(newsettings.Value(permkey) == "value");

        // Now erase the value and test if it's still in there
        try
        {
            settings->RemoveValue(tmpkey);
        }
        catch(const Exception<> &)
        {
            QVERIFY(false);
        }

        newsettings.Reload();
        QVERIFY2(newsettings.Value(tmpkey).isNull(),
                 newsettings.Value(tmpkey).toString().toStdString().c_str());
        QVERIFY(newsettings.Value(permkey) == "value");
    }
    catch(const Exception<> &ex)
    {
        dLogException(ex);
        QFAIL("Unhandled exception");
    }
}

void settingsTest::test_clear_all_values()
{
    try
    {
        String tmpkey = "thisisnew";
        String permkey = "thisstays";

        settings->SetValue(tmpkey, "nothing");
        settings->SetValue(permkey, "value");

        ConfigFile newsettings(*settings);
        newsettings.Initialize();

        QVERIFY(newsettings.Value(tmpkey) == "nothing");
        QVERIFY(newsettings.Value(permkey) == "value");

        try
        {
            settings->Clear();
        }
        catch(const Exception<> &)
        {
            QVERIFY(false);
        }

        newsettings.Reload();
        QVERIFY2(newsettings.Value(tmpkey).isNull(),
                 newsettings.Value(tmpkey).toString().toStdString().c_str());
        QVERIFY(newsettings.Value(permkey).isNull());
    }
    catch(const Exception<> &ex)
    {
        dLogException(ex);
        QFAIL("Unhandled exception");
    }
}

void settingsTest::test_with_compression()
{
    // newsettings is human readable
    ConfigFile newsettings(*settings);
    newsettings.Initialize();

    // But now settings is not
    settings->SetIsHumanReadable(false);

    // We write a compressed file
    settings->SetValue("test", "value");

    // Then try to read it with the "Human readable" settings
    newsettings.Reload();
    QVERIFY(newsettings.Value("test") == "value");
}

void settingsTest::cleanupTestCase()
{
    //QString probe = QString::fromStdString(GUtil::StringHelpers::pathName(settings->FileName().toStdString()));
    QFile::remove(QString::fromStdString(settings->FileName().toStdString()));
}

QTEST_MAIN(settingsTest);

#include "settingstest.moc"
