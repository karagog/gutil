/*Copyright 2010-2012 George Karagoulis

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
#include <QWaitCondition>
USING_NAMESPACE_GUTIL1(DataObjects);

NAMESPACE_GUTIL2(QT, DataAccess);


GIODevice::GIODevice(QObject *parent)
    :QThread(parent),
    _p_Identity(QUuid::createUuid())
{
    // So you can pass QUuid's through signals and slots
    qRegisterMetaType<QUuid>("QUuid");
}

GIODevice::~GIODevice()
{
    if(isRunning())
    {
        terminate();
        wait();
    }
}

void GIODevice::SendData(const QByteArray &data)
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

QByteArray GIODevice::ReceiveData()
{
    QByteArray ret;
    _this_lock.lock();
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

GIODevice &GIODevice::operator << (const char*data)
{
    SendData(QByteArray(data));
    return *this;
}

GIODevice &GIODevice::operator << (const QString &data)
{
    return operator <<(data.toUtf8());
}

GIODevice &GIODevice::operator << (const QByteArray &data)
{
    Write(data);
    return *this;
}

void GIODevice::operator >> (QByteArray &data_target)
{
    data_target = ReceiveData();
}

void GIODevice::operator >> (QString &dt)
{
    QByteArray res;
    *this>>res;
    dt = QString(res);
}

void GIODevice::raiseReadyRead()
{
    emit ReadyRead(GetIdentity());
}

bool GIODevice::HasDataAvailable()
{
    bool ret(false);
    _this_lock.lock();
    {
        ret = has_data_available();
    }
    _this_lock.unlock();
    return ret;
}


END_NAMESPACE_GUTIL2;
