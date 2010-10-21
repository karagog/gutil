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

#include "consoletransport.h"
#include <iostream>
#include <QWaitCondition>
using namespace GUtil;
using namespace std;

QMutex DataAccess::ConsoleTransport::console_mutex;

DataAccess::ConsoleTransport::ConsoleTransport(QObject *parent) :
    DataAccess::StreamTransport(&cin, &cout, parent)
{
    _initialized = console_mutex.tryLock();
}

DataAccess::ConsoleTransport::~ConsoleTransport()
{
    if(_initialized)
        console_mutex.unlock();
}

void DataAccess::ConsoleTransport::WriteLine(const QByteArray &data)
{
    WriteLine(QString(data));
}

void DataAccess::ConsoleTransport::WriteLine(const QString &data)
{
    QString data_copy = data;

    if(data.length() == 0 ||
       data_copy.at(data_copy.length() - 1) != '\n')
        data_copy.append('\n');

    Write(data_copy.toAscii());
}

void DataAccess::ConsoleTransport::send_data(const QByteArray &d)
        throw(GUtil::Core::DataTransportException)
{
    _fail_if_not_initialized();

    StreamTransport::send_data(d);
}

QByteArray DataAccess::ConsoleTransport::receive_data()
        throw(GUtil::Core::DataTransportException,
              GUtil::Core::EndOfFileException)
{
    _fail_if_not_initialized();

    return StreamTransport::receive_data();
}

void DataAccess::ConsoleTransport::_fail_if_not_initialized()
{
    if(!_initialized)
        throw Core::DataTransportException("The console is already in use");
}
