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

#include "gconsoleiodevice.h"
#include <cstdio>
using namespace GUtil;
using namespace std;

QMutex DataAccess::GConsoleIODevice::global_console_mutex;

DataAccess::GConsoleIODevice::GConsoleIODevice(QObject *parent)
    :GIODevice(parent)
{
    _engaged = false;

    Engage();
}

DataAccess::GConsoleIODevice::~GConsoleIODevice()
{
    Disengage();
}

void DataAccess::GConsoleIODevice::SetEngaged(bool engage)
{
    if(engage)
    {
        if(_engaged)
            return;

        _engaged = global_console_mutex.tryLock();
        start();
    }
    else
    {
        if(!_engaged)
            return;

        terminate();
        wait();

        _engaged = false;
        global_console_mutex.unlock();
    }
}

bool DataAccess::GConsoleIODevice::HasDataAvailable() const
{
    return _messages_received.length() > 0;
}

void DataAccess::GConsoleIODevice::run()
{
    QString buf;

    while(true)
    {
        char c = (char)getchar();

        if(c == '\n')
        {
            _messages_lock.lock();
            {
                _messages_received.enqueue(buf);
            }
            _messages_lock.unlock();

            raiseReadyRead();
            buf.clear();
        }
        else if(c == (char)8)   // erase if the user pressed backspace
            buf.chop(1);
        else
            buf.append(c);
    }
}

void DataAccess::GConsoleIODevice::WriteLine(const QString &data)
{
    QString data_copy = data;

    if(data.length() == 0 ||
       data_copy.at(data_copy.length() - 1) != '\n')
        data_copy.append('\n');

    Write(data_copy.toAscii());
}

QString DataAccess::GConsoleIODevice::ReadLine(bool block)
{
    return ReceiveData(block);
}

void DataAccess::GConsoleIODevice::send_data(const QByteArray &d)
        throw(GUtil::Core::DataTransportException)
{
    _fail_if_not_initialized();

    printf(QString(d).toStdString().c_str());
}

QByteArray DataAccess::GConsoleIODevice::receive_data()
        throw(GUtil::Core::DataTransportException)
{
    QByteArray ret;
    _messages_lock.lock();
    {
        if(HasDataAvailable())
            ret = _messages_received.dequeue().toAscii();
    }
    _messages_lock.unlock();
    return ret;
}

void DataAccess::GConsoleIODevice::_fail_if_not_initialized()
{
    if(!_engaged)
        THROW_NEW_GUTIL_EXCEPTION( Core::DataTransportException, "The console is already in use" );
}
