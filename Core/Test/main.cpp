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

#include "strings_test.h"
#include "crypto_test.h"
#include "exception_test.h"
#include <QTest>

int main()
{
    int ret = QTest::qExec(new strings_test());
    if(ret != 0)
        return ret;

    ret = QTest::qExec(new crypto_test());
    if(ret != 0)
        return ret;

    try
    {
        ret = QTest::qExec(new exception_test());
        if(ret != 0)
            return ret;
    }
    catch(std::exception)
    {
        return false;
    }

    return ret;
}
