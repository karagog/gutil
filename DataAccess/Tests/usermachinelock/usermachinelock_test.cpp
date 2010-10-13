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

#include "DataAccess/DA_UserMachineLock.h"
#include "Core/exception.h"
#include <QTest>
using namespace GUtil;

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

usermachinelock_test::usermachinelock_test(QObject *parent) :
    QObject(parent)
{
}

void usermachinelock_test::test_locking()
{
    DataAccess::DA_UserMachineLock a("GTestLib");
    DataAccess::DA_UserMachineLock b("GTestLib");

    QString probe = a.fileName();

    try
    {
        a.lock();
    }
    catch(Core::Exception)
    {
        QVERIFY(false);
    }

    // This doesn't work unless you do it from another instance
    //QVERIFY(!b.Lock());
}

QTEST_APPLESS_MAIN(usermachinelock_test);

#include "usermachinelock_test.moc"
