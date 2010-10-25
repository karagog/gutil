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

#ifndef APPLICATIONLOCK_H
#define APPLICATIONLOCK_H

#include "Core/exception.h"
#include "Custom/gsemaphore.h"
#include <QString>
#include <QMap>
#include <QReadWriteLock>

class QtLockedFile;

namespace GUtil
{
    namespace Utils
    {
        // An easy to use inter-process locking mechanism.

        // Classes can implement this interface to achieve locking across the
        //    machine for the user who invokes it
        class MachineLockBase
        {
        public:

            void SetUserMachineLockIdentifier(const QString &identifier, const QString &modifier);
            void SetUserMachineLockFileName(const QString &);

            void UnlockForMachine();
            bool IsLockedOnMachine() const;

            QString FileNameForMachineLock() const;

        protected:

            explicit MachineLockBase(const QString &identifier, const QString &modifier);
            explicit MachineLockBase(const QString &file_name = QString::null);
            virtual ~MachineLockBase();

            void lock(bool for_read, bool block);

            bool _i_own_lock;
            bool _i_have_read_lock;

            // identifiers are modified with this modifier, so there are no naming collisions
            virtual QString string_modifier() const = 0;

        private:
            QtLockedFile *_usermachinelockfile;

            void _grab_lock_in_process(bool for_read, bool block);
            void _release_lock();

            void _pre_init();
            void _post_init();

            QReadWriteLock &_get_lock_reference();

            // All of these objects share these global variables
            static QMap<QString, QReadWriteLock *> process_locks;
            static QReadWriteLock process_locks_lock;
            static GUtil::Custom::GSemaphore process_locks_sem;
        };



        class UserMachineReadWriteLock : public MachineLockBase
        {
        public:
            explicit UserMachineReadWriteLock(const QString &identifier, const QString &modifier);
            explicit UserMachineReadWriteLock(const QString &file_name = QString::null);

            // Use these functions to lock/unlock this object for the user's machine
            void LockForReadOnMachine(bool block = false)
                    throw(GUtil::Core::LockException,
                          GUtil::Core::Exception);
            void LockForWriteOnMachine(bool block = false)
                    throw(GUtil::Core::LockException,
                          GUtil::Core::Exception);

            bool IsLockedForReadOnMachine() const;
            bool IsLockedForWriteOnMachine() const;

        protected:
            virtual QString string_modifier() const;

        };



        class UserMachineMutex : public MachineLockBase
        {
        public:
            explicit UserMachineMutex(const QString &identifier, const QString &modifier);
            explicit UserMachineMutex(const QString &file_name = QString::null);

            void LockMutexOnMachine(bool block = false)
                    throw(GUtil::Core::LockException,
                          GUtil::Core::Exception);

        protected:
            virtual QString string_modifier() const;

        };
    }
}

#endif // APPLICATIONLOCK_H
