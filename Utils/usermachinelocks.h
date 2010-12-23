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
#include <QString>
#include <QMap>
#include <QReadWriteLock>
#include <QSemaphore>

class QtLockedFile;

GUTIL_BEGIN_NAMESPACE( Utils );


// An easy to use inter-process locking mechanism.

// Classes can implement this interface to achieve locking across the
//    machine for the user who invokes it
class MachineLockBase
{
public:

    void SetUserMachineLockIdentifier(const QString &identifier, const QString &modifier);
    void SetUserMachineLockFileName(const QString &);

    void UnlockForMachine();
    inline bool IsLockedOnMachine() const{ return GetLockOwner(); }

    QString FileNameForMachineLock() const;

    PROPERTY( StringModifier, QString );
    PROPERTY( LockOwner, bool );
    PROPERTY( ReadLockOwner, bool );


protected:

    // The unique modifier is basically a stamp that derived classes want
    //  put on the filenames of all the lock files they create
    explicit MachineLockBase(const QString &unique_modifier,
                             const QString &identifier,
                             const QString &modifier);
    explicit MachineLockBase(const QString &unique_modifier,
                             const QString &file_name = QString::null);
    virtual ~MachineLockBase();

    void lock(bool for_read, bool block);


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
    static QSemaphore process_locks_sem;

};



class UserMachineReadWriteLock :
        public MachineLockBase
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

    inline bool IsLockedForReadOnMachine() const{
        return GetLockOwner() && GetReadLockOwner();
    }
    inline bool IsLockedForWriteOnMachine() const{
        return GetLockOwner() && !GetReadLockOwner();
    }

};



class UserMachineMutex :
        public MachineLockBase
{
public:

    explicit UserMachineMutex(const QString &identifier, const QString &modifier);
    explicit UserMachineMutex(const QString &file_name = QString::null);

    void LockMutexOnMachine(bool block = false)
            throw(GUtil::Core::LockException,
                  GUtil::Core::Exception);

};


GUTIL_END_NAMESPACE;

#endif // APPLICATIONLOCK_H
