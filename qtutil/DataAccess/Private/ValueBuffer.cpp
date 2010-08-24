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
#include "stringhelpers.h"
#include "exception.h"
#include <QStringList>
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

    // Start off with one variable container (default only uses one)
    enQueue();

    connect(_transport, SIGNAL(notifyNewData(QByteArray)), this, SLOT(importData(QByteArray)));
}

void ValueBuffer::enQueue(bool copy)
{
    queue_lock.lockForWrite();
    _enQueue(copy);
    queue_lock.unlock();
}

void ValueBuffer::_enQueue(bool copy)
{
    if(copy)
        _values.enqueue(new DataObjects::DataContainer(*currentDataContainer()));
    else
        _values.enqueue(new DataObjects::DataContainer());
}

void ValueBuffer::deQueue()
{
    queue_lock.lockForWrite();
    _deQueue();
    queue_lock.unlock();
}

void ValueBuffer::_deQueue()
{
    _values.dequeue();
}

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

    queue_lock.lockForRead();
    data_container_lock.lockForWrite();

    foreach(QString s, values.keys())
        currentDataContainer()->setValue(s, values[s]);

    data_container_lock.unlock();
    queue_lock.unlock();

    value_changed();
}

QByteArray& ValueBuffer::operator [](QString key)
{
    return (*currentDataContainer())[key];
}

QByteArray ValueBuffer::value(const QString &key)
{
    return values(QStringList(key)).value(key);
}

QMap<QString, QByteArray> ValueBuffer::values(const QStringList &keys)
{
    queue_lock.lockForRead();
    data_container_lock.lockForRead();

    QMap<QString, QByteArray> ret;
    foreach(QString s, keys)
        ret[s] = currentDataContainer()->getValue(s);

    data_container_lock.unlock();
    queue_lock.unlock();

    return ret;
}

bool ValueBuffer::contains(const QString &key)
{
    queue_lock.lockForRead();
    data_container_lock.lockForRead();

    bool ret = currentDataContainer()->contains(key);

    data_container_lock.lockForRead();
    queue_lock.unlock();

    return ret;
}

void ValueBuffer::clear()
{
    if(isReadOnly())
        return;

    queue_lock.lockForRead();
    data_container_lock.lockForWrite();

    currentDataContainer()->clear();

    data_container_lock.unlock();
    queue_lock.unlock();

    value_changed();
}

void ValueBuffer::clearQueue()
{
    queue_lock.lockForWrite();

    while(_values.count() > 1)
    {
        delete firstContainerInLine();
        _deQueue();
    }

    queue_lock.unlock();
}

void ValueBuffer::remove(const QString &key)
{
    QStringList sl;
    sl.append(key);
    remove(sl);
}

void ValueBuffer::remove(const QStringList &keys)
{
    if(isReadOnly())
        return;

    queue_lock.lockForRead();
    data_container_lock.lockForWrite();

    foreach(QString s, keys)
        currentDataContainer()->remove(s);

    data_container_lock.unlock();
    queue_lock.unlock();

    value_changed();
}

void ValueBuffer::value_changed()
{
    // The base class does nothing, derived classes may choose to do something
}

QList<QByteArray> ValueBuffer::prepare_data_for_export()
{
    queue_lock.lockForWrite();

    QList<QByteArray> ret;
    while(_values.count() > 1)
    {
        DataObjects::DataContainer *vals = firstContainerInLine();
        _deQueue();
        ret.append(vals->toXml());
        delete vals;
    }

    queue_lock.unlock();

    return ret;
}

void ValueBuffer::importData(const QByteArray &dat)
{
    queue_lock.lockForRead();
    data_container_lock.lockForWrite();

    try
    {
        currentDataContainer()->clear();
        currentDataContainer()->fromXml(dat);
    }
    catch(GUtil::Exception)
    {
        data_container_lock.unlock();
        queue_lock.unlock();
        return;
    }

    data_container_lock.unlock();
    queue_lock.unlock();

    value_changed();
}

void ValueBuffer::exportData()
{
    try
    {
        foreach(QByteArray b, prepare_data_for_export())
            _transport->sendData(b);
    }
    catch(GUtil::Exception)
    {
        return;
    }
}

DataContainer *ValueBuffer::firstContainerInLine()
{
    return _values.first();
}

DataContainer *ValueBuffer::currentDataContainer()
{
    return _values.last();
}
