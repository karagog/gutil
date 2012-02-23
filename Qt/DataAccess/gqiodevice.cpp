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

#include "Core/extendedexception.h"
#include "gqiodevice.h"
#include <QIODevice>

NAMESPACE_GUTIL2(QT, DataAccess);


#define DEFAULT_WRITE_PROGRESS_RESOLUTION -1
#define DEFAULT_READ_PROGRESS_RESOLUTION -1

GQIODevice::GQIODevice(QIODevice *io, QObject *parent)
    :GIODevice(parent),
    _p_WriteProgressResolution(DEFAULT_WRITE_PROGRESS_RESOLUTION),
    _p_ReadProgressResolution(DEFAULT_READ_PROGRESS_RESOLUTION),
    _p_WriteProgress(0),
    _p_ReadProgress(0)
{
    _io_device = io;
    _io_device->setParent(this);

    connect(&IODevice(), SIGNAL(readyRead()), this, SLOT(raiseReadyRead()));
}

void GQIODevice::send_data(const QByteArray &data)
{
    _fail_if_not_open();

    emit WriteProgressUpdated(_p_WriteProgress = 0);

    int bytes_written(0);
    int byte_resolution( GetWriteProgressResolution() == DEFAULT_WRITE_PROGRESS_RESOLUTION ?
                         data.length() :
                         GetWriteProgressResolution() );
    while(bytes_written < data.length())
    {
        int difference(data.length() - bytes_written);
        int to_write(byte_resolution);

        // If the difference is less than we're about to write, then only write
        //  as much as we can
        if(difference < to_write)
            to_write = difference;

        int written(
                IODevice().write(data.constData() + bytes_written, to_write)
                );

        bytes_written += written;

        // Update the write progress
        emit WriteProgressUpdated(
                _p_WriteProgress = (bytes_written * 100) / data.length()
                                   );

        // If the write failed for some reason, break
        if(written != to_write)
            break;
    }

    if(bytes_written != data.length())
    {
        DataTransportException<true> ex(
                QString("Write failed after writing %1 / %2 bytes")
                .arg(bytes_written)
                .arg(data.length())
                .toAscii().constData());
        ex.SetData("err", IODevice().errorString().toAscii().constData());
        THROW_GUTIL_EXCEPTION( ex );
    }
}

QByteArray GQIODevice::receive_data()
{
    _fail_if_not_open();

    emit ReadProgressUpdated(_p_ReadProgress = 0);

    QByteArray ret;
    int bytes_available(IODevice().bytesAvailable());
    int bytes_read(0);
    int resolution(GetReadProgressResolution() == DEFAULT_READ_PROGRESS_RESOLUTION ?
                   bytes_available : GetReadProgressResolution());

    while(bytes_read < bytes_available)
    {
        int to_read(resolution);
        int difference(bytes_available - bytes_read);
        if(difference < to_read)
            to_read = difference;

        QByteArray tmp = IODevice().read(to_read);
        ret.append(tmp);
        bytes_read += tmp.length();

        emit ReadProgressUpdated(
                _p_ReadProgress = (bytes_read * 100) / bytes_available
                );

        if(tmp.length() < to_read)
            break;
    }

    if(bytes_read != bytes_available)
    {
        DataTransportException<true> ex(QString("Read %1 / %2 bytes")
                                        .arg(bytes_read)
                                        .arg(bytes_available)
                                        .toAscii().constData());
        ex.SetData("err", IODevice().errorString().toAscii().constData());
        THROW_GUTIL_EXCEPTION( ex );
    }

    return ret;
}

bool GQIODevice::has_data_available()
{
    if(!IODevice().isOpen())
        return false;

    return IODevice().bytesAvailable() > 0;
}

void GQIODevice::_fail_if_not_open()
{
    if(!IODevice().isOpen())
        THROW_NEW_GUTIL_EXCEPTION2(DataTransportException,
                                   "IO Device is not open!" );
}


END_NAMESPACE_GUTIL2;
