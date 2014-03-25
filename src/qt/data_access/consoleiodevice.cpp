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
#include <QFuture>
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>
#include <QtConcurrentRun>
#include <QThread>
USING_NAMESPACE_GUTIL;

NAMESPACE_GUTIL1(QT);


// This is the data that is shared between the class and the background threads.
//  It must be protected by the appropriate locks.
struct __console_io_data_t
{
    // Data for the receiver thread
    static QMutex receive_lock;
    static QWaitCondition received_condition;
    static QQueue<QString> receive_queue;


    // Data for the sender thread
    static QMutex send_lock;
    static QQueue<QPair<GUINT64, QByteArray> > send_queue;
    static GUINT64 sent_id;
    static bool cancel_send;

    // Wait condition tells the thread when there is new data to send
    static QWaitCondition send_condition;

    // Wait condition tells class consumers when their data has been written
    static QWaitCondition sent_condition;

} __console_io_data;

QMutex __console_io_data_t::receive_lock;
QMutex __console_io_data_t::send_lock;
QWaitCondition __console_io_data_t::received_condition;
QWaitCondition __console_io_data_t::send_condition;
QWaitCondition __console_io_data_t::sent_condition;
QQueue<QString> __console_io_data_t::receive_queue;
QQueue<QPair<GUINT64, QByteArray> > __console_io_data_t::send_queue;
bool __console_io_data_t::cancel_send(false);
GUINT64 __console_io_data_t::sent_id(0);


static void __send_worker()
{
    __console_io_data.send_lock.lock();

    // Initialize static variables here
    __console_io_data.sent_id = 0;

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
            QPair<GUINT64, QByteArray> p( __console_io_data.send_queue.dequeue() );
            __console_io_data.send_lock.unlock();

            // Write the data to console (slow operation)
            Console::Write(p.second.constData());

            __console_io_data.send_lock.lock();
            __console_io_data.sent_id = p.first;
            __console_io_data.sent_condition.wakeAll();
        }
    }
    while(!__console_io_data.cancel_send);

    GASSERT(__console_io_data.send_queue.isEmpty());

    // Reset static variables for the next user
    __console_io_data.cancel_send = false;
    __console_io_data.send_lock.unlock();
}


class __receive_worker : public QThread
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
                // Once we grab the lock we cannot be terminated, so anything
                //  in this critical section between the squigglies is safe.
                //  This is how we guarantee that data does not get corrupted.
                __console_io_data.receive_lock.lock();
                {
                    __console_io_data.receive_queue.enqueue(buf);
                    __console_io_data.received_condition.wakeAll();

                    // This goes in the critical section to prevent us from potentially
                    //  terminating while executing someone's slot.  Normally slots execute
                    //  on the receiver's thread, but the user can override it to do
                    //  a direct connection and execute on the caller's thread.  In
                    //  either case you don't know what the receiver is doing so you don't
                    //  want this thread to terminate while holding locks or modifying
                    //  important data.
                    emit new_line();
                }

                // Once we unlock we can be safely terminated
                __console_io_data.receive_lock.unlock();

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


// Static class data to really hide the implementation.  This data is unprotected by locks.
struct __class_data_t
{
    static SmartPointer<__receive_worker> receive_worker;
    static QFuture<void> send_worker;
    static GUINT64 send_count;
    static bool initialized;

} __class_data;

SmartPointer<__receive_worker> __class_data_t::receive_worker;
QFuture<void> __class_data_t::send_worker;
GUINT64 __class_data_t::send_count(0);
bool __class_data_t::initialized(false);


ConsoleIODevice::ConsoleIODevice(bool aw, QObject *parent)
    :IODevice(parent),
      _p_AsynchronousWrite(aw)
{
    if(__class_data.initialized)
        THROW_NEW_GUTIL_EXCEPTION2(DataTransportException,
                                   "You have already instantiated one of these. "
                                   " This class was intended only to be instantiated once in"
                                   " an application."
                                   );

    __class_data.initialized = true;
    __class_data.send_count = 0;

    // Start the send/receive workers
    __class_data.send_worker = QtConcurrent::run(&__send_worker);

    __class_data.receive_worker = new __receive_worker;
    connect(__class_data.receive_worker, SIGNAL(new_line()), this, SLOT(raiseReadyRead()));
    __class_data.receive_worker->start();
}

ConsoleIODevice::~ConsoleIODevice()
{
    // Cancel the send worker
    __console_io_data.send_lock.lock();
    __console_io_data.cancel_send = true;
    __console_io_data.send_lock.unlock();
    __console_io_data.send_condition.wakeOne();


    // Terminate the receive worker (we are harsher with the receiver,
    //  because it uses a blocking call to getchar() that requires us
    //  to murder it, while we can simply ask the sender worker to quit)

    // We must hold the lock to terminate, thereby guaranteeing that no important data
    //  is corrupted.
    __console_io_data.receive_lock.lock();
    __class_data.receive_worker->terminate();
    __class_data.receive_worker->wait();
    __console_io_data.receive_queue.clear();
    __console_io_data.receive_lock.unlock();
    __class_data.receive_worker.Clear();

    // Wait for everyone to finish
    __class_data.send_worker.waitForFinished();

    // Now we allow another instantiation of the class
    __class_data.initialized = false;
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

GUINT64 ConsoleIODevice::WriteLine(QString data)
{
    QString data_copy = data;

    if(data.length() == 0 ||
       data_copy.at(data_copy.length() - 1) != '\n')
        data_copy.append('\n');

    Write(data_copy.toAscii());
    return __class_data.send_count;
}

QString ConsoleIODevice::ReadLine()
{
    return ReceiveData();
}

bool ConsoleIODevice::WaitForReadyRead(unsigned long wait_time) const
{
    bool ret = true;
    __console_io_data.receive_lock.lock();
    if(__console_io_data.receive_queue.isEmpty())
        ret = __console_io_data.received_condition.wait(&__console_io_data.receive_lock, wait_time);
    __console_io_data.receive_lock.unlock();
    return ret;
}

// This helper function can only be used if you already hold the send_lock
static void __wait_for_line_written(GUINT64 id)
{
    while(__console_io_data.sent_id < id)
        __console_io_data.sent_condition.wait(&__console_io_data.send_lock);
}

void ConsoleIODevice::WaitForLineWritten(GUINT64 id) const
{
    __console_io_data.send_lock.lock();
    __wait_for_line_written(id);
    __console_io_data.send_lock.unlock();
}

void ConsoleIODevice::send_data(const QByteArray &d)
{
    ++__class_data.send_count;
    __console_io_data.send_lock.lock();
    {
        __console_io_data.send_queue.enqueue(QPair<GUINT64, QByteArray>(__class_data.send_count, d));
        __console_io_data.send_condition.wakeOne();

        if(!GetAsynchronousWrite())
            __wait_for_line_written(__class_data.send_count);
    }
    __console_io_data.send_lock.unlock();
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


END_NAMESPACE_GUTIL1;
