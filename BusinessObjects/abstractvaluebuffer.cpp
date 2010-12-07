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
#include <QStringList>
#include <QtConcurrentRun>
using namespace GUtil;
using namespace DataObjects;

BusinessObjects::AbstractValueBuffer::AbstractValueBuffer(
        DataAccess::GIODevice *transport,
        QObject *parent)
            :QObject(parent),
            _transport(transport)
{
    _init_abstract_value_buffer();
}

BusinessObjects::AbstractValueBuffer::AbstractValueBuffer(
        DataAccess::GIODevice *transport,
        const DataObjects::DataTable &dt,
        QObject *parent)
            :QObject(parent),
            _transport(transport),
            cur_outgoing_data(dt)
{
    _init_abstract_value_buffer();
}

void BusinessObjects::AbstractValueBuffer::_init_abstract_value_buffer()
{
    connect(_transport, SIGNAL(ReadyRead()), this, SLOT(importData()));
}

BusinessObjects::AbstractValueBuffer::~AbstractValueBuffer()
{
    delete _transport;
}

void BusinessObjects::AbstractValueBuffer::_get_queue_and_mutex(QueueTypeEnum qt,
                                                           QQueue<QByteArray> **q,
                                                           QMutex **m)
        throw(Core::Exception)
{
    switch(qt)
    {
    case InQueue:
        *m = &in_queue_mutex;
        *q = &in_queue;
        break;
    case OutQueue:
        *m = &out_queue_mutex;
        *q = &out_queue;
        break;
    default:
        throw Core::NotImplementedException("Unrecognized queue type");
    }
}

void BusinessObjects::AbstractValueBuffer::clearQueues()
{
    _clear_queue(in_queue_mutex, in_queue);
    _clear_queue(out_queue_mutex, out_queue);
}

void BusinessObjects::AbstractValueBuffer::_clear_queue(QMutex &lock, QQueue< QByteArray > &queue)
{
    lock.lock();
    queue.clear();
    lock.unlock();
}

void BusinessObjects::AbstractValueBuffer::importData()
{
    QString data;

    try
    {
        data = import_current_data();
    }
    catch(Core::Exception &ex)
    {
        Logging::GlobalLogger::LogException(ex);
        return;
    }

    enQueueMessage(InQueue, data.toAscii());
}

void BusinessObjects::AbstractValueBuffer::enQueueMessage(QueueTypeEnum q, const QByteArray &msg)
{
    en_deQueueMessage(q, msg, true);
}

void BusinessObjects::AbstractValueBuffer::enQueueCurrentData(bool clear)
{
    QString data;

    // Critical section for current data
    try
    {
        data = get_current_data();

        if(clear)
            cur_outgoing_data.Clear();
    }
    catch(Core::Exception &ex)
    {
        Logging::GlobalLogger::LogException(ex);
        return;
    }

    enQueueMessage(OutQueue, data.toAscii());
}

QByteArray BusinessObjects::AbstractValueBuffer::get_current_data(bool hr) const
{
    return cur_outgoing_data.ToXmlQString(hr).toAscii();
}

QString BusinessObjects::AbstractValueBuffer::import_current_data()
{
    return QString(transport().ReceiveData(false));
}

void BusinessObjects::AbstractValueBuffer::process_input_data(const QByteArray &data)
{
    try
    {
        cur_incoming_data.FromXmlQString(QString(data));
    }
    catch(Core::Exception &ex)
    {
        Logging::GlobalLogger::LogException(ex);
        return;
    }
}

QByteArray BusinessObjects::AbstractValueBuffer::deQueueMessage(QueueTypeEnum q)
{
    return en_deQueueMessage(q, QByteArray(), false);
}

QByteArray BusinessObjects::AbstractValueBuffer::en_deQueueMessage(QueueTypeEnum q,
                                                              const QByteArray &msg,
                                                              bool enqueue)
{
    QByteArray ret;
    QMutex *m;
    QQueue<QByteArray> *tmpq;

    try
    {
        _get_queue_and_mutex(q, &tmpq, &m);
    }
    catch(Core::Exception &ex)
    {
        Logging::GlobalLogger::LogException(ex);
        return ret;
    }

    m->lock();
    {
        // Critical section
        if(enqueue)
            tmpq->enqueue(msg);
        else if(!tmpq->empty())
            ret = tmpq->dequeue();
    }
    m->unlock();

    // We want to get rid of anything in the queue as soon as possible
    if(enqueue)
        _process_queues();

    return ret;
}

void BusinessObjects::AbstractValueBuffer::_process_queues()
{
    // Flush the queues
    _flush_queue(OutQueue);
    _flush_queue(InQueue);
}

void BusinessObjects::AbstractValueBuffer::_flush_queue(QueueTypeEnum qt)
{
    QQueue<QByteArray> *queue;
    QMutex *mutex;

    try
    {
        _get_queue_and_mutex(qt, &queue, &mutex);
    }
    catch(Core::Exception &ex)
    {
        Logging::GlobalLogger::LogException(ex);
        return;
    }

    bool queue_has_data = true;

    while(queue_has_data)
    {
        QByteArray ba;

        mutex->lock();

        if(queue->count() > 0)
            ba = queue->dequeue();
        queue_has_data = queue->count() > 0;

        mutex->unlock();

        if(!ba.isNull())
        {
            if(qt == InQueue)
                process_input_data(ba);
            else if (qt == OutQueue)
                transport().SendData(ba);
        }
    }
}

void BusinessObjects::AbstractValueBuffer::WriteXml(QXmlStreamWriter &sw) const
{
    cur_outgoing_data.WriteXml(sw);
}

void BusinessObjects::AbstractValueBuffer::ReadXml(QXmlStreamReader &sr)
        throw(Core::XmlException)
{
    DataTable tmp;
    tmp.ReadXml(sr);
    enQueueMessage(InQueue, tmp.ToXmlQString().toAscii());
}

std::string BusinessObjects::AbstractValueBuffer::ReadonlyMessageIdentifier() const
{
    return "DataAccess::AbstractValueBuffer";
}
