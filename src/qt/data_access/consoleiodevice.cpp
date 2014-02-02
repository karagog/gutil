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
#include <QThread>
USING_NAMESPACE_GUTIL1(DataAccess);
USING_NAMESPACE_GUTIL1(Utils);

NAMESPACE_GUTIL2(QT, DataAccess);


struct __console_io_data_t
{
    static QMutex receive_lock;
    static QQueue<QString> receive_queue;

    static QMutex send_lock;
    static QWaitCondition send_condition;
    static QQueue<QByteArray> send_queue;
    static bool cancel_send;

    static bool initialized;

} __console_io_data;

QMutex __console_io_data_t::receive_lock;
QMutex __console_io_data_t::send_lock;
QWaitCondition __console_io_data_t::send_condition;
QQueue<QString> __console_io_data_t::receive_queue;
QQueue<QByteArray> __console_io_data_t::send_queue;
bool __console_io_data_t::cancel_send(false);
bool __console_io_data_t::initialized(false);



void ConsoleIODevice::_send_worker()
{
    __console_io_data.send_lock.lock();

    // We use a do-while here to guarantee that the loop body will execute
    //  at least once, which guarantees that the send queue is flushed
    //  even if the thread gets cancelled before it starts.
    do
    {
        // Wait here for data to be queued for sending
        while(__console_io_data.send_queue.isEmpty() &&
              !__console_io_data.cancel_send)
        {
            __console_io_data.send_condition.wait(&__console_io_data.send_lock);
        }

        // Always empty the queue before we wait or cancel
        while(!__console_io_data.send_queue.isEmpty())
        {
            // Dequeue some data from the send queue and relinquish the lock while we write
            QByteArray b( __console_io_data.send_queue.dequeue() );
            __console_io_data.send_lock.unlock();

            // Write the data to console (slow operation)
            Console::Write(b.constData());

            __console_io_data.send_lock.lock();
        }
    }
    while(!__console_io_data.cancel_send);

    __console_io_data.cancel_send = false;
    __console_io_data.send_lock.unlock();
}


class _receive_worker : public QThread
{
    Q_OBJECT
signals:
    void new_line();
protected:
    virtual void run()
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

                emit new_line();
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
};

// We have to include this since the above QObject is defined in the source file
#include "consoleiodevice.moc"



ConsoleIODevice::ConsoleIODevice(QObject *parent)
    :IODevice(parent)
{
    if(__console_io_data.initialized)
    {
        THROW_NEW_GUTIL_EXCEPTION2(DataTransportException,
                                   "You have already instantiated one of these. "
                                   " This class was intended only to be instantiated once in"
                                   " an application, and never destructed until the app closes."
                                   );
    }
    else
    {
        // Once we set this to 'initialized', it is never unset.  Thus preventing
        //  another instantiation within this program.  We do not allow this because
        //  in the destructor we must terminate the receiver thread to break the
        //  blocking call to getchar().  Since we terminate the thread it could stop
        //  at any point in the thread's execution, even when it's modifying the
        //  static data objects.  Therefore we can't guarantee the state of those
        //  variables after the first usage of this class, so we prevent you from
        //  instantiating multiples.
        __console_io_data.initialized = true;
    }

    // Start the send/receive workers
    m_sendWorker = QtConcurrent::run(this, &ConsoleIODevice::_send_worker);

    _receive_worker *rw = new _receive_worker;
    m_receiveWorker = rw;
    connect(rw, SIGNAL(new_line()), this, SLOT(raiseReadyRead()));
    rw->start();
}

ConsoleIODevice::~ConsoleIODevice()
{
    // Terminate the receive worker (we are harsher with the receiver,
    //  because it uses a blocking call to getchar() that requires us
    //  to terminate)
    _receive_worker *rw = reinterpret_cast<_receive_worker*>(m_receiveWorker);
    rw->terminate();

    // Cancel the send worker
    __console_io_data.send_lock.lock();
    __console_io_data.cancel_send = true;
    __console_io_data.send_lock.unlock();
    __console_io_data.send_condition.wakeOne();

    // Wait for everyone to finish
    m_sendWorker.waitForFinished();
    rw->wait();
    delete rw;
}

bool ConsoleIODevice::has_data_available()
{
    bool ret;
    __console_io_data.receive_lock.lock();
    {
        ret = !__console_io_data.receive_queue.isEmpty();
    }
    __console_io_data.receive_lock.unlock();
    return ret;
}

void ConsoleIODevice::WriteLine(QString data)
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
