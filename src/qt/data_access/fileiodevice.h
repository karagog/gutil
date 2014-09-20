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

#ifndef GUTIL_FILETRANSPORT_H
#define GUTIL_FILETRANSPORT_H

#include "gutil_exception.h"
#include "gutil_qt_iodevice.h"
#include "gutil_usermachinelocks.h"
#include <QDateTime>
#include <QFile>

class QFileSystemWatcher;

namespace GUtil{ namespace QT{


/** A mechanism for exchanging data with a file.

    This class has a read/write lock for the file, so separate processes
    on the same machine will have controlled access to the file, but
    processes on separate machines will not.

    \todo Implement one that locks across machines too.
*/
class FileIODevice :
        public QT_IODevice
{
    Q_OBJECT
public:

    explicit FileIODevice(const QString &filename = QString::null,
                           QObject *parent = 0);

    enum WriteModeEnum
    {
        WriteAppend,
        WriteOver
    };

    PROPERTY(WriteMode, WriteModeEnum);

    void SetFileName(const QString &);
    QString FileName() const{ return File().fileName(); }
    QByteArray FileData();

    void TruncateFile();


protected:

    virtual void send_data(const QByteArray &);
    virtual QByteArray receive_data();

    QFile &File(){ return (QFile &)GetIODevice(); }
    const QFile &File() const{ return (const QFile &)GetIODevice(); }

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

    UserMachineReadWriteLock _machine_lock;

};

}}

#endif // GUTIL_FILETRANSPORT_H

#endif // GUI_FUNCTIONALITY
