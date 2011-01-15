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
#include "Core/exception.h"
#include <QFile>
GUTIL_USING_NAMESPACE(Utils);
GUTIL_USING_NAMESPACE(Core);

#define CHUNK_SIZE 4096

void DataTransferUtils::WriteDataToFileInChunks(const QString &filename,
                                                const QByteArray &data,
                                                GUtil::Utils::PubSubSystem *PubSub,
                                                int progress_id)
{
    QFile f(filename);
    if(!f.open(QFile::WriteOnly | QFile::Truncate))
    {
        THROW_NEW_GUTIL_EXCEPTION2(Exception,
                                   QString("Could not open file: %1")
                                   .arg(f.errorString()).toStdString());
    }

    int bytes_written(0);
    while(bytes_written < data.length())
    {
        int bytes_remaining = data.length() - bytes_written;
        int this_run = bytes_remaining < CHUNK_SIZE ?
                       bytes_remaining : CHUNK_SIZE;

        f.write(data.constData() + bytes_written, this_run);

        bytes_written += this_run;

        PubSub->PublishProgress((100 * bytes_written) / data.length(),
                                progress_id);
    }

    if(bytes_written != data.length())
    {
        QString err(f.errorString());
        f.close();
        THROW_NEW_GUTIL_EXCEPTION2(Exception,
                                   QString("Write Failed: %1")
                                   .arg(err).toStdString());
    }
    f.close();
}

QByteArray DataTransferUtils::ReadDataFromFileInChunks(const QString &filename,
                                                       GUtil::Utils::PubSubSystem *PubSub,
                                                       int progress_id)
{
    QFile f(filename);
    if(!f.open(QFile::ReadOnly))
    {
        THROW_NEW_GUTIL_EXCEPTION2(Exception,
                                   QString("Could not open file: %1")
                                   .arg(f.errorString()).toStdString());
    }

    QByteArray ret;
    const int bytes_to_read(f.size());
    int bytes_read(0);
    while(bytes_read < bytes_to_read)
    {
        int bytes_remaining = bytes_to_read - bytes_read;
        int this_run = bytes_remaining < CHUNK_SIZE ?
                       bytes_remaining : CHUNK_SIZE;

        ret.append(f.read(this_run));

        bytes_read += this_run;

        PubSub->PublishProgress((100 * bytes_read) / bytes_to_read,
                                progress_id);
    }

    if(bytes_read != bytes_to_read)
    {
        QString err(f.errorString());
        f.close();
        THROW_NEW_GUTIL_EXCEPTION2(Exception,
                                   QString("Write Failed: %1")
                                   .arg(err).toStdString());
    }
    f.close();
    return ret;
}
