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

#include "ITransportMechanism.h"
#include "exception.h"
using namespace GQtUtil::DataAccess::Private::Transports;

ITransportMechanism::ITransportMechanism(QObject *parent)
    :QObject(parent)
{
    setHasData(false);
}

void ITransportMechanism::sendData(const QByteArray &data)
{
    _lock.lock();

    try
    {
        send_data(data);
    }
    catch(GUtil::Exception)
    {
        _lock.unlock();
        throw;
    }

    _lock.unlock();
}

QByteArray ITransportMechanism::receiveData()
{
    return receive_data_private(false);
}

QByteArray ITransportMechanism::receive_data_private(bool asynchronous)
{
    QByteArray res;
    _lock.lock();

    try
    {
        res = receive_data();
    }
    catch(GUtil::Exception)
    {
        _lock.unlock();
        throw;
    }

    _lock.unlock();

    if(asynchronous)
        emit notifyNewData(res);

    update_has_data_available();

    return res;
}

bool ITransportMechanism::hasData() const
{
    _lock.lock();
    bool ret = _has_data;
    _lock.unlock();
    return ret;
}

void ITransportMechanism::setHasData(bool val)
{
    _lock.lock();
    _has_data = val;
    _lock.unlock();
}

void ITransportMechanism::receive_asynchronous_update()
{
    update_has_data_available();

    while(hasData())
        receive_data_private(true);
}
