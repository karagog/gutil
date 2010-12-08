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

#ifndef FILETRANSPORT_H
#define FILETRANSPORT_H

#include "DataAccess/gqiodevice.h"
#include <QDateTime>
#include <QFile>

class QFileSystemWatcher;

namespace GUtil
{
    namespace DataAccess
    {
        // A mechanism for exchanging data with a file
        class GFileIODevice :
                public GQIODevice
        {
            Q_OBJECT
        public:
            explicit GFileIODevice(const QString &filename = QString::null, QObject *parent = 0);

            enum WriteModeEnum
            {
                WriteAppend,
                WriteOver
            };

            void SetWriteMode(WriteModeEnum);
            inline WriteModeEnum GetWriteMode() const { return _write_mode; }

            virtual bool HasDataAvailable() const;

            void SetFileName(const QString &);
            inline QString FileName() const{ return File().fileName(); }
            QByteArray FileData();

            void TruncateFile();


        protected:
            virtual void send_data(const QByteArray &)
                    throw(GUtil::Core::DataTransportException);
            virtual QByteArray receive_data()
                    throw(GUtil::Core::DataTransportException);

            inline QFile &File(){ return (QFile &)IODevice(); }
            inline const QFile &File() const{ return (const QFile &)IODevice(); }

            // Has the file been updated since we've seen it?
            bool has_been_updated() const;

        private:
            WriteModeEnum _write_mode;

            QFileSystemWatcher *_file_watcher;
            QDateTime _last_update_time;

            void _open_file(bool for_write);
            void _close_file();
        };
    }
}

#endif // FILETRANSPORT_H
