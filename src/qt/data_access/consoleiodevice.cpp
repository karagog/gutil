/*Copyright 2010-2013 George Karagoulis

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
#include "gutil_atomic.h"
#include <stdio.h>
USING_NAMESPACE_GUTIL1(DataAccess);
USING_NAMESPACE_GUTIL1(Utils);

NAMESPACE_GUTIL2(QT, DataAccess);


// This is used to guarantee that only one ConsoleIODevice per process can be used
static AtomicInt __global_console_lock(1);

static QMutex _messages_lock;
static QQueue<QString> _messages_received;

ConsoleIODevice::ConsoleIODevice(QObject *parent)
    :IODevice(parent)
{
    if(__global_console_lock.Decrement())
        THROW_NEW_GUTIL_EXCEPTION2(DataTransportException, 
            "You have already instantiated one of these");
}

ConsoleIODevice::~ConsoleIODevice()
{
    __global_console_lock.Increment();
}

bool ConsoleIODevice::has_data_available()
{
    bool ret;
    _messages_lock.lock();
    {
        ret = _messages_received.length() > 0;
    }
    _messages_lock.unlock();
    return ret;
}

bool ConsoleIODevice::_has_data_available_locked() const
{
    return ;
}

void ConsoleIODevice::run()
{
    QString buf;

    while(true)
    {
        // The call to getchar() blocks until data has been read
        char c = getchar();

        // If we detect an end line
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

        // erase if the user pressed backspace
        else if(c == (char)8)
            buf.chop(1);
            
        // If it's not a special character then append it to the buffer
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
        if(_messages_received.length() > 0)
            ret = _messages_received.dequeue().toAscii();
    }
    _messages_lock.unlock();
    return ret;
}


END_NAMESPACE_GUTIL2;
