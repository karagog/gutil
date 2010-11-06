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

#include "gfileiodevice.h"
#include "Core/exception.h"
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QCryptographicHash>
using namespace GUtil;
using namespace std;

DataAccess::GFileIODevice::GFileIODevice(const QString &filename, QObject *parent)
    :GQIODevice(new QFile, parent)
{
    _file_watcher = new QFileSystemWatcher(this);
    connect(_file_watcher, SIGNAL(fileChanged(QString)), this, SLOT(raiseReadyRead()));

    SetFileName(filename);
    SetWriteMode(WriteAppend);
}

void DataAccess::GFileIODevice::SetWriteMode(WriteModeEnum mode)
{
    _write_mode = mode;
}

DataAccess::GFileIODevice::WriteModeEnum DataAccess::GFileIODevice::GetWriteMode()
{
    return _write_mode;
}

void DataAccess::GFileIODevice::send_data(const QByteArray &data)
        throw(Core::DataTransportException)
{
    _open_file(true);

    // At this point the settings file is ours for sole writing
    GQIODevice::send_data(data);

    _close_file();
}

QByteArray DataAccess::GFileIODevice::receive_data()
        throw(Core::DataTransportException)
{
    _open_file(false);

    QByteArray ret = DataAccess::GQIODevice::receive_data();

    _close_file();

    _last_update_time = QFileInfo(FileName()).lastModified();

    return ret;
}

bool DataAccess::GFileIODevice::has_been_updated()
{
    return _last_update_time != QFileInfo(FileName()).lastModified();
}

bool DataAccess::GFileIODevice::HasDataAvailable()
{
    return has_been_updated();
}

QFile &DataAccess::GFileIODevice::File() const
{
    return (QFile &)IODevice();
}

void DataAccess::GFileIODevice::SetFileName(const QString &filename)
{
    if(FileName().length() > 0)
        _file_watcher->removePath(FileName());

    File().setFileName(filename);

    if(FileName().length() > 0)
    {
        QFile f(FileName());

        // Create the file if it doesn't exist
        if(!f.exists())
        {
            f.open(QFile::WriteOnly);
            f.close();
        }

        _file_watcher->addPath(FileName());
    }
}

QString DataAccess::GFileIODevice::FileName() const
{
    return File().fileName();
}

QByteArray DataAccess::GFileIODevice::FileData()
{
    return ReceiveData(false);
}

void DataAccess::GFileIODevice::TruncateFile()
{
    FailIfReadOnly();

    WriteModeEnum _md = GetWriteMode();
    SetWriteMode(WriteOver);
    Write("");
    SetWriteMode(_md);
}

void DataAccess::GFileIODevice::_open_file(bool for_write)
{
    QFile::OpenMode flags;

    if(for_write)
    {
        flags = QFile::WriteOnly;

        if(_write_mode == WriteAppend)
            flags |= QFile::Append;
        else if(_write_mode == WriteOver)
            flags |= QFile::Truncate;
        else
        {
            THROW_GUTIL_EXCEPTION( Core::NotImplementedException() )
        }
    }
    else
    {
        flags = QFile::ReadOnly;
    }

    File().open(flags);

    if(!File().isOpen())
    {
        Core::DataTransportException ex(
                QString("Could not open file: %1").arg(FileName()).toStdString());
        ex.SetData("err", File().errorString().toStdString());
        THROW_GUTIL_EXCEPTION( ex )
    }
}

void DataAccess::GFileIODevice::_close_file()
{
    File().close();
}
