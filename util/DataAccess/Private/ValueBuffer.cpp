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

#include "ValueBuffer.h"
#include "Interfaces/ITransportMechanism.h"
#include "Core/stringhelpers.h"
#include "Core/exception.h"
#include <QStringList>
#include <QtConcurrentRun>
using namespace GUtil;
using namespace GQtUtil::DataObjects;
using namespace GQtUtil::DataAccess::Private;

ValueBuffer::ValueBuffer(
        Interfaces::ITransportMechanism *transport_mechanism,
        QObject *parent)
            :QObject(parent)
{
    _transport = transport_mechanism;
    _readonly = false;

    connect(_transport, SIGNAL(notifyNewData(QByteArray)), this, SLOT(importData(QByteArray)));
}

//void ValueBuffer::enQueue(bool copy)
//{
//    queue_lock.lockForWrite();
//    _enQueue(copy);
//    queue_lock.unlock();
//}

//void ValueBuffer::_enQueue(bool copy)
//{
//    if(copy)
//        _values.enqueue(new DataObjects::DataContainer(*currentDataContainer()));
//    else
//        _values.enqueue(new DataObjects::DataContainer());
//}

//void ValueBuffer::deQueue()
//{
//    queue_lock.lockForWrite();
//    _deQueue();
//    queue_lock.unlock();
//}

//void ValueBuffer::_deQueue()
//{
//    _values.dequeue();
//}

void ValueBuffer::makeReadOnly(bool val)
{
    _readonly = val;
}

bool ValueBuffer::isReadOnly()
{
    return _readonly;
}

void ValueBuffer::setValue(const QString &key, const QByteArray& value)
{
    QMap<QString, QByteArray> m;
    m.insert(key, value);
    setValues(m);
}

void ValueBuffer::setValues(const QMap<QString, QByteArray> &values)
{
    if(isReadOnly())
        return;

    current_data_lock.lockForWrite();

    foreach(QString s, values.keys())
        current_data->setValue(s, values[s]);

    current_data_lock.unlock();

    value_changed();
}

QByteArray& ValueBuffer::operator [](QString key)
{
    return (*current_data)[key];
}

QByteArray ValueBuffer::value(const QString &key)
{
    return values(QStringList(key)).value(key);
}

QMap<QString, QByteArray> ValueBuffer::values(const QStringList &keys)
{
    current_data_lock.lockForRead();

    QMap<QString, QByteArray> ret;
    foreach(QString s, keys)
        ret[s] = current_data->getValue(s);

    current_data_lock.unlock();

    return ret;
}

bool ValueBuffer::contains(const QString &key)
{
    current_data_lock.lockForRead();

    bool ret = current_data->contains(key);

    current_data_lock.unlock();

    return ret;
}

void ValueBuffer::clear()
{
    if(isReadOnly())
        return;

    current_data_lock.lockForWrite();
    current_data->clear();
    current_data_lock.unlock();

    value_changed();
}

void ValueBuffer::clearQueues()
{
    _clear_queue(in_queue_mutex, in_queue);
    _clear_queue(out_queue_mutex, out_queue);
}

void ValueBuffer::_clear_queue(QMutex &lock, QQueue< QByteArray > &queue)
{
    lock.lock();
    queue.clear();
    lock.unlock();
}

void ValueBuffer::removeValue(const QString &key)
{
    QStringList sl;
    sl.append(key);
    removeValue(sl);
}

void ValueBuffer::removeValue(const QStringList &keys)
{
    if(isReadOnly())
        return;

    current_data_lock.lockForWrite();

    foreach(QString s, keys)
        current_data->remove(s);

    current_data_lock.unlock();

    value_changed();
}

void ValueBuffer::importData(const QByteArray &dat)
{
    enQueueMessage(InQueue, dat);
}

void ValueBuffer::enQueueMessage(QueueTypeEnum q, const QByteArray &msg)
{
    QMutex *m;
    QQueue< QByteArray > *tmpq;

    switch(q)
    {
    case InQueue:
        m = &in_queue_mutex;
        tmpq = &in_queue;
        break;
    case OutQueue:
        m = &out_queue_mutex;
        tmpq = &out_queue;
        break;
    default:
        return;
    }

    m->lock();
    tmpq->enqueue(msg);
    m->unlock();

    // We want to get rid of anything in the queue as soon as possible
    process_queues();
}

void ValueBuffer::enQueueCurrentData(bool clear)
{
    if(clear)
        current_data_lock.lockForWrite();
    else
        current_data_lock.lockForRead();

    enQueueMessage(OutQueue, current_data->toXml());

    if(clear)
        current_data->clear();

    current_data_lock.unlock();
}

QByteArray ValueBuffer::deQueueMessage(QueueTypeEnum q)
{
    QByteArray ret;
    QMutex *m;
    QQueue<QByteArray> *tmpq;

    switch(q)
    {
    case InQueue:
        m = &in_queue_mutex;
        tmpq = &in_queue;
        break;
    case OutQueue:
        m = &out_queue_mutex;
        tmpq = &out_queue;
        break;
    default:
        return ret;
    }

    m->lock();
    if(!tmpq->empty())
        ret = tmpq->dequeue();
    m->unlock();

    return ret;
}

void ValueBuffer::value_changed()
{
    // Does nothing, override to do something
}

void ValueBuffer::process_queues()
{
    // Flush the queues
    _flush_queue(OutQueue);
    _flush_queue(InQueue);
}

void ValueBuffer::_flush_queue(QueueTypeEnum qt) throw()
{
    QQueue<QByteArray> *queue;
    QMutex *mutex;

    if(qt == InQueue)
    {
        queue = &in_queue;
        mutex = &in_queue_mutex;
    }
    else if (qt == OutQueue)
    {
        queue = &out_queue;
        mutex = &out_queue_mutex;
    }
    else
    {
        throw GUtil::Exception("Unrecognized queue type");
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
                _transport->sendData(ba);
        }
    }
}
