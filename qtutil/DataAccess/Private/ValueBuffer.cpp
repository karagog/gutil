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

    connect(_transport, SIGNAL(notifyNewData(QByteArray)), this, SLOT(load_data(QByteArray)));
}

void ValueBuffer::enQueue()
{
    queue_mutex.lock();
    _values.enqueue(new DataObjects::DataContainer());
    queue_mutex.unlock();
}

DataObjects::DataContainer *ValueBuffer::deQueue()
{
    queue_mutex.lock();
    DataObjects::DataContainer *ret = _values.dequeue();
    queue_mutex.unlock();

    return ret;
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

    foreach(QString s, values.keys())
        _values.last()->setValue(s, values[s]);

    // Only save if we actually made changes
    if(values.keys().length() > 0)
        export_data();
}

QByteArray ValueBuffer::value(const QString &key) const
{
    return _values.last()->getValue(key);
}

QMap<QString, QByteArray> ValueBuffer::values(const QStringList &keys) const
{
    QMap<QString, QByteArray> ret;

    foreach(QString s, keys)
        ret[s] = value(s);

    return ret;
}

bool ValueBuffer::contains(const QString &key) const
{
    return _values.last()->contains(key);
}

void ValueBuffer::clear()
{
    if(isReadOnly())
        return;

    _values.last()->clear();

    export_data();
}

void ValueBuffer::clearQueue()
{
    while(_values.count() > 1)
        delete _values.dequeue();
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

    bool val_erased = false;

    foreach(QString s, keys)
    {
        if(_values.last()->remove(s))
            val_erased = true;
    }

    if(val_erased)
        export_data();
}

QList<QByteArray> ValueBuffer::prepare_data_for_export()
{
    QList<QByteArray> ret;
    while(_values.count() > 1)
        ret.append(_values.dequeue()->toXml());
    return ret;
}

void ValueBuffer::import_data(const QByteArray &dat)
{
    _values.last()->fromXml(dat);

    enQueue();
}

void ValueBuffer::export_data()
{
    QList<QByteArray> data = prepare_data_for_export();
    foreach(QByteArray b, data)
        _transport->sendData(b);
}

void ValueBuffer::load_data(const QByteArray &new_data)
{
    import_data(new_data);
}
