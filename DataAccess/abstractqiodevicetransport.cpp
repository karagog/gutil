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

#include "abstractqiodevicetransport.h"
#include <QIODevice>
using namespace GUtil;

DataAccess::AbstractQIODeviceTransport::AbstractQIODeviceTransport(QIODevice *io, QObject *parent) :
    AbstractDataTransportMechanism(parent)
{
    _io_device = io;

    connect(&IODevice(), SIGNAL(readyRead()), this, SLOT(trigger_update_has_data_available()));
}

QIODevice &DataAccess::AbstractQIODeviceTransport::IODevice() const
{
    return *_io_device;
}

void DataAccess::AbstractQIODeviceTransport::send_data(const QByteArray &data)
        throw(Core::DataTransportException)
{
    _fail_if_not_open();

    if(data.length() != IODevice().write(data))
        throw Core::DataTransportException(IODevice().errorString().toStdString());
}

QByteArray DataAccess::AbstractQIODeviceTransport::receive_data()
        throw(Core::DataTransportException,
              Core::EndOfFileException)
{
    _fail_if_not_open();

    if(IODevice().atEnd())
        throw Core::EndOfFileException();

    QByteArray ret = IODevice().readAll();
    if(ret.isNull())
        throw Core::DataTransportException(IODevice().errorString().toStdString());

    return ret;
}

void DataAccess::AbstractQIODeviceTransport::update_has_data_variable(bool &has_data_variable)
        throw(GUtil::Core::DataTransportException)
{
    if(IODevice().isOpen())
        has_data_variable = IODevice().bytesAvailable() > 0;
    else
        has_data_variable = false;
}

void DataAccess::AbstractQIODeviceTransport::_fail_if_not_open()
{
    if(!IODevice().isOpen())
        throw Core::DataTransportException("IO Device is not open!");
}
