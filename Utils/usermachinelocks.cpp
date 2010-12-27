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

#include "usermachinelocks.h"
#include "ThirdParty/QtLockedFile/qtlockedfile.h"
#include <QFileInfo>
#include <QDesktopServices>
#include <QHostInfo>
using namespace GUtil;

QMap<QString, QReadWriteLock *> Utils::MachineLockBase::process_locks;
QReadWriteLock Utils::MachineLockBase::process_locks_lock;
QSemaphore Utils::MachineLockBase::process_locks_sem;

Utils::MachineLockBase::MachineLockBase(const QString &u, const QString &id, const QString &modifier)
    :_p_StringModifier(u),
    _p_LockOwner(false),
    _p_ReadLockOwner(false)
{
    _pre_init();

    SetUserMachineLockIdentifier(id, modifier);

    _post_init();
}

Utils::MachineLockBase::MachineLockBase(const QString &u, const QString &file_name)
    :_p_StringModifier(u),
    _p_LockOwner(false),
    _p_ReadLockOwner(false)
{
    _pre_init();

    SetUserMachineLockFileName(file_name);

    _post_init();
}

void Utils::MachineLockBase::_pre_init()
{
    _usermachinelockfile = new QtLockedFile();
}

void Utils::MachineLockBase::_post_init()
{
    process_locks_lock.lockForWrite();
    process_locks_sem.release();    // Up the semaphore
    process_locks_lock.unlock();
}

Utils::MachineLockBase::~MachineLockBase()
{
    UnlockForMachine();

    delete _usermachinelockfile;

    process_locks_lock.lockForWrite();
    process_locks_sem.acquire();    // Down the semaphore

    // Clean up the locks if i'm the last one dying
    if(process_locks_sem.available() == 0)
    {
        foreach(QString k, process_locks.keys())
            delete process_locks.value(k);
    }

    process_locks_lock.unlock();
}

void Utils::MachineLockBase::SetUserMachineLockIdentifier(
        const QString &identifier,
        const QString &modifier)
{
    if(identifier.length() == 0)
        return;

    SetUserMachineLockFileName(QString("%1/%2%3.%4.%5")
                               .arg(QDesktopServices::storageLocation(QDesktopServices::DataLocation))
                               .arg(identifier)
                               .arg(modifier.length() != 0 ?
                                    QString(".%1").arg(modifier) :
                                    QString::null)
                               .arg(QHostInfo::localHostName())
                               .arg(GetStringModifier()));
}

void Utils::MachineLockBase::SetUserMachineLockFileName(const QString &fn)
{
    if(fn == FileNameForMachineLock())
        return;

    UnlockForMachine();

    delete _usermachinelockfile;
    _usermachinelockfile = new QtLockedFile(fn);
}

void Utils::MachineLockBase::lock(bool for_read, bool block)
{
    try
    {
        _grab_lock_in_process(for_read, block);

        // Then open the file in preparation for locking
        if(!_usermachinelockfile->open(QFile::ReadWrite))
        {
            Core::Exception ex("Couldn't open lockfile!");
            ex.SetData("err", _usermachinelockfile->errorString().toStdString());

            _release_lock();

            THROW_GUTIL_EXCEPTION( ex );
        }

        // Then actually lock the file
        else if(!_usermachinelockfile->lock(
                for_read ? QtLockedFile::ReadLock : QtLockedFile::WriteLock,
                block))
        {
            _usermachinelockfile->close();
            _release_lock();

            THROW_NEW_GUTIL_EXCEPTION2( Core::LockException, "Already locked by another process" );
        }
    }
    catch(Core::LockException &le)
    {
        le.SetData("Filename", FileNameForMachineLock().toStdString());
        throw;
    }
}

void Utils::MachineLockBase::UnlockForMachine()
{
    if(!IsLockedOnMachine())
        return;

    _usermachinelockfile->unlock();
    _usermachinelockfile->close();

    _release_lock();
}

QString Utils::MachineLockBase::FileNameForMachineLock() const
{
    return QFileInfo(*_usermachinelockfile).absoluteFilePath();
}

void Utils::MachineLockBase::_grab_lock_in_process(bool for_read, bool block)
{
    if(FileNameForMachineLock().isEmpty())
        THROW_NEW_GUTIL_EXCEPTION2( Core::Exception, "The machine-lock file has not been set.  You must "
                              "provide an identifier and optional modifier to use this function" );

    if(IsLockedOnMachine())
        THROW_NEW_GUTIL_EXCEPTION2( Core::LockException, "I already own the lock!" );


    QReadWriteLock &l = _get_lock_reference();
    if(block)
    {
        if(for_read)
            l.lockForRead();
        else
            l.lockForWrite();

        SetLockOwner(true);
    }
    else
    {
        if(for_read)
            SetLockOwner(l.tryLockForRead());
        else
            SetLockOwner(l.tryLockForWrite());
    }

    if(GetLockOwner())
        SetReadLockOwner(for_read);
    else
        THROW_NEW_GUTIL_EXCEPTION2( Core::LockException, "Lock held by someone else in this process!" );
}

void Utils::MachineLockBase::_release_lock()
{
    _get_lock_reference().unlock();
    SetLockOwner(false);
}

QReadWriteLock &Utils::MachineLockBase::_get_lock_reference()
{
    process_locks_lock.lockForRead();

    QString s = FileNameForMachineLock();
    if(!process_locks.contains(s))
        process_locks.insert(s, new QReadWriteLock());

    QReadWriteLock *ret = process_locks.value(s);

    process_locks_lock.unlock();
    return *ret;
}



#define UM_RWLOCK_IDENTIFIER "MACHINE_RW_LOCK"

Utils::UserMachineReadWriteLock::UserMachineReadWriteLock(const QString &identifier, const QString &modifier)
    :MachineLockBase(UM_RWLOCK_IDENTIFIER, identifier, modifier)
{
}

Utils::UserMachineReadWriteLock::UserMachineReadWriteLock(const QString &file_name)
    :MachineLockBase(UM_RWLOCK_IDENTIFIER, file_name)
{
}

void Utils::UserMachineReadWriteLock::LockForReadOnMachine(bool block)
        throw(Core::LockException, Core::Exception)
{
    // Lock the local mutex
    lock(true, block);
}

void Utils::UserMachineReadWriteLock::LockForWriteOnMachine(bool block)
        throw(GUtil::Core::LockException,
              GUtil::Core::Exception)
{
    lock(false, block);
}




#define UM_LOCK_IDENTIFIER "MACHINE_LOCK"

Utils::UserMachineMutex::UserMachineMutex(const QString &identifier, const QString &modifier)
    :MachineLockBase(UM_LOCK_IDENTIFIER, identifier, modifier)
{
}

Utils::UserMachineMutex::UserMachineMutex(const QString &file_name)
    :MachineLockBase(UM_LOCK_IDENTIFIER, file_name)
{
}

void Utils::UserMachineMutex::LockMutexOnMachine(bool block)
        throw(GUtil::Core::LockException,
              GUtil::Core::Exception)
{
    lock(false, block);
}
