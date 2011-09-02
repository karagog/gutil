/*Copyright 2011 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifdef GUI_FUNCTIONALITY

#include "gfileiodevice.h"
#include "Core/exception.h"
#include "gassert.h"
#include <QFileInfo>
#include <QDir>
#include <QFileSystemWatcher>
#include <QCryptographicHash>
using namespace GUtil;
using namespace std;

DataAccess::GFileIODevice::GFileIODevice(const QString &filename, QObject *parent)
    :GQIODevice(new QFile, parent),
      _machine_lock(filename)
{
    // We will assume ownership of the io device
    IODevice().setParent(this);

    _file_watcher = new QFileSystemWatcher(this);
    connect(_file_watcher, SIGNAL(fileChanged(QString)),
            this, SLOT(raiseReadyRead()));

    SetFileName(filename);
    SetWriteMode(WriteAppend);
}

void DataAccess::GFileIODevice::send_data(const QByteArray &data)
        throw(Core::DataTransportException)
{
    lock_file(true);
    _open_file(true);

    // At this point the settings file is ours for sole writing
    GQIODevice::send_data(data);

    _close_file();
    unlock_file();
}

QByteArray DataAccess::GFileIODevice::receive_data()
        throw(Core::DataTransportException)
{
    lock_file(false);
    _open_file(false);

    QByteArray ret = DataAccess::GQIODevice::receive_data();

    _close_file();
    unlock_file();

    _last_update_time = QFileInfo(FileName()).lastModified();

    return ret;
}

bool DataAccess::GFileIODevice::has_been_updated() const
{
    return _last_update_time != QFileInfo(FileName()).lastModified();
}

bool DataAccess::GFileIODevice::has_data_available()
{
    return has_been_updated();
}

void DataAccess::GFileIODevice::SetFileName(const QString &filename)
{
    if(filename == FileName())
        return;

    if(FileName().length() > 0)
        _file_watcher->removePath(FileName());

    File().setFileName(filename);
    _machine_lock.SetUserMachineLockFileName(filename);

    if(filename.length() > 0)
    {
        QFile f(filename);

        // Create the file if it doesn't exist
        if(!f.exists())
        {
            QDir d;
            QFileInfo fi(f);
            if(!d.exists(fi.path()))
                d.mkpath(fi.path());

            bool res( f.open(QFile::WriteOnly) );
            f.close();

            GASSERT(res);
        }

        _file_watcher->addPath(filename);
    }
}

QByteArray DataAccess::GFileIODevice::FileData()
{
    return ReceiveData();
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

        if(GetWriteMode() == WriteAppend)
            flags |= QFile::Append;
        else if(GetWriteMode() == WriteOver)
            flags |= QFile::Truncate;
        else
            THROW_NEW_GUTIL_EXCEPTION2( Core::NotImplementedException, "" );
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
        THROW_GUTIL_EXCEPTION( ex );
    }
}

void DataAccess::GFileIODevice::_close_file()
{
    File().close();
}

void DataAccess::GFileIODevice::lock_file(bool write)
{
    if(write)
        _machine_lock.LockForWriteOnMachine(true);
    else
        _machine_lock.LockForReadOnMachine(true);
}

void DataAccess::GFileIODevice::unlock_file()
{
    _machine_lock.UnlockForMachine();
}

#endif // GUI_FUNCTIONALITY
