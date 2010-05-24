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
#include "filesystem_test.h"
#include "usermachinelock_test.h"
#include "stringhelpers.h"
#include "xmlhelpers_test.h"
#include "flattreemodeltest.h"
#include "file_manager_test.h"
#include "exception.h"
#include <QApplication>
using namespace std;


int main(int argc, char **argv)
{
    int res;

    QApplication app(argc, argv);
    {
        settingsTest test;
        res = QTest::qExec(&test);
        if(res != 0)
            return res;
    }

    {
        filesystem_test test;
        res = QTest::qExec(&test);
        if(res != 0)
            return res;
    }

    {
        res = QTest::qExec(new usermachinelock_test());
        if(res != 0)
            return res;
    }

    {
        XmlHelpers_test test;
        res = QTest::qExec(&test);
        if(res != 0)
            return res;
    }

    {
        file_manager_test fmt;
        try
        {
            res = QTest::qExec(&fmt);
        }
        catch(GUtil::Exception ex)
        {
            qDebug(ex.Message().c_str());
        }

        if(res != 0)
            return res;
    }

    flatTreeModelTest *tmt = new flatTreeModelTest(0);
    tmt->show();

    // This is a test to see if I'm able to use gutil functions
    //  while linking with the shared version of GQtUtil
    string probe = GUtil::StringHelpers::baseName("/home/testing/yay/");

    return app.exec();
}
