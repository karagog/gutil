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
        QObject *parent)
            :QObject(parent),
            _flag_new_outgoing_data_enqueued(false),
            _flag_new_incoming_data_enqueued(false),
            _flag_exiting(false),
            _transport(transport)
{
    _init_abstract_value_buffer();
}

AbstractValueBuffer::AbstractValueBuffer(
        DataAccess::GIODevice *transport,
        const DataObjects::DataTable &dt,
        QObject *parent)
            :QObject(parent),
            cur_outgoing_data(dt),
            _flag_new_outgoing_data_enqueued(false),
            _flag_new_incoming_data_enqueued(false),
            _flag_exiting(false),
            _transport(transport)
{
    _init_abstract_value_buffer();
}

void AbstractValueBuffer::_init_abstract_value_buffer()
{
    connect(_transport, SIGNAL(ReadyRead()), this, SLOT(importData()));

    // Start up our background workers
    _ref_worker_outgoing =
            QtConcurrent::run(this, &AbstractValueBuffer::_worker_outgoing);

    _ref_worker_incoming =
            QtConcurrent::run(this, &AbstractValueBuffer::_worker_incoming);
}

AbstractValueBuffer::~AbstractValueBuffer()
{
    _incoming_flags_mutex.lock();
    _outgoing_flags_mutex.lock();
    {
        _flag_exiting = true;
    }
    _outgoing_flags_mutex.unlock();
    _incoming_flags_mutex.unlock();

    // Wake up our background workers and wait for them to finish
    _condition_data_sent.wakeOne();
    _condition_data_arrived.wakeOne();

    _ref_worker_incoming.waitForFinished();
    _ref_worker_outgoing.waitForFinished();

    delete _transport;
}

void AbstractValueBuffer::_get_queue_and_mutex(
        QueueTypeEnum qt,
        QQueue< QPair<QUuid, QByteArray> > **q,
        QMutex **m)
{
    switch(qt)
    {
    case InQueue:
        *m = &_in_queue_mutex;
        *q = &in_queue;
        break;
    case OutQueue:
        *m = &_out_queue_mutex;
        *q = &out_queue;
        break;
    default:
        throw Core::NotImplementedException("Unrecognized queue type");
    }
}

void AbstractValueBuffer::clearQueues()
{
    _clear_queue(_in_queue_mutex, in_queue);
    _clear_queue(_out_queue_mutex, out_queue);
}

void AbstractValueBuffer::_clear_queue(QMutex &lock,
                                                        QQueue< QPair<QUuid, QByteArray> > &queue)
{
    lock.lock();
    queue.clear();
    lock.unlock();
}

void AbstractValueBuffer::importData()
{
    QString data;

    try
    {
        data = import_incoming_data();
    }
    catch(Core::Exception &ex)
    {
        Logging::GlobalLogger::LogException(ex);
        return;
    }

    enQueueMessage(InQueue, data.toAscii());
}

QUuid AbstractValueBuffer::enQueueMessage(QueueTypeEnum q,
                                          const QByteArray &msg)
{
    return en_deQueueMessage(q, msg, true).first;
}

QUuid AbstractValueBuffer::enQueueCurrentData(bool clear)
{
    QString data;

    try
    {
        _outgoing_flags_mutex.lock();
        {
            data = get_current_data();

            if(clear)
                cur_outgoing_data.Clear();
        }
        _outgoing_flags_mutex.unlock();
    }
    catch(Core::Exception &ex)
    {
        // I want to log the exception before yielding control
        Logging::GlobalLogger::LogException(ex);
        _outgoing_flags_mutex.unlock();
        return QUuid();
    }

    return enQueueMessage(OutQueue, data.toAscii());
}

QByteArray AbstractValueBuffer::get_current_data(bool hr) const
{
    return cur_outgoing_data.ToXmlQString(hr).toAscii();
}

QString AbstractValueBuffer::import_incoming_data()
        throw(Core::Exception)
{
    return QString(transport().ReceiveData(false));
}

void AbstractValueBuffer::process_input_data(
        const QPair<QUuid, QByteArray> &data)
{
    try
    {
        cur_incoming_data.FromXmlQString(QString(data.second));
    }
    catch(Core::Exception &ex)
    {
        Logging::GlobalLogger::LogException(ex);
    }
}

QByteArray AbstractValueBuffer::deQueueMessage(QueueTypeEnum q)
{
    return en_deQueueMessage(q, QByteArray(), false).second;
}

QPair<QUuid, QByteArray> AbstractValueBuffer::en_deQueueMessage(
        QueueTypeEnum q,
        const QByteArray &msg,
        bool enqueue)
{
    QPair<QUuid, QByteArray> ret;
    QMutex *m;
    QQueue< QPair<QUuid, QByteArray> > *tmpq;

    bool *bln_new_data_flag;
    QMutex *data_mutex;
    QWaitCondition *wc;

    try
    {
        _get_queue_and_mutex(q, &tmpq, &m);
    }
    catch(Core::Exception &ex)
    {
        Logging::GlobalLogger::LogException(ex);
        return ret;
    }

    // Set the wait condition and the missed wake-up flags
    switch(q)
    {
    case InQueue:
        bln_new_data_flag = &_flag_new_incoming_data_enqueued;
        data_mutex = &_incoming_flags_mutex;
        wc = &_condition_data_arrived;
        break;
    case OutQueue:
        bln_new_data_flag = &_flag_new_outgoing_data_enqueued;
        data_mutex = &_outgoing_flags_mutex;
        wc = &_condition_data_sent;
        break;
    default:
        THROW_NEW_GUTIL_EXCEPTION(Core::NotImplementedException, "");
    }


    // Stamp each outgoing message with a GUID so we can refer to it later
    if(enqueue)
        ret = QPair<QUuid, QByteArray>(QUuid::createUuid(), msg);

    m->lock();
    {
        if(enqueue)
            tmpq->enqueue(ret);
        else if(!tmpq->empty())
            ret = tmpq->dequeue();
    }
    m->unlock();

    // Wake up our background worker to the fact that there's new
    //  data available.  If they miss the wakeup because they're already
    //  running, then they will see that _new_***_work_queued has
    //  been set to true, and they will reprocess the appropriate queue.
    if(enqueue)
    {
        data_mutex->lock();
        {
            *bln_new_data_flag = true;
        }
        data_mutex->unlock();

        wc->wakeOne();
    }

    return ret;
}

void AbstractValueBuffer::_flush_queue(QueueTypeEnum qt)
{
    QQueue< QPair<QUuid, QByteArray> > *queue;
    QMutex *mutex;

    bool queue_has_data(true);

    _get_queue_and_mutex(qt, &queue, &mutex);

    while(queue_has_data)
    {
        QPair<QUuid, QByteArray> item;

        mutex->lock();
        {
            if(queue->count() > 0)
                item = queue->dequeue();

            queue_has_data = queue->count() > 0;
        }
        mutex->unlock();

        if(!item.second.isNull())
        {
            if(qt == InQueue)
                process_input_data(item);
            else if (qt == OutQueue)
            {
                try
                {
                    transport().SendData(item.second);
                }
                catch(Core::Exception &ex)
                {
                    Logging::GlobalLogger::LogException(ex);
                    // Don't crash on transport errors
                    //throw;
                }
            }
        }
    }
}

void AbstractValueBuffer::WriteXml(QXmlStreamWriter &sw) const
{
    cur_outgoing_data.WriteXml(sw);
}

void AbstractValueBuffer::ReadXml(QXmlStreamReader &sr)
        throw(Core::XmlException)
{
    DataTable tmp;
    tmp.ReadXml(sr);
    enQueueMessage(InQueue, tmp.ToXmlQString().toAscii());
}

void AbstractValueBuffer::_worker_outgoing()
{
    forever
    {
        _outgoing_flags_mutex.lock();
        {
            // There are flags that need to be protected by the
            //  critical section, and whoever else modifies them
            //  needs to grab the lock
            if(_flag_exiting)
            {
                _outgoing_flags_mutex.unlock();
                break;
            }

            // Somebody may have queued more data for us while we
            //   were processing the last run.  Maybe we picked it up,
            //   or maybe we exited before sending it.  Better be safe
            //   and reprocess the queue, so we skip the wait here.
            if(!_flag_new_outgoing_data_enqueued)
                // Wait for someone to tell us to send data
                _condition_data_sent.wait(&_outgoing_flags_mutex);

            // lower the flag
            _flag_new_outgoing_data_enqueued = false;
        }
        _outgoing_flags_mutex.unlock();

        // Then send it
        _flush_queue(OutQueue);
    }
}

void AbstractValueBuffer::_worker_incoming()
{
    forever
    {
        _incoming_flags_mutex.lock();
        {
            if(_flag_exiting)
            {
                _incoming_flags_mutex.unlock();
                break;
            }

            if(!_flag_new_incoming_data_enqueued)
                _condition_data_arrived.wait(&_incoming_flags_mutex);

            _flag_new_incoming_data_enqueued = false;
        }
        _incoming_flags_mutex.unlock();

        _flush_queue(InQueue);
    }
}
