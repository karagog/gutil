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

#include "abstractdatatransportmechanism.h"
using namespace GUtil::DataAccess;

DataTransports::AbstractDataTransportMechanism::AbstractDataTransportMechanism(QObject *parent)
    :QObject(parent), Core::Interfaces::IReadOnlyObject(false)
{
    _has_data = false;
    _cur_state = GoodState;
}

void DataTransports::AbstractDataTransportMechanism::Write(const QByteArray &data)
{
    SendData(data);
}

void DataTransports::AbstractDataTransportMechanism::SendData(const QByteArray &data)
{
    FailIfReadOnly();

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

QByteArray DataTransports::AbstractDataTransportMechanism::ReceiveData()
{
    _lock.lock();

    QByteArray ret;
    try
    {
        ret = last_data_received = receive_data();
    }
    catch(Core::EndOfFileException &){}  // Quietly catch; returns null
    catch(...)
    {
        _lock.unlock();
        throw;
    }

    _lock.unlock();

    return ret;
}

DataTransports::AbstractDataTransportMechanism &
        DataTransports::AbstractDataTransportMechanism::operator << (const char*data)
{
    SendData(QByteArray(data));
    return *this;
}

DataTransports::AbstractDataTransportMechanism &
        DataTransports::AbstractDataTransportMechanism::operator << (const std::string &data)
{
    return *this<<data.c_str();
}

DataTransports::AbstractDataTransportMechanism &
        DataTransports::AbstractDataTransportMechanism::operator << (const QString &data)
{
    return *this<<data.toStdString();
}

void DataTransports::AbstractDataTransportMechanism::operator >> (QByteArray &data_target)
{
    data_target = ReceiveData();
}

void DataTransports::AbstractDataTransportMechanism::operator >> (QString &dt)
{
    QByteArray res;
    *this>>res;
    dt = QString(res);
}

void DataTransports::AbstractDataTransportMechanism::operator >> (std::string &dt)
{
    QByteArray res;
    *this>>res;
    dt = QString(res).toStdString();
}

void DataTransports::AbstractDataTransportMechanism::trigger_update_has_data_available()
{
    _lock.lock();

    try
    {
        update_has_data_variable(_has_data);

        while(_has_data)
        {
            try
            {
                last_data_received = receive_data();
            }
            catch(Core::EndOfFileException &)
            {
                break;
            }

            emit notifyNewData(last_data_received);

            update_has_data_variable(_has_data);
        }
    }
    catch(Core::Exception &)
    {
        // Don't let exceptions crash us
    }

    _lock.unlock();
}

QString DataTransports::AbstractDataTransportMechanism::ReadonlyMessageIdentifier()
{
    return "DataAccess::DataTransports::AbstractDataTransportMechanism";
}
