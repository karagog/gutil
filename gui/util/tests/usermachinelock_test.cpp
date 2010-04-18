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

#include "usermachinelock_test.h"
#include "usermachinelock.h"
#include <QTest>
using namespace GUtil::QtUtil;

usermachinelock_test::usermachinelock_test(QObject *parent) :
    QObject(parent)
{
}

void usermachinelock_test::test_locking()
{
    UserMachineLock a("GTestLib");
    UserMachineLock b("GTestLib");

    QString probe = a.FileName();

    QVERIFY(a.Lock());

    // This doesn't work unless you do it from another instance
    //QVERIFY(!b.Lock());
}
