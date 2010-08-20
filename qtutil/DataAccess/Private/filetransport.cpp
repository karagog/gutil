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
#include "exception.h"
#include <QFileInfo>
#include <QFileSystemWatcher>
#include <QCryptographicHash>
using namespace GQtUtil::DataAccess::Private;

FileTransport::FileTransport(const QString &filename, QObject *parent)
    :ITransportMechanism(parent)
{
    _lf->setFileName(filename);

    _file_watcher = new QFileSystemWatcher(this);
    _file_watcher->addPath(filename);

    connect(_file_watcher, SIGNAL(fileChanged(QString)), this, SLOT(update_has_data_available()));

    update_has_data_available();
}

void FileTransport::send_data(const QByteArray &data)
{
    _open_and_lock_file(true);

    if(!_lf->resize(0))
    {
        _unlock_and_close_file();
        throw GUtil::Exception(_lf->errorString().toStdString());
    }

    // At this point the settings file is ours for sole writing
    if(data.length() != _lf->write(data))
    {
        QString err = _lf->errorString();
        _unlock_and_close_file();
        throw GUtil::Exception(QString("Couldn't write all the data to the file: %1")
                               .arg(err).toStdString());
    }

    _unlock_and_close_file();
}

QByteArray FileTransport::receive_data()
{
    _open_and_lock_file(false);

    QByteArray dat = _lf->readAll();

    _unlock_and_close_file();

    return dat;
}

void FileTransport::updateHasDataAvailable(bool &has_data_variable)
{
    has_data_variable = false;

    _open_and_lock_file(false);

    QFileInfo fi(*_lf);
    if(_last_update_time != fi.lastModified())
    {
        _last_update_time = fi.lastModified();

        // The modify times don't match, so read the data and determine if it's new or not
        QByteArray tmphash = QCryptographicHash::hash(_lf->readAll(), QCryptographicHash::Md5);

        if(tmphash != _hash)
        {
            _hash = tmphash;
            has_data_variable = true;
        }
    }

    _unlock_and_close_file();
}

QString FileTransport::fileName() const
{
    return _lf->fileName();
}

void FileTransport::_open_and_lock_file(bool for_write)
{
    if(!_lf->open(QtLockedFile::ReadWrite))
        throw GUtil::Exception(_lf->errorString().toStdString());

    if(!_lf->lock(for_write ? QtLockedFile::WriteLock : QtLockedFile::ReadLock))
    {
        _lf->close();
        throw GUtil::Exception(_lf->errorString().toStdString());
    }
}

void FileTransport::_unlock_and_close_file()
{
    _lf->unlock();
    _lf->close();
}
