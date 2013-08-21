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
#include "DataAccess/gconsoleiodevice.h"
using namespace GUtil::DataAccess;

// A simple program that repeats everything you give it on stdin out to stdout
int main(int argc, char *argv[])
{
//    QCoreApplication a(argc, argv);

//    GConsoleIODevice *ct = new GConsoleIODevice(&a);
//    a.connect(ct, SIGNAL(ReadyRead()), ct, SLOT(WriteLine(QByteArray)));

//    return a.exec();

    GConsoleIODevice c;
    while(true)
    {
        QString tmp;
        c>>tmp;

        if(tmp == "quit")
            break;

        c<<tmp<<"\n";
    }
}
