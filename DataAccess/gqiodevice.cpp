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

#include "gqiodevice.h"
#include <QIODevice>
using namespace GUtil;

DataAccess::GQIODevice::GQIODevice(QIODevice *io, QObject *parent) :
    GIODevice(parent)
{
    _io_device = io;

    connect(&IODevice(), SIGNAL(readyRead()), this, SLOT(raiseReadyRead()));
}

QIODevice &DataAccess::GQIODevice::IODevice() const
{
    return *_io_device;
}

void DataAccess::GQIODevice::send_data(const QByteArray &data)
        throw(Core::DataTransportException)
{
    _fail_if_not_open();

    if(data.length() != IODevice().write(data))
    {
        Core::DataTransportException ex("Write failed");
        ex.SetData("err", IODevice().errorString().toStdString());
        throw ex;
    }
}

QByteArray DataAccess::GQIODevice::receive_data()
        throw(Core::DataTransportException)
{
    _fail_if_not_open();

    bool has_data = HasDataAvailable();
    QByteArray ret = IODevice().readAll();

    if(has_data && ret.length() == 0)
    {
        Core::DataTransportException ex("Read 0 bytes!");
        ex.SetData("err", IODevice().errorString().toStdString());
        throw ex;
    }

    return ret;
}

bool DataAccess::GQIODevice::HasDataAvailable()
{
    if(!IODevice().isOpen())
        return false;

    return IODevice().bytesAvailable() > 0;
}

void DataAccess::GQIODevice::_fail_if_not_open()
{
    if(!IODevice().isOpen())
        throw Core::DataTransportException("IO Device is not open!");
}
