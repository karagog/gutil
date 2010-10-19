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
#include "Custom/datacontainer.h"
#include "DataAccess/abstractdatatransportmechanism.h"
#include "Core/Utils/stringhelpers.h"
#include "Core/exception.h"
#include <QStringList>
#include <QtConcurrentRun>
using namespace GUtil;

BusinessObjects::AbstractValueBuffer::AbstractValueBuffer(
        DataAccess::AbstractDataTransportMechanism *transport,
        QObject *parent)
            :QObject(parent),
            Core::Interfaces::IReadOnlyObject(false),
            Core::Interfaces::IXmlSerializable(false)
{
    current_data = new Custom::DataContainer();

    _transport = transport;
    connect(transport, SIGNAL(notifyNewData(QByteArray)), this, SLOT(importData(QByteArray)));
}

BusinessObjects::AbstractValueBuffer::~AbstractValueBuffer()
{
    delete current_data;
    delete _transport;
}

DataAccess::AbstractDataTransportMechanism &BusinessObjects::AbstractValueBuffer::Transport() const
{
    return *_transport;
}

void BusinessObjects::AbstractValueBuffer::ValueChanged_protected() throw(Core::Exception)
{
    // Do nothing by default
}

bool BusinessObjects::AbstractValueBuffer::SetValue(const QString &key, const QByteArray& value)
{
    QMap<QString, QByteArray> m;
    m.insert(key, value);
    return SetValues(m);
}

bool BusinessObjects::AbstractValueBuffer::SetValues(const QMap<QString, QByteArray> &values)
{
    FailIfReadOnly();

    current_data_lock.lockForWrite();
    foreach(QString s, values.keys())
        current_data->insert(s, values[s]);
    current_data_lock.unlock();

    return ValueChanged();
}

bool BusinessObjects::AbstractValueBuffer::ValueChanged()
{
    try
    {
        ValueChanged_protected();
    }
    catch(Core::Exception &ex)
    {
        LogException(ex);
        return false;
    }

    return true;
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

QByteArray BusinessObjects::AbstractValueBuffer::Value(const QString &key)
{
    return Values(QStringList(key)).value(key);
}

QMap<QString, QByteArray> BusinessObjects::AbstractValueBuffer::Values(const QStringList &keys)
{
    current_data_lock.lockForRead();

    QMap<QString, QByteArray> ret;
    foreach(QString s, keys)
        ret[s] = current_data->value(s);

    current_data_lock.unlock();

    return ret;
}

bool BusinessObjects::AbstractValueBuffer::Contains(const QString &key)
{
    current_data_lock.lockForRead();

    bool ret = current_data->contains(key);

    current_data_lock.unlock();

    return ret;
}

void BusinessObjects::AbstractValueBuffer::Clear()
{
    FailIfReadOnly();

    current_data_lock.lockForWrite();
    current_data->clear();
    current_data_lock.unlock();

    ValueChanged();
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

bool BusinessObjects::AbstractValueBuffer::RemoveValue(const QString &key)
{
    QStringList sl;
    sl.append(key);
    return RemoveValue(sl);
}

bool BusinessObjects::AbstractValueBuffer::RemoveValue(const QStringList &keys)
{
    FailIfReadOnly();

    current_data_lock.lockForWrite();

    foreach(QString s, keys)
        current_data->remove(s);

    current_data_lock.unlock();

    return ValueChanged();
}

void BusinessObjects::AbstractValueBuffer::importData(const QByteArray &dat)
{
    enQueueMessage(InQueue, dat);
}

void BusinessObjects::AbstractValueBuffer::enQueueMessage(QueueTypeEnum q, const QByteArray &msg)
{
    en_deQueueMessage(q, msg, true);
}

void BusinessObjects::AbstractValueBuffer::enQueueCurrentData(bool clear)
{
    QString data;

    if(clear)
        current_data_lock.lockForWrite();
    else
        current_data_lock.lockForRead();

    // Critical section for current data
    try
    {
        data = QString::fromStdString(current_data->ToXml());

        if(clear)
            current_data->clear();
    }
    catch(Core::Exception &ex)
    {
        current_data_lock.unlock();

        LogException(ex);
        return;
    }

    current_data_lock.unlock();

    enQueueMessage(OutQueue, data.toAscii());
}

void BusinessObjects::AbstractValueBuffer::process_input_data(const QByteArray &data)
{
    current_data_lock.lockForWrite();
    try
    {
        current_data->FromXml(QString(data).toStdString());
    }
    catch(Core::Exception &ex)
    {
        current_data_lock.unlock();

        LogException(ex);
        return;
    }

    current_data_lock.unlock();
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
        LogException(ex);
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
        process_queues();

    return ret;
}

void BusinessObjects::AbstractValueBuffer::process_queues()
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
        LogException(ex);
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
                Transport().SendData(ba);
        }
    }
}

std::string BusinessObjects::AbstractValueBuffer::ToXml()
{
    current_data_lock.lockForRead();
    std::string ret = current_data->ToXml();
    current_data_lock.unlock();
    return ret;
}

void BusinessObjects::AbstractValueBuffer::FromXml(const std::string &xml) throw(Core::XmlException)
{
    enQueueMessage(InQueue, QByteArray(xml.c_str(), xml.length()));
}

void BusinessObjects::AbstractValueBuffer::SetXmlHumanReadableFormat(bool h)
{
    Core::Interfaces::IXmlSerializable::SetXmlHumanReadableFormat(h);

    current_data->SetXmlHumanReadableFormat(h);
}

std::string BusinessObjects::AbstractValueBuffer::ReadonlyMessageIdentifier() const
{
    return "DataAccess::AbstractValueBuffer";
}
