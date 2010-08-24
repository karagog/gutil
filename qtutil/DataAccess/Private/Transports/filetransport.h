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

#include "Interfaces/ITransportMechanism.h"
#include <QDateTime>

class QtLockedFile;
class QFileSystemWatcher;

namespace GQtUtil
{
    namespace DataAccess
    {
        namespace Private
        {
            // A simple mechanism for exchanging data with a file
            class FileTransport : public Interfaces::ITransportMechanism
            {
            public:
                FileTransport(const QString &, QObject *parent = 0);

                QString fileName() const;
                QByteArray fileData();

                void reload();

            protected:
                virtual void send_data(const QByteArray &);
                virtual QByteArray receive_data();

                virtual void update_has_data_variable(bool &has_data_variable);

            private:
                QtLockedFile *_lf;

                QFileSystemWatcher *_file_watcher;

                QDateTime _last_update_time;
                QByteArray _hash;

                void _open_and_lock_file(bool for_write);
                void _unlock_and_close_file();
            };
        }
    }
}

#endif // FILETRANSPORT_H
