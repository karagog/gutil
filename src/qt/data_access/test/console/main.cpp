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

#include <QtCore/QCoreApplication>
#include "gutil_consoleiodevice.h"
USING_NAMESPACE_GUTIL2(QT, DataAccess);

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    ConsoleIODevice cio;

    for(int i = 0; i < 5; ++i)
        cio.WriteLine(QString("Counting:  %1").arg(i));

    // Even though we write this at the end, the console io device is
    //  writing on a background thread, so this may appear before the
    //  counting text.  Very cool!
    printf("Finished Counting\n");

    return 0;
}
