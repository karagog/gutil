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

#ifndef GSHAREDLOCK_H
#define GSHAREDLOCK_H

#include <QSharedData>
#include <QReadWriteLock>
#include <QExplicitlySharedDataPointer>

namespace GUtil
{
    namespace Custom
    {
        // This implements an explicitly shared read-write lock, so you can lock
        //  against a global lock, and also access the convenient 'IsLocked' functions
        //  which only belong to a specific instance

        class GSharedLock
        {
        public:
            GSharedLock();
            GSharedLock(const GSharedLock &);

            // Detaches our copy of the shared lock and creates a new lock
            void Detach();

            GSharedLock &operator =(const GSharedLock &);

            void Lock();
            void LockForRead();
            void LockForWrite();

            inline bool TryLock(int timeout = -1);
            bool TryLockForRead(int timeout = -1);
            bool TryLockForWrite(int timeout = -1);

            void Unlock();

            inline bool IsLocked() const;
            inline bool IsLockedForRead() const;
            inline bool IsLockedForWrite() const;

        private:

            class LockData : public QSharedData, public QReadWriteLock
            {
            public:

                LockData() : QSharedData(), QReadWriteLock(QReadWriteLock::NonRecursive){}
                LockData(const LockData &o)
                    : QSharedData(o), QReadWriteLock(QReadWriteLock::NonRecursive){}
            };

            QExplicitlySharedDataPointer<LockData> _lock;

            bool _i_own_readlock;
            bool _i_own_writelock;

            void _init();
            void _clear_flags();

        };
    }
}

#endif // GSHAREDLOCK_H
