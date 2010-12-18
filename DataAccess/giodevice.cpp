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

#include "giodevice.h"
#include "Core/exception.h"
#include <QWaitCondition>
using namespace GUtil;

DataAccess::GIODevice::GIODevice(QObject *parent)
    :QThread(parent),
    _p_Identity(QUuid::createUuid())
{
}

DataAccess::GIODevice::~GIODevice()
{
    if(isRunning())
    {
        terminate();
        wait();
    }
}

void DataAccess::GIODevice::SendData(const QByteArray &data)
{
    FailIfReadOnly();

    _this_lock.lock();

    try
    {
        send_data(data);
    }
    catch(...)
    {
        _this_lock.unlock();
        throw;
    }

    _this_lock.unlock();
}

QByteArray DataAccess::GIODevice::ReceiveData(bool block)
        throw(Core::DataTransportException)
{
    _this_lock.lock();

    while(block && !has_data_available())
        // Wait for new data to become available
        condition_new_data_available.wait(&_this_lock);

    QByteArray ret;
    try
    {
        ret = receive_data();
    }
    catch(...)
    {
        _this_lock.unlock();
        throw;
    }

    _this_lock.unlock();

    return ret;
}

DataAccess::GIODevice &
        DataAccess::GIODevice::operator << (const char*data)
{
    SendData(QByteArray(data));
    return *this;
}

DataAccess::GIODevice &
        DataAccess::GIODevice::operator << (const std::string &data)
{
    return *this<<data.c_str();
}

DataAccess::GIODevice &
        DataAccess::GIODevice::operator << (const QString &data)
{
    return *this<<data.toStdString();
}

void DataAccess::GIODevice::operator >> (QByteArray &data_target)
{
    data_target = ReceiveData();
}

void DataAccess::GIODevice::operator >> (QString &dt)
{
    QByteArray res;
    *this>>res;
    dt = QString(res);
}

void DataAccess::GIODevice::operator >> (std::string &dt)
{
    QByteArray res;
    *this>>res;
    dt = QString(res).toStdString();
}

void DataAccess::GIODevice::raiseReadyRead()
{
    condition_new_data_available.wakeAll();
    emit ReadyRead(GetIdentity());
}

std::string DataAccess::GIODevice::ReadonlyMessageIdentifier() const
{
    return "DataAccess::GIODevice";
}

bool DataAccess::GIODevice::HasDataAvailable()
{
    bool ret;
    _this_lock.lock();
    {
        ret = has_data_available();
    }
    _this_lock.unlock();
    return ret;
}
