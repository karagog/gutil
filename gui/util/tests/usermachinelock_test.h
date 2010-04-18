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

#ifndef USERMACHINELOCK_TEST_H
#define USERMACHINELOCK_TEST_H

#include <QObject>

class usermachinelock_test : public QObject
{
Q_OBJECT
public:
    explicit usermachinelock_test(QObject *parent = 0);

signals:

public slots:

private slots:
    void test_locking();
};

#endif // USERMACHINELOCK_TEST_H
