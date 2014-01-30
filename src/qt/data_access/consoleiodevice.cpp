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
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>
#include <QtConcurrentRun>
USING_NAMESPACE_GUTIL1(DataAccess);
USING_NAMESPACE_GUTIL1(Utils);

NAMESPACE_GUTIL2(QT, DataAccess);


struct __console_io_data_t
{
    static QMutex global_lock;

    static QMutex receive_lock;
    static QQueue<QString> receive_queue;

    static QMutex send_lock;
    static QWaitCondition send_condition;
    static QQueue<QByteArray> send_queue;

    static bool initialized;

} __console_io_data;

QMutex __console_io_data_t::global_lock;
QMutex __console_io_data_t::receive_lock;
QMutex __console_io_data_t::send_lock;
QWaitCondition __console_io_data_t::send_condition;
QQueue<QString> __console_io_data_t::receive_queue;
QQueue<QByteArray> __console_io_data_t::send_queue;
bool __console_io_data_t::initialized(false);



void ConsoleIODevice::_send_worker()
{
    while(true)
    {
        // Wait here for data to be queued for sending
        __console_io_data.send_lock.lock();
        while(__console_io_data.send_queue.isEmpty())
            __console_io_data.send_condition.wait(&__console_io_data.send_lock);

        // Dequeue some data from the send queue and relinquish the lock while we write
        QByteArray b( __console_io_data.send_queue.dequeue() );
        __console_io_data.send_lock.unlock();

        // Write the data to console:
        Console::Write(b.constData());
    }
}

void ConsoleIODevice::_receive_worker()
{
    QString buf;

    while(true)
    {
        // The call to getchar() blocks until data has been read
        char c = getchar();

        // If we detect an end line
        if(c == '\n')
        {
            __console_io_data.receive_lock.lock();
            {
                __console_io_data.receive_queue.enqueue(buf);
            }
            __console_io_data.receive_lock.unlock();

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




ConsoleIODevice::ConsoleIODevice(QObject *parent)
    :IODevice(parent)
{
    bool initialized;
    __console_io_data.global_lock.lock();
    {
        if(!(initialized = __console_io_data.initialized))
        {
            // Once we set this to 'initialized', it is never unset.  Thus preventing
            //  another instantiation within this program.
            __console_io_data.initialized = true;
        }
    }
    __console_io_data.global_lock.unlock();

    if(initialized)
        THROW_NEW_GUTIL_EXCEPTION2(DataTransportException, 
                                   "You have already instantiated one of these. "
                                   " This class was intended only to be instantiated once in"
                                   " an application, and never destructed until the app closes."
                                   );

    // Start the send/receive workers
    QtConcurrent::run(this, &ConsoleIODevice::_send_worker);
    QtConcurrent::run(this, &ConsoleIODevice::_receive_worker);
}

ConsoleIODevice::~ConsoleIODevice()
{}

bool ConsoleIODevice::has_data_available()
{
    bool ret;
    __console_io_data.receive_lock.lock();
    {
        ret = __console_io_data.receive_queue.length() > 0;
    }
    __console_io_data.receive_lock.unlock();
    return ret;
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
    __console_io_data.send_lock.lock();
    {
        __console_io_data.send_queue.enqueue(d);
    }
    __console_io_data.send_lock.unlock();
    __console_io_data.send_condition.wakeOne();
}

QByteArray ConsoleIODevice::receive_data()
{
    QByteArray ret;
    __console_io_data.receive_lock.lock();
    {
        if(!__console_io_data.receive_queue.isEmpty())
            ret = __console_io_data.receive_queue.dequeue().toAscii();
    }
    __console_io_data.receive_lock.unlock();
    return ret;
}


END_NAMESPACE_GUTIL2;
