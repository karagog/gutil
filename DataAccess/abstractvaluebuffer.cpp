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
#include "Interfaces/ITransportMechanism.h"
#include "Utils/abstractlogger.h"
#include "Core/Tools/stringhelpers.h"
#include "Core/exception.h"
#include <QStringList>
#include <QtConcurrentRun>
using namespace GUtil;

DataAccess::AbstractValueBuffer::AbstractValueBuffer(
        Interfaces::ITransportMechanism *transport_mechanism,
		Utils::AbstractLogger *logger,
        QObject *parent)
            :QObject(parent),
            Interfaces::IReadOnlyObject(false)
{
	_logger = logger;
    current_data = new DataObjects::DataContainer();

    _transport = transport_mechanism;

    connect(_transport, SIGNAL(notifyNewData(QByteArray)), this, SLOT(importData(QByteArray)));
}

DataAccess::AbstractValueBuffer::~AbstractValueBuffer()
{
    delete current_data;
    delete _transport;
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

//void ValueBuffer::makeReadOnly(bool val)
//{
//    _readonly = val;
//}

//bool ValueBuffer::IsReadOnly()
//{
//    return _readonly;
//}

bool DataAccess::AbstractValueBuffer::setValue(const QString &key, const QByteArray& value)
{
    QMap<QString, QByteArray> m;
    m.insert(key, value);
    return setValues(m);
}

bool DataAccess::AbstractValueBuffer::setValues(const QMap<QString, QByteArray> &values)
{
    if(IsReadOnly())
        return false;

    current_data_lock.lockForWrite();

    foreach(QString s, values.keys())
        current_data->setValue(s, values[s]);

    current_data_lock.unlock();

	return TriggerValueChanged();
}

bool DataAccess::AbstractValueBuffer::TriggerValueChanged()
{
	try
	{
		value_changed();
	}
	catch(Core::Exception ex)
	{
		LogException(ex);
		return false;
	}

	return true;
}

QByteArray& DataAccess::AbstractValueBuffer::operator [](QString key)
{
    return (*current_data)[key];
}

QByteArray DataAccess::AbstractValueBuffer::value(const QString &key)
{
    return values(QStringList(key)).value(key);
}

QMap<QString, QByteArray> DataAccess::AbstractValueBuffer::values(const QStringList &keys)
{
    current_data_lock.lockForRead();

    QMap<QString, QByteArray> ret;
    foreach(QString s, keys)
        ret[s] = current_data->getValue(s);

    current_data_lock.unlock();

    return ret;
}

bool DataAccess::AbstractValueBuffer::contains(const QString &key)
{
    current_data_lock.lockForRead();

    bool ret = current_data->contains(key);

    current_data_lock.unlock();

    return ret;
}

void DataAccess::AbstractValueBuffer::clear()
{
    if(IsReadOnly())
        return;

    current_data_lock.lockForWrite();
    current_data->clear();
    current_data_lock.unlock();

	TriggerValueChanged();
}

void DataAccess::AbstractValueBuffer::clearQueues()
{
    _clear_queue(in_queue_mutex, in_queue);
    _clear_queue(out_queue_mutex, out_queue);
}

void DataAccess::AbstractValueBuffer::_clear_queue(QMutex &lock, QQueue< QByteArray > &queue)
{
    lock.lock();
    queue.clear();
    lock.unlock();
}

bool DataAccess::AbstractValueBuffer::removeValue(const QString &key)
{
    QStringList sl;
    sl.append(key);
    return removeValue(sl);
}

bool DataAccess::AbstractValueBuffer::removeValue(const QStringList &keys)
{
    if(IsReadOnly())
        return false;

    current_data_lock.lockForWrite();

    foreach(QString s, keys)
        current_data->remove(s);

    current_data_lock.unlock();

    return TriggerValueChanged();
}

void DataAccess::AbstractValueBuffer::importData(const QByteArray &dat)
{
    enQueueMessage(InQueue, dat);
}

void DataAccess::AbstractValueBuffer::enQueueMessage(QueueTypeEnum q, const QByteArray &msg)
{
    en_deQueueMessage(q, msg, true);
}

void DataAccess::AbstractValueBuffer::enQueueCurrentData(bool clear)
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

QByteArray DataAccess::AbstractValueBuffer::deQueueMessage(QueueTypeEnum q)
{
    return en_deQueueMessage(q, QByteArray(), false);
}

QByteArray DataAccess::AbstractValueBuffer::en_deQueueMessage(QueueTypeEnum q, const QByteArray &msg, bool enqueue)
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

void DataAccess::AbstractValueBuffer::LogException(const GUtil::Core::Exception &ex) const
{
    if(_logger != 0)
        _logger->LogException(ex);
}

Utils::AbstractLogger *DataAccess::AbstractValueBuffer::Logger() const
{
    return _logger;
}

Interfaces::ITransportMechanism *DataAccess::AbstractValueBuffer::Transport() const
{
    return _transport;
}

void DataAccess::AbstractValueBuffer::process_queues()
{
    // Flush the queues
    _flush_queue(OutQueue);
    _flush_queue(InQueue);
}

void DataAccess::AbstractValueBuffer::_flush_queue(QueueTypeEnum qt)
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
        throw Core::Exception("Unrecognized queue type");
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
