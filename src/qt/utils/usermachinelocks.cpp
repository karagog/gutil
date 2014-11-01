/*Copyright 2010-2015 George Karagoulis

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

#include <gutil/usermachinelocks.h>
#include <gutil/exception.h>
#include <QFileInfo>
#include <QStandardPaths>
#include <QLockFile>

NAMESPACE_GUTIL1(Qt);

QMap<QString, QReadWriteLock *> MachineLockBase::process_locks;
QReadWriteLock MachineLockBase::process_locks_lock;
QSemaphore MachineLockBase::process_locks_sem;

MachineLockBase::MachineLockBase(const QString &u, const QString &id, const QString &modifier)
    :_p_StringModifier(u),
    _p_LockOwner(false),
    _p_ReadLockOwner(false),
    _usermachinelockfile(0)
{
    SetUserMachineLockIdentifier(id, modifier);
    _post_init();
}

MachineLockBase::MachineLockBase(const QString &u, const QString &file_name)
    :_p_StringModifier(u),
    _p_LockOwner(false),
    _p_ReadLockOwner(false),
    _usermachinelockfile(0)
{
    SetUserMachineLockFileName(file_name);
    _post_init();
}

void MachineLockBase::_post_init()
{
    process_locks_lock.lockForWrite();
    process_locks_sem.release();    // Up the semaphore
    process_locks_lock.unlock();
}

MachineLockBase::~MachineLockBase()
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

void MachineLockBase::SetUserMachineLockIdentifier(
        const QString &identifier,
        const QString &modifier)
{
    if(identifier.length() == 0)
        return;

    SetUserMachineLockFileName(QString("%1/%2%3.%4")
                               .arg(QStandardPaths::writableLocation(QStandardPaths::DataLocation))
                               .arg(identifier)
                               .arg(modifier.length() != 0 ?
                                    QString(".%1").arg(modifier) :
                                    QString::null)
                               .arg(GetStringModifier()));
}

void MachineLockBase::SetUserMachineLockFileName(const QString &fn)
{
    if(fn == FileNameForMachineLock())
        return;

    UnlockForMachine();

    delete _usermachinelockfile;
    _usermachinelockfile = new QLockFile(fn);
    m_filename = fn;
}

void MachineLockBase::lock(bool for_read, bool block)
{
    try
    {
        _grab_lock_in_process(for_read, block);

        if((block && !_usermachinelockfile->lock()) ||
           (!block && !_usermachinelockfile->tryLock()))
        {
            _release_lock();

            throw LockException<>("Already locked by another process");
        }
    }
    catch(LockException<true> &le)
    {
        le.Data["Filename"] = FileNameForMachineLock().toUtf8().constData();
        throw;
    }
}

void MachineLockBase::UnlockForMachine()
{
    if(!IsLockedOnMachine())
        return;

    _usermachinelockfile->unlock();

    _release_lock();
}

QString MachineLockBase::FileNameForMachineLock() const
{
    return QFileInfo(m_filename).absoluteFilePath();
}

void MachineLockBase::_grab_lock_in_process(bool for_read, bool block)
{
    if(FileNameForMachineLock().isEmpty())
        throw Exception<>("The machine-lock file has not been set.  You must"
                          " provide an identifier and optional modifier to use this function" );

    if(IsLockedOnMachine())
        throw LockException<>("I already own the lock!");


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
        throw LockException<>("Lock held by someone else in this process!");
}


END_NAMESPACE_GUTIL1;

#endif // GUI_FUNCTIONALITY
