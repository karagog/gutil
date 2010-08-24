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
using namespace GQtUtil::Interfaces;

ITransportMechanism::ITransportMechanism(QObject *parent)
    :QObject(parent)
{
    _has_data = false;
}

void ITransportMechanism::sendData(const QByteArray &data)
{
    _lock.lock();

    try
    {
        send_data(data);
    }
    catch(...)
    {
        _lock.unlock();
        throw;
    }

    _lock.unlock();

    emit notifyDataSent(data);
}

QByteArray ITransportMechanism::receiveData()
{
    _lock.lock();

    QByteArray ret;
    try
    {
        ret = last_data_received = receive_data();
    }
    catch(...)
    {
        _lock.unlock();
        throw;
    }

    _lock.unlock();

    return ret;
}

void ITransportMechanism::trigger_update_has_data_available()
{
    _lock.lock();

    try
    {
        update_has_data_variable(_has_data);

        while(_has_data)
        {
            emit notifyNewData(last_data_received = receive_data());

            update_has_data_variable(_has_data);
        }
    }
    catch(...)
    {
        _lock.unlock();
        throw;
    }

    _lock.unlock();
}
