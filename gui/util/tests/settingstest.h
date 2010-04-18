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

#ifndef SETTINGSTEST_H
#define SETTINGSTEST_H

#include "settings.h"
#include <QObject>
#include <QTest>

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
    GUtil::QtUtil::Settings *settings;

    bool save_flag;
};

#endif // SETTINGSTEST_H
