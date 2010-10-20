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
#include <QProcess>
#include <QObject>
#include "DataAccess/consoletransport.h"
#include "DataAccess/processtransport.h"
using namespace GUtil::DataAccess;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    ConsoleTransport *ct = new ConsoleTransport(&a);

    if(argc == 1)
    {
        ProcessTransport *pt = new ProcessTransport(&a);
        pt->Process().start(QString(argv[0]), QStringList("-n"));

        a.connect(ct, SIGNAL(notifyNewData(QByteArray)), pt, SLOT(Write(QByteArray)));
        a.connect(pt, SIGNAL(notifyNewData(QByteArray)), ct, SLOT(Write(QByteArray)));
    }
    else
    {
        a.connect(ct, SIGNAL(notifyNewData(QByteArray)), ct, SLOT(SendData(QByteArray)));
    }

    return a.exec();
}
