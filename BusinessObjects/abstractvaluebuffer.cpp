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

#include "abstractvaluebuffer.h"
#include "DataAccess/giodevice.h"
#include "Core/Utils/stringhelpers.h"
#include "Core/exception.h"
//#include "Logging/debuglogger.h"
#include <QStringList>
#include <QtConcurrentRun>
using namespace GUtil;
using namespace DataObjects;
using namespace BusinessObjects;

AbstractValueBuffer::AbstractValueBuffer(
        DataAccess::GIODevice *transport,
        DerivedClassFunctions *dp,
        QObject *parent)
            :QObject(parent),
            _p_AsyncWrite(true),
            _flag_new_outgoing_data_enqueued(false),
            _flag_new_incoming_data_enqueued(false),
            _flag_exiting(false),
            _transport(transport),
            _derived_class_pointer(dp)
{
    start_worker_threads();
}

void AbstractValueBuffer::start_worker_threads()
{
    connect(_transport, SIGNAL(ReadyRead()), this, SLOT(importData()));

    // Start up our background workers
    _ref_worker_outgoing =
            QtConcurrent::run(this, &AbstractValueBuffer::_queue_processor_thread,
                              &_outgoing_flags_mutex,
                              &_condition_outgoing_data_enqueued,
                              &_flag_new_outgoing_data_enqueued,
                              OutQueue);

    _ref_worker_incoming =
            QtConcurrent::run(this, &AbstractValueBuffer::_queue_processor_thread,
                              &_incoming_flags_mutex,
                              &_condition_incoming_data_enqueued,
                              &_flag_new_incoming_data_enqueued,
                              InQueue);
}

AbstractValueBuffer::~AbstractValueBuffer()
{
    // Derived classes must call this first thing in their destructors, but just
    //  in case we still call it here.  But this may cause a seg fault if called
    //  here
    kill_worker_threads();

    delete _transport;
}

void AbstractValueBuffer::kill_worker_threads()
{
    _incoming_flags_mutex.lock();
    _outgoing_flags_mutex.lock();
    {
        _flag_exiting = true;
    }
    _outgoing_flags_mutex.unlock();
    _incoming_flags_mutex.unlock();

    // Wake up our background workers and wait for them to finish
    _condition_outgoing_data_enqueued.wakeOne();
    _condition_incoming_data_enqueued.wakeOne();

    _ref_worker_incoming.waitForFinished();
    _ref_worker_outgoing.waitForFinished();
}

void AbstractValueBuffer::_get_queue_and_mutex(
        QueueTypeEnum qt,
        QQueue<DataObjects::DataTable> **q,
        QMutex **m)
{
    switch(qt)
    {
    case InQueue:
        *m = &_in_queue_mutex;
        *q = &_in_queue;
        break;
    case OutQueue:
        *m = &_out_queue_mutex;
        *q = &_out_queue;
        break;
    default:
        throw Core::NotImplementedException("Unrecognized queue type");
    }
}

void AbstractValueBuffer::clearQueues()
{
    _clear_queue(_in_queue_mutex, _in_queue);
    _clear_queue(_out_queue_mutex, _out_queue);
}

void AbstractValueBuffer::_clear_queue(
        QMutex &lock,
        QQueue<DataObjects::DataTable> &queue)
{
    lock.lock();
    queue.clear();
    lock.unlock();
}

void AbstractValueBuffer::importData()
{
    QByteArray *data;

    try
    {
        // First get the data off the io device
        data = new QByteArray(transport().ReceiveData(false));
    }
    catch(Core::Exception &ex)
    {
        Logging::GlobalLogger::LogException(ex);
        return;
    }


    // Translate, if necessary
    if(_derived_class_pointer)
        _derived_class_pointer->preprocess_incoming_data(*data);


    DataTable tbl;
    try
    {
        // Populate the table with the xml data
        tbl.FromXmlQString(*data);
    }
    catch(Core::XmlException &)
    {
        // If the xml parse fails, then it's corrupt for whatever reason.
        //  we'll ignore it and treat it as a null configuration
        tbl.Clear();

        if(_derived_class_pointer)
            _derived_class_pointer->init_new_table(tbl);
    }

    // Enqueue it
    en_deQueueMessage(InQueue, tbl, true);
    delete data;
}

QUuid AbstractValueBuffer::enQueueCurrentData(bool clear)
{
    QUuid ret;
    DataTable *tmp = 0;

    _cur_data_lock.lock();
    {
        // Tag the table data with a GUID
        _cur_data.SetTableName((ret = QUuid::createUuid()).toString());

        tmp = new DataTable(_cur_data.Clone());
        if(clear)
            _cur_data.Clear();

        // Automatically commit the changes in the table, even if we didn't
        //  just clear it, because that is our policy
        _cur_data.CommitChanges();
    }
    _cur_data_lock.unlock();

    en_deQueueMessage(OutQueue, *tmp, true);

    delete tmp;
    return ret;
}

DataTable AbstractValueBuffer::deQueueMessage(QueueTypeEnum q)
{
    return en_deQueueMessage(q, DataTable(), false);
}

DataTable AbstractValueBuffer::en_deQueueMessage(
        QueueTypeEnum q,
        const DataTable &msg,
        bool enqueue)
{
    DataTable *tmp_tbl;
    QMutex *queue_mutex;
    QQueue<DataTable> *queue;

    bool *flag_new_data;
    QMutex *flags_mutex;
    QWaitCondition *condition_newdata_available;

    try
    {
        _get_queue_and_mutex(q, &queue, &queue_mutex);
    }
    catch(Core::Exception &ex)
    {
        Logging::GlobalLogger::LogException(ex);
        return DataTable();
    }

    // Set the wait condition and the new data flags
    switch(q)
    {
    case InQueue:
        flag_new_data = &_flag_new_incoming_data_enqueued;
        flags_mutex = &_incoming_flags_mutex;
        condition_newdata_available = &_condition_incoming_data_enqueued;
        break;
    case OutQueue:
        flag_new_data = &_flag_new_outgoing_data_enqueued;
        flags_mutex = &_outgoing_flags_mutex;
        condition_newdata_available = &_condition_outgoing_data_enqueued;
        break;
    default:
        THROW_NEW_GUTIL_EXCEPTION(Core::NotImplementedException, "");
    }

    // en/dequeue the data
    queue_mutex->lock();
    {
        if(enqueue)
            queue->enqueue(*(tmp_tbl = new DataTable(msg.Clone())));
        else if(!queue->empty())
            tmp_tbl = new DataTable(queue->dequeue());
    }
    queue_mutex->unlock();

    // Wake up our background worker to the fact that there's new
    //  data available.  If they miss the wakeup because they're already
    //  running, then they will see that _new_***_work_queued has
    //  been set to true, and they will reprocess the appropriate queue.
    if(enqueue)
    {
        flags_mutex->lock();
        {
            *flag_new_data = true;
        }
        flags_mutex->unlock();

        condition_newdata_available->wakeOne();
    }

    // Create a return table on the stack.  We do this because creating a
    //  DataTable using the copy constructor is significantly less time consuming
    //  than creating it with the default constructor.  It is a shared data class
    //  so when copying all it does is set a single pointer, but when creating
    //  from scratch it would allocate a new shared data structure only to
    //  delete it when you use the '=' operator to set the return data
    DataTable ret(*tmp_tbl);
    delete tmp_tbl;

    // If we write synchronously, then we wait here until the data is actually written
    if(!GetAsyncWrite())
        wait_for_message_sent(ret.Name());

    return ret;
}

void AbstractValueBuffer::_flush_queue(QueueTypeEnum qt)
{
    QQueue<DataTable> *queue;
    QMutex *mutex;

    bool queue_has_data(true);

    _get_queue_and_mutex(qt, &queue, &mutex);

    while(queue_has_data)
    {
        DataTable tbl;

        mutex->lock();
        {
            if(queue->count() > 0)
                tbl = queue->dequeue();

            queue_has_data = queue->count() > 0;
        }
        mutex->unlock();

        if(qt == InQueue)
        {
            QByteArray *data;
            try
            {
                data = new QByteArray(transport().ReceiveData(false));
            }
            catch(Core::Exception &ex)
            {
                Logging::GlobalLogger::LogException(ex);
                return;
            }

            if(_derived_class_pointer)
            {
                _derived_class_pointer->preprocess_incoming_data(*data);

                DataTable tbl;
                try
                {
                    tbl.FromXmlQString(*data);
                }
                catch(Core::XmlException &)
                {
                    // Ignore and treat as null configuration
                    tbl.Clear();

                    _derived_class_pointer->init_new_table(tbl);
                }

                _derived_class_pointer->new_input_data_arrived(tbl);
            }

            delete data;
        }
        else if(qt == OutQueue)
        {
            QByteArray data = tbl.ToXmlQString().toAscii();

            if(_derived_class_pointer)
                _derived_class_pointer->preprocess_outgoing_data(data);

            try
            {
                transport().SendData(data);
            }
            catch(Core::Exception &ex)
            {
                Logging::GlobalLogger::LogException(ex);
                // Don't crash on transport errors
                //throw;
            }

            _condition_outgoing_data_sent.wakeAll();
        }
    }
}

void AbstractValueBuffer::_queue_processor_thread(
        QMutex *flags_mutex,
        QWaitCondition *condition_data_ready,
        bool *flag_data_ready,
        int queue_type)
{
    forever
    {
        flags_mutex->lock();
        {
            // Somebody may have queued more data for us while we
            //  were processing the last run.  If so, we skip waiting and
            //  go right to process the queue.
            if(!(*flag_data_ready))
                condition_data_ready->wait(flags_mutex);

            if(_flag_exiting)
            {
                flags_mutex->unlock();
                break;
            }

            // lower the flag
            *flag_data_ready = false;
        }
        flags_mutex->unlock();

        // Process any data in the queue
        _flush_queue((QueueTypeEnum)queue_type);
    }
}

void AbstractValueBuffer::wait_for_message_sent(const QUuid &id)
{
    _out_queue_mutex.lock();
    {
        bool contains(true);

        while(contains)
        {
            contains = false;

            // Figure out if the queue contains the given id
            foreach(DataTable tbl, _out_queue)
            {
                if(id == tbl.Name())
                {
                    contains = true;
                    break;
                }
            }

            if(contains)
                _condition_outgoing_data_sent.wait(&_out_queue_mutex);
        }
    }
    _out_queue_mutex.unlock();
}
