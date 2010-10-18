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
using namespace GUtil;
using namespace std;

DataAccess::FileTransport::FileTransport(const QString &filename, QObject *parent)
    :DataAccess::StreamTransport(_file = new fstream(), parent)
{
    SetStopOnLineEnd(false);

    _file_watcher = new QFileSystemWatcher(this);
    connect(_file_watcher, SIGNAL(fileChanged(QString)), this, SLOT(trigger_update_has_data_available()));

    SetFileName(filename);
    SetWriteMode(WriteAppend);

    trigger_update_has_data_available();
}

void DataAccess::FileTransport::SetWriteMode(WriteModeEnum mode)
{
    _write_mode = mode;
}

DataAccess::FileTransport::WriteModeEnum DataAccess::FileTransport::GetWriteMode()
{
    return _write_mode;
}

void DataAccess::FileTransport::send_data(const QByteArray &data) throw(Core::DataTransportException)
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

    // We read in what we just wrote so we refresh our record of what's in the file
    receive_data();
}

QByteArray DataAccess::FileTransport::receive_data() throw(Core::DataTransportException)
{
    _open_file(false);

    last_data_received = DataAccess::StreamTransport::receive_data();

    _close_file();

    _last_update_time = QFileInfo(FileName()).lastModified();
    _hash = QCryptographicHash::hash(last_data_received, QCryptographicHash::Md5);

    return last_data_received;
}

bool DataAccess::FileTransport::has_been_updated()
{
    return _last_update_time != QFileInfo(FileName()).lastModified();
}

void DataAccess::FileTransport::update_has_data_variable(bool &has_data_variable)
        throw(Core::DataTransportException)
{
    has_data_variable = false;

    if(has_been_updated())
    {
        _open_file(false);

        // The modify times don't match, so read the data and determine if it's new or not
        QByteArray tmphash = QCryptographicHash::hash(
                DataAccess::StreamTransport::receive_data(),
                QCryptographicHash::Md5);

        has_data_variable = tmphash != _hash;

        _close_file();
    }
}

void DataAccess::FileTransport::SetFileName(const QString &filename)
{
    if(_filename.length() > 0)
        _file_watcher->removePath(QString::fromStdString(_filename));

    _filename = filename.toStdString();

    if(_filename.length() > 0)
    {
        QFile f(filename);

        // Create the file if it doesn't exist
        if(!f.exists())
        {
            f.open(QFile::WriteOnly);
            f.close();
        }

        _file_watcher->addPath(filename);
    }
}

QString DataAccess::FileTransport::FileName() const
{
    return QString::fromStdString(_filename);
}

QByteArray DataAccess::FileTransport::FileData()
{
    return last_data_received;
}

//void DataTransports::FileTransport::ReloadFile()
//{
//    ReceiveData();
//}

void DataAccess::FileTransport::TruncateFile()
{
    FailIfReadOnly();

    WriteModeEnum _md = GetWriteMode();
    SetWriteMode(WriteOver);
    Write("");
    SetWriteMode(_md);
}

void DataAccess::FileTransport::_open_file(bool for_write)
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

    _file->open(_filename.c_str(), md);

    if(!_file->is_open())
        throw Core::DataTransportException(
                QString("Could not open file: %1").arg(FileName()).toStdString());
}

void DataAccess::FileTransport::_close_file()
{
    _file->close();
}
