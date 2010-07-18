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

#include "DA_ValueBuffer_P.h"
#include "Transports/ITransportMechanism.h"
#include "stringhelpers.h"
#include "exception.h"
#include <QStringList>
using namespace GUtil;
using namespace GQtUtil::DataAccess::Private;

DA_ValueBuffer_P::DA_ValueBuffer_P(
        Transports::ITransportMechanism *transport_mechanism,
        QObject *parent)
            :QObject(parent)
{
    _transport = transport_mechanism;
    _readonly = false;

    // Start off with one variable container (default only uses one)
    enQueue();

    reload();
}

void DA_ValueBuffer_P::enQueue()
{
    queue_mutex.lock();
    _values.enqueue(new DataObjects::DataContainer());
    queue_mutex.unlock();
}

DataObjects::DataContainer *DA_ValueBuffer_P::deQueue()
{
    queue_mutex.lock();
    DataObjects::DataContainer *ret = _values.dequeue();
    queue_mutex.unlock();

    return ret;
}

void DA_ValueBuffer_P::makeReadOnly(bool val)
{
    _readonly = val;
}

bool DA_ValueBuffer_P::isReadOnly()
{
    return _readonly;
}

void DA_ValueBuffer_P::setValue(const QString &key, const QString& value)
{
    QMap<QString, QString> m;
    m.insert(key, value);
    setValues(m);
}

void DA_ValueBuffer_P::setValues(const QMap<QString, QString> &values)
{
    if(isReadOnly())
        return;

    foreach(QString s, values.keys())
        _values.last()->setValue(s, values[s]);

    // Only save if we actually made changes
    if(values.keys().length() > 0)
        export_data();
}

QString DA_ValueBuffer_P::value(const QString &key) const
{
    return _values.last()->getValue(key);
}

QMap<QString, QString> DA_ValueBuffer_P::values(const QStringList &keys) const
{
    QMap<QString, QString> ret;

    foreach(QString s, keys)
        ret[s] = value(s);

    return ret;
}

bool DA_ValueBuffer_P::contains(const QString &key) const
{
    return _values.last()->contains(key);
}

void DA_ValueBuffer_P::clear()
{
    if(isReadOnly())
        return;

    _values.last()->clear();

    export_data();
}

void DA_ValueBuffer_P::clearQueue()
{
    while(_values.count() > 1)
        delete _values.dequeue();
}

void DA_ValueBuffer_P::remove(const QString &key)
{
    QStringList sl;
    sl.append(key);
    remove(sl);
}

void DA_ValueBuffer_P::remove(const QStringList &keys)
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

QList<QByteArray> DA_ValueBuffer_P::prepare_data_for_export()
{
    QList<QByteArray> ret;
    while(_values.count() > 1)
        ret.append(_values.deque()->toXml());
    return ret;
}

void DA_ValueBuffer_P::import_data(const QList<QByteArray> &dat)
{
    while(dat.length() > 0)
    {
        _values.last()->fromXml(dat.first());
        enQueue();
        dat.removeFirst();
    }
}

void DA_ValueBuffer_P::export_data()
{
    QList<QByteArray> data = prepare_data_for_export();
    foreach(QByteArray b, data)
        _transport->exportData(b);
}

void DA_ValueBuffer_P::reload_data()
{
    import_data(_transport->importData());
}

void DA_ValueBuffer_P::reload()
{
    reload_data();
}
