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

#ifndef STRINGS_TEST_H
#define STRINGS_TEST_H

#include <QObject>
#include <QTest>

class strings_test : public QObject
{
Q_OBJECT
public:
    explicit strings_test(QObject *parent = 0);

signals:

public slots:

private slots:
    void test_pathname();
    void test_basename();

    void test_hex();

    void test_base64();

    void test_base16();

    //void test_chars();

};

#endif // STRINGS_TEST_H
