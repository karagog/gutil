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

#include "filetransport.h"
#include "qtlockedfile.h"
#include "Core/exception.h"
#include <fstream>
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QCryptographicHash>
using namespace GUtil::DataAccess;
using namespace std;

DataTransports::FileTransport::FileTransport(const QString &filename, QObject *parent)
    :DataTransports::StreamTransport(_file = new fstream(), parent)
{
    _filename = new char[filename.length()];
    strcpy(_filename, filename.toStdString().c_str());

    SetWriteMode(WriteAppend);

    trigger_update_has_data_available();

    _file_watcher = new QFileSystemWatcher(this);
    _file_watcher->addPath(filename);

    connect(_file_watcher, SIGNAL(fileChanged(QString)), this, SLOT(trigger_update_has_data_available()));
}

void DataTransports::FileTransport::SetWriteMode(WriteModeEnum mode)
{
    _write_mode = mode;
}

void DataTransports::FileTransport::send_data(const QByteArray &data) throw(Core::DataTransportException)
{
    _open_file(true);

    if(!_file->is_open())
        throw Core::DataTransportException(
                QString("Could not open file for write: %1").arg(FileName()).toStdString());

    // At this point the settings file is ours for sole writing
    _file->write(data.constData(), data.length());
    if(_file->fail())
    {
        _close_file();
        throw Core::DataTransportException("Write failed");
    }

    _close_file();
}

QByteArray DataTransports::FileTransport::receive_data() throw(Core::DataTransportException)
{
    _open_file(false);

    QByteArray dat = DataTransports::StreamTransport::receive_data();

    _close_file();

    _last_update_time = QFileInfo(FileName()).lastModified();
    _hash = QCryptographicHash::hash(dat, QCryptographicHash::Md5);

    return dat;
}

void DataTransports::FileTransport::update_has_data_variable(bool &has_data_variable) throw(Core::DataTransportException)
{
    has_data_variable = false;

    if(_last_update_time != QFileInfo(FileName()).lastModified())
    {
        _open_file(false);

        // The modify times don't match, so read the data and determine if it's new or not
        QByteArray tmphash = QCryptographicHash::hash(
                DataTransports::StreamTransport::receive_data(),
                QCryptographicHash::Md5);

        has_data_variable = tmphash != _hash;

        _close_file();
    }
}

QString DataTransports::FileTransport::FileName() const
{
    return QString(_filename);
}

QByteArray DataTransports::FileTransport::FileData()
{
    return receive_data();
}

void DataTransports::FileTransport::Reload()
{
    emit notifyNewData(receive_data());
}

void DataTransports::FileTransport::_open_file(bool for_write)
{
    ios_base::openmode md;

    if(for_write)
    {
        md = ios_base::out;

        if(_write_mode == WriteAppend)
            md |= ios_base::app;
        else if(_write_mode == WriteOver)
            md |= ios_base::trunc;
        else
            throw Core::NotImplementedException();
    }
    else
    {
        md = ios_base::in;
    }

    _file->open(_filename, md);

    if(!_file->is_open())
        throw Core::DataTransportException(
                QString("Could not open file: %1").arg(FileName()).toStdString());
}

void DataTransports::FileTransport::_close_file()
{
    _file->close();
}
