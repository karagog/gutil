/*Copyright 2010-2013 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_NO_GUI_FUNCTIONALITY

#include "fileiodevice.h"
#include <QFileInfo>
#include <QDir>
#include <QFileSystemWatcher>
#include <QCryptographicHash>

NAMESPACE_GUTIL1(QT);


FileIODevice::FileIODevice(const QString &filename, QObject *parent)
    :QT_IODevice(new QFile, parent),
      _machine_lock(filename)
{
    _file_watcher = new QFileSystemWatcher(this);
    connect(_file_watcher, SIGNAL(fileChanged(QString)),
            this, SLOT(raiseReadyRead()));

    SetFileName(filename);
    SetWriteMode(WriteAppend);
}

void FileIODevice::send_data(const QByteArray &data)
{
    lock_file(true);
    _open_file(true);

    // At this point the settings file is ours for sole writing
    QT_IODevice::send_data(data);

    _close_file();
    unlock_file();
}

QByteArray FileIODevice::receive_data()
{
    lock_file(false);
    _open_file(false);

    QByteArray ret = QT_IODevice::receive_data();

    _close_file();
    unlock_file();

    _last_update_time = QFileInfo(FileName()).lastModified();

    return ret;
}

bool FileIODevice::has_been_updated() const
{
    return _last_update_time != QFileInfo(FileName()).lastModified();
}

bool FileIODevice::has_data_available()
{
    return has_been_updated();
}

void FileIODevice::SetFileName(const QString &filename)
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

            if(!f.open(QFile::WriteOnly))
                THROW_NEW_GUTIL_EXCEPTION2(Exception, f.errorString().toUtf8());
            f.close();
        }

        _file_watcher->addPath(filename);
    }
}

QByteArray FileIODevice::FileData()
{
    return ReceiveData();
}

void FileIODevice::TruncateFile()
{
    FailIfReadOnly();

    WriteModeEnum _md = GetWriteMode();
    SetWriteMode(WriteOver);
    Write("");
    SetWriteMode(_md);
}

void FileIODevice::_open_file(bool for_write)
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
            THROW_NEW_GUTIL_EXCEPTION( NotImplementedException);
    }
    else
    {
        flags = QFile::ReadOnly;
    }

    File().open(flags);

    if(!File().isOpen())
    {
        DataTransportException<true> ex(
                QString("Could not open file: %1").arg(FileName()).toAscii().constData());
        ex.SetData("err", File().errorString().toAscii().constData());
        THROW_GUTIL_EXCEPTION( ex );
    }
}

void FileIODevice::_close_file()
{
    File().close();
}

void FileIODevice::lock_file(bool write)
{
    if(write)
        _machine_lock.LockForWriteOnMachine(true);
    else
        _machine_lock.LockForReadOnMachine(true);
}

void FileIODevice::unlock_file()
{
    _machine_lock.UnlockForMachine();
}


END_NAMESPACE_GUTIL1;

#endif // GUI_FUNCTIONALITY
