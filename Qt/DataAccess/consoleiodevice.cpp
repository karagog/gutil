/*Copyright 2010-2012 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "gutil_extendedexception.h"
#include "gutil_consoleiodevice.h"
#include "gutil_console.h"
#include <stdio.h>
USING_NAMESPACE_GUTIL1(DataAccess);

NAMESPACE_GUTIL2(QT, DataAccess);


QMutex ConsoleIODevice::global_console_mutex;

ConsoleIODevice::ConsoleIODevice(QObject *parent)
    :IODevice(parent)
{
    _engaged = false;

    Engage();
}

ConsoleIODevice::~ConsoleIODevice()
{
    Disengage();
}

void ConsoleIODevice::SetEngaged(bool engage)
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

bool ConsoleIODevice::has_data_available()
{
    bool ret;
    _messages_lock.lock();
    {
        ret = _has_data_available_locked();
    }
    _messages_lock.unlock();
    return ret;
}

bool ConsoleIODevice::_has_data_available_locked() const
{
    return _messages_received.length() > 0;
}

void ConsoleIODevice::run()
{
    QString buf;

    while(true)
    {
        char c = getchar();

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

void ConsoleIODevice::WriteLine(const QString &data)
{
    QString data_copy = data;

    if(data.length() == 0 ||
       data_copy.at(data_copy.length() - 1) != '\n')
        data_copy.append('\n');

    Write(data_copy.toAscii());
}

QString ConsoleIODevice::ReadLine()
{
    return ReceiveData();
}

void ConsoleIODevice::send_data(const QByteArray &d)
{
    _fail_if_not_initialized();

    Console::Write(d);
}

QByteArray ConsoleIODevice::receive_data()
{
    QByteArray ret;
    _messages_lock.lock();
    {
        if(_has_data_available_locked())
            ret = _messages_received.dequeue().toAscii();
    }
    _messages_lock.unlock();
    return ret;
}

void ConsoleIODevice::_fail_if_not_initialized()
{
    if(!_engaged)
        THROW_NEW_GUTIL_EXCEPTION2(DataTransportException,
                                   "The console is already in use");
}


END_NAMESPACE_GUTIL2;
