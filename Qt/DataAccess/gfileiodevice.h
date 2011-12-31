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

#ifndef GUTIL_FILETRANSPORT_H
#define GUTIL_FILETRANSPORT_H

#include "Core/extendedexception.h"
#include "DataAccess/gqiodevice.h"
#include "Utils/usermachinelocks.h"
#include <QDateTime>
#include <QFile>

class QFileSystemWatcher;

namespace GUtil{ namespace QT{ namespace DataAccess{


/** A mechanism for exchanging data with a file.

    This class has a read/write lock for the file, so separate processes
    on the same machine will have controlled access to the file, but
    processes on separate machines will not.

    \todo Implement one that locks across machines too.
*/
class GFileIODevice :
        public GQIODevice
{
    Q_OBJECT
public:

    explicit GFileIODevice(const QString &filename = QString::null,
                           QObject *parent = 0);

    enum WriteModeEnum
    {
        WriteAppend,
        WriteOver
    };

    PROPERTY(WriteMode, WriteModeEnum);

    void SetFileName(const QString &);
    inline QString FileName() const{ return File().fileName(); }
    QByteArray FileData();

    void TruncateFile();


protected:

    virtual void send_data(const QByteArray &);
    virtual QByteArray receive_data();

    inline QFile &File(){ return (QFile &)IODevice(); }
    inline const QFile &File() const{ return (const QFile &)IODevice(); }

    bool has_data_available();

    // Has the file been updated since we've seen it?
    bool has_been_updated() const;

    virtual void lock_file(bool write);
    virtual void unlock_file();


private:

    QFileSystemWatcher *_file_watcher;
    QDateTime _last_update_time;

    void _open_file(bool for_write);
    void _close_file();

    Utils::UserMachineReadWriteLock _machine_lock;

};

}}}

#endif // GUTIL_FILETRANSPORT_H

#endif // GUI_FUNCTIONALITY
