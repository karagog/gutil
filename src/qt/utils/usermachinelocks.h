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

#ifndef APPLICATIONLOCK_H
#define APPLICATIONLOCK_H

#include "gutil_exception.h"
#include <QString>
#include <QMap>
#include <QReadWriteLock>
#include <QSemaphore>

class QtLockedFile;

NAMESPACE_GUTIL2(QT, Utils);


/** An easy to use inter-process locking mechanism.
    Classes can implement this base to achieve locking across the
    machine for the machine user who invokes it.
    \note Normally you don't use this directly.  Use UserMachineMutex or
    UserMachineReadWriteLock for normal cases.
    \sa UserMachineReadWriteLock, UserMachineMutex
*/
class MachineLockBase
{
public:

    void SetUserMachineLockIdentifier(const QString &identifier, const QString &modifier);
    void SetUserMachineLockFileName(const QString &);

    void UnlockForMachine();
    bool IsLockedOnMachine() const{ return GetLockOwner(); }

    QString FileNameForMachineLock() const;

    PROPERTY( StringModifier, QString );
    PROPERTY( LockOwner, bool );
    PROPERTY( ReadLockOwner, bool );


protected:

    /** The unique modifier is basically a stamp that derived classes want
        put on the filenames of all the lock files they create
    */
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
    void _release_lock(){
        _get_lock_reference().unlock();
        SetLockOwner(false);
    }

    void _pre_init();
    void _post_init();

    QReadWriteLock &_get_lock_reference(){
        process_locks_lock.lockForRead();

        QString s = FileNameForMachineLock();
        if(!process_locks.contains(s))
            process_locks.insert(s, new QReadWriteLock);

        QReadWriteLock *ret = process_locks.value(s);

        process_locks_lock.unlock();
        return *ret;
    }

    // All of these objects share these global variables
    static QMap<QString, QReadWriteLock *> process_locks;
    static QReadWriteLock process_locks_lock;
    static QSemaphore process_locks_sem;

};



/** A read/write lock that can lock a file for the whole operating system.
    It can be used as a process lock, if you inject an application identifier,
    and then you can lock between processes.
    \note The file lock doesn't work if it's on a network drive.
*/
class UserMachineReadWriteLock :
        public MachineLockBase
{
public:

    /** Locks using an application identifier and modifier.
        An example would be: UserMachineMutex("MyApplication", "Database");
    */
    UserMachineReadWriteLock(const QString &identifier, const QString &modifier)
        :MachineLockBase("MACHINE_RW_LOCK", identifier, modifier){}

    explicit UserMachineReadWriteLock(const QString &file_name)
        :MachineLockBase("MACHINE_RW_LOCK", file_name){}

    /** Locks this object for read on the user's machine */
    void LockForReadOnMachine(bool block = false){
        lock(true, block);
    }

    /** Locks this object for write on the user's machine */
    void LockForWriteOnMachine(bool block = false){
        lock(false, block);
    }

    /** Returns whether we currently hold a read lock. */
    bool IsLockedForReadOnMachine() const{
        return GetLockOwner() && GetReadLockOwner();
    }

    /** Returns whether we currently hold a write lock. */
    bool IsLockedForWriteOnMachine() const{
        return GetLockOwner() && !GetReadLockOwner();
    }

};



/** A read/write lock that can lock a file for the whole operating system.
    It can be used as a process lock, if you inject an application identifier,
    and then you can lock between processes.
    \note The file lock doesn't work if it's on a network drive.
*/
class UserMachineMutex :
        public MachineLockBase
{
public:

    /** Locks using an application identifier and modifier.
        An example would be: UserMachineMutex("MyApplication", "Database");
    */
    UserMachineMutex(const QString &identifier, const QString &modifier)
        :MachineLockBase("MACHINE_LOCK", identifier, modifier){}

    /** Locks a specific file. */
    explicit UserMachineMutex(const QString &file_name)
        :MachineLockBase("MACHINE_LOCK", file_name){}

    void LockMutexOnMachine(bool block = false){
        lock(false, block);
    }

};


END_NAMESPACE_GUTIL2;

#endif // APPLICATIONLOCK_H

#endif // GUI_FUNCTIONALITY
