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

#include "exception_test.h"
#include "exception.h"
#include <QTest>
using namespace GUtil;

exception_test::exception_test(QObject *parent) :
        QObject(parent)
{
}

void exception_test::test_works()
{
    bool flag = false;
    try
    {
        throw GUtil::Exception();
    }
    catch(GUtil::Exception ex)
    {
        QVERIFY(ex.Message() == "");
        flag = true;
    }
    catch(std::exception)
    {}
    QVERIFY(flag);
}

void exception_test::test_message()
{
    try
    {
        throw GUtil::Exception("message");
    }
    catch(GUtil::Exception ex)
    {
        QVERIFY(ex.Message() == "message");
    }
}
