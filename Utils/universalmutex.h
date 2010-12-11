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

#ifndef UNIVERSALMUTEX_H
#define UNIVERSALMUTEX_H

#include "usermachinelocks.h"
#include <QUuid>
#include <QDateTime>
#include <QFileSystemWatcher>
#include <QWaitCondition>
#include <QReadWriteLock>
#include <QThread>


namespace GUtil
{
    namespace Utils
    {
        // This class provides mutually exclusive access to a file.  The file can be on the current machine
        //  or exist on a network drive, but as long as everyone accessing it locks it with this Universal Mutex
        //  class the access will be controlled

        class UniversalMutex :
                public QThread
        {
            Q_OBJECT
        public:

            UniversalMutex(const QString &file_path);
            ~UniversalMutex();

            // If you don't block, then it will throw a LockException if lock fails
            void Lock(bool block = true)
                    throw(Core::LockException);
            void Unlock();

            void SetFilePath(const QString &fp);
            QString GetFilepath() const;

            // If you pass 'false', it will take significantly longer to determine if it has the lock,
            //  but it will be the most up-to-date
            bool HasLock(bool from_cache = true);


        signals:

            // This is a bad signal, and means that for one reason or another
            //  you have lost the lock on the file
            void NotifyLostLock();


        protected:

            void run();


        private slots:

            void lock_file_updated();


        private:

            UserMachineMutex _machine_mutex;
            QUuid _id;
            QString _lock_file_path;
            QDateTime _last_read_date;

            // These data objects are used to check for updates and wait on the update condition
            QFileSystemWatcher _fsw;
            QReadWriteLock _lockfile_lock;
            QWaitCondition _condition_file_updated;

            void _lock(bool block);
            void _unlock();

            bool has_lock(bool) const;

            void _fail_if_locked() const;
            inline bool _lock_file_set() const{ return _lock_file_path.length() > 0; }

            bool _is_locked;

        };
    }
}

#endif // UNIVERSALMUTEX_H
