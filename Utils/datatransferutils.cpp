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

#include "datatransferutils.h"
#include "Core/extendedexception.h"
#include <QFile>
GUTIL_USING_NAMESPACE(Utils);
GUTIL_USING_NAMESPACE(Core);

#define CHUNK_SIZE 4096

void DataTransferUtils::WriteDataToFileInChunks(const QString &filename,
                                                const QByteArray &data,
                                                GUtil::Utils::PubSubSystem *PubSub,
                                                int progress_id,
                                                bool *cancel_flag)
{
    bool tmp(false);
    if(!cancel_flag)
        cancel_flag = &tmp;

    QFile f(filename);
    if(!f.open(QFile::WriteOnly | QFile::Truncate))
    {
        THROW_NEW_GUTIL_EXCEPTION2(Exception,
                                   QString("Could not open file: %1")
                                   .arg(f.errorString()).toAscii().constData());
    }

    int bytes_written(0);
    while(!*cancel_flag && bytes_written < data.length())
    {
        int bytes_remaining = data.length() - bytes_written;
        int this_run = bytes_remaining < CHUNK_SIZE ?
                       bytes_remaining : CHUNK_SIZE;

        f.write(data.constData() + bytes_written, this_run);

        bytes_written += this_run;

        if(PubSub)
            PubSub->PublishProgress((100 * bytes_written) / data.length(),
                                    progress_id);
    }

    if(bytes_written != data.length())
    {
        QString err(*cancel_flag ? "Cancelled" : f.errorString());
        f.close();
        THROW_NEW_GUTIL_EXCEPTION2(Exception,
                                   QString("Write Failed: %1")
                                   .arg(err).toAscii().constData());
    }
    f.close();
}

void DataTransferUtils::CopyFileInChunks(const QString &source,
                      const QString &dest,
                      GUtil::Utils::PubSubSystem *PubSub,
                      int progress_id,
                      bool *cancel_flag)
{
    bool tmp(false);
    if(!cancel_flag)
        cancel_flag = &tmp;

    QFile s(source);
    QFile d(dest);
    if(!d.open(QFile::WriteOnly | QFile::Truncate))
    {
        THROW_NEW_GUTIL_EXCEPTION2(Exception,
                                   QString("Could not open file for writing: %1")
                                   .arg(d.errorString()).toAscii().constData());
    }
    if(!s.open(QFile::ReadOnly))
    {
        d.close();
        THROW_NEW_GUTIL_EXCEPTION2(Exception,
                                   QString("Could not open file for reading: %1")
                                   .arg(s.errorString()).toAscii().constData());
    }

    int data_length(s.size());
    int bytes_written(0);
    while(!*cancel_flag && bytes_written < data_length)
    {
        int bytes_remaining = data_length - bytes_written;
        int this_run = bytes_remaining < CHUNK_SIZE ?
                       bytes_remaining : CHUNK_SIZE;

        QByteArray dat(s.read(this_run));
        d.write(dat);

        bytes_written += dat.length();

        if(PubSub)
            PubSub->PublishProgress((100 * bytes_written) / dat.length(),
                                    progress_id);

        if(this_run != dat.length())
            break;
    }

    if(bytes_written != data_length)
    {
        QString err(*cancel_flag ?"Cancelled" :
                    QString("%1 :: %2")
                    .arg(d.errorString())
                    .arg(s.errorString()));
        d.close();
        s.close();
        THROW_NEW_GUTIL_EXCEPTION2(Exception,
                                   QString("Write Failed: %1")
                                   .arg(err).toAscii().constData());
    }
    d.close();
    s.close();
}

QByteArray DataTransferUtils::ReadDataFromFileInChunks(const QString &filename,
                                                       GUtil::Utils::PubSubSystem *PubSub,
                                                       int progress_id,
                                                       bool *cancel_flag)
{
    bool tmp(false);
    if(!cancel_flag)
        cancel_flag = &tmp;

    QFile f(filename);
    if(!f.open(QFile::ReadOnly))
    {
        THROW_NEW_GUTIL_EXCEPTION2(Exception,
                                   QString("Could not open file: %1")
                                   .arg(f.errorString()).toAscii().constData());
    }

    QByteArray ret;
    const int bytes_to_read(f.size());
    int bytes_read(0);
    while(!*cancel_flag && bytes_read < bytes_to_read)
    {
        int bytes_remaining = bytes_to_read - bytes_read;
        int this_run = bytes_remaining < CHUNK_SIZE ?
                       bytes_remaining : CHUNK_SIZE;

        ret.append(f.read(this_run));

        bytes_read += this_run;

        if(PubSub)
            PubSub->PublishProgress((100 * bytes_read) / bytes_to_read,
                                    progress_id);
    }

    if(bytes_read != bytes_to_read)
    {
        QString err(*cancel_flag ? "Cancelled" : f.errorString());
        f.close();
        THROW_NEW_GUTIL_EXCEPTION2(Exception,
                                   QString("Write Failed: %1")
                                   .arg(err).toAscii().constData());
    }
    f.close();
    return ret;
}
