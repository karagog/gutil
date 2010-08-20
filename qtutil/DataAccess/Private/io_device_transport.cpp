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

#include "io_device_transport.h"
#include "exception.h"
#include <QIODevice>
using namespace GQtUtil::DataAccess::Private;

IODevice_Transport::IODevice_Transport(QIODevice *io_device, QObject *parent)
    :ITransportMechanism(parent)
{
    _io_device = io_device;

    connect(_io_device, SIGNAL(readyRead()), this, SLOT(update_has_data_available()));
}

IODevice_Transport::~IODevice_Transport()
{
    _io_device->close();
}

void IODevice_Transport::send_data(const QByteArray &data)
{
    _io_device_lock.lock();

    bool failed = _io_device->write(data) != data.length();
    QString err = failed ? _io_device->errorString() : QString::null;

    _io_device_lock.unlock();

    if(failed)
        throw GUtil::Exception(err.toStdString());
}

QByteArray IODevice_Transport::receive_data()
{
    _io_device_lock.lock();

    QByteArray dat = _io_device->readAll();
    QString err = dat.isNull() ? _io_device->errorString() : QString::null;

    _io_device_lock.unlock();

    if(dat.isNull())
        throw GUtil::Exception(err.toStdString());

    return dat;
}

void IODevice_Transport::updateHasDataAvailable(bool &has_data_variable)
{
    has_data_variable = _io_device->bytesAvailable() > 0;
}
