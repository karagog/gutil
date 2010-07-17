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
using namespace GQtUtil::DataAccess::Private::Transports;

FileTransport::FileTransport(const QString &filename)
{
    _lf->setFileName(filename);
}

void FileTransport::exportData(const QByteArray &data)
{
    if(!_lf->open(QtLockedFile::ReadWrite))
        throw GUtil::Exception(_lf->errorString().toStdString());

    if(!_lf->lock(QtLockedFile::WriteLock))
    {
        _lf->close();
        throw GUtil::Exception(_lf->errorString().toStdString());
    }

    if(!_lf->resize(0))
    {
        _lf->close();
        throw GUtil::Exception(_lf->errorString().toStdString());
    }

    // At this point the settings file is ours for sole writing
    if(data.length() != _lf->write(data))
    {
        QString err = _lf->errorString();
        _lf->close();
        throw GUtil::Exception(QString("Couldn't write all the data to the file: %1")
                               .arg(err).toStdString());
    }

    _lf->unlock();
    _lf->close();
}

QByteArray FileTransport::importData()
{
    if(!_lf->open(QtLockedFile::ReadWrite))
        throw GUtil::Exception(_lf->errorString().toStdString());

    if(!_lf->lock(QtLockedFile::ReadLock))
        throw GUtil::Exception(_lf->errorString().toStdString());

    QByteArray dat = _lf->readAll();

    _lf->unlock();
    _lf->close();

    return dat;
}

QString FileTransport::fileName() const
{
    return _lf->fileName();
}
