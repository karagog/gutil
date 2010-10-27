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
#include "DataAccess/consoletransport.h"
#include "receiver_T.h"
using namespace GUtil::DataAccess;

ConsoleTransport *ct;

receiver_t receiver;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    ct = new ConsoleTransport(&a);

    a.connect(ct, SIGNAL(notifyNewData(QByteArray)),
              &receiver, SLOT(user_entered_data(QByteArray)));

    return a.exec();
}