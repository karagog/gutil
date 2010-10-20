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

#include "usermachinemutex.h"
#include "ThirdParty/QtLockedFile/qtlockedfile.h"
#include <QFileInfo>
#include <QDesktopServices>
using namespace GUtil;

QMap<QString, QMutex *> Utils::UserMachineMutex::process_locks;
QMutex Utils::UserMachineMutex::process_locks_lock;

Utils::UserMachineMutex::UserMachineMutex(const QString &id, const QString &modifier)
{
    _i_own_mutex = false;

    _usermachinelockfile = new QtLockedFile();

    SetUserMachineLockIdentifier(id, modifier);
}

Utils::UserMachineMutex::~UserMachineMutex()
{
    UnlockForUserOnMachine();

    delete _usermachinelockfile;
}

void Utils::UserMachineMutex::SetUserMachineLockIdentifier(
        const QString &identifier,
        const QString &modifier)
{
    if(identifier.length() == 0)
        return;

    SetUserMachineLockFileName(QString("%1/%2%3.MACHINE_LOCK")
                               .arg(QDesktopServices::storageLocation(QDesktopServices::DataLocation))
                               .arg(identifier)
                               .arg(modifier.length() != 0 ?
                                    QString(".%1").arg(modifier) :
                                    QString::null));
}

void Utils::UserMachineMutex::SetUserMachineLockFileName(const QString &fn)
{
    UnlockForUserOnMachine();

    _usermachinelockfile->setFileName(fn);
}

void Utils::UserMachineMutex::LockForUserOnMachine(bool block)
        throw(Core::LockException, Core::Exception)
{
    // Lock the local mutex
    try
    {
        _grab_mutex(block);

        // Then open the file in preparation for locking
        if(!_usermachinelockfile->open(QFile::ReadWrite))
        {
            Core::Exception ex("Couldn't open lockfile!");
            ex.SetData("Filename", FileNameForUserMachineLock().toStdString());
            ex.SetData("Error", _usermachinelockfile->errorString().toStdString());

            _release_mutex();
            throw ex;
        }

        // Then actually lock the file
        else if(!_usermachinelockfile->lock(QtLockedFile::WriteLock, block))
        {
            _usermachinelockfile->close();
            _release_mutex();

            throw Core::LockException("Already locked by another process");
        }
    }
    catch(Core::LockException &le)
    {
        le.SetData("Filename", FileNameForUserMachineLock().toStdString());
        throw;
    }
}

bool Utils::UserMachineMutex::TryLockForUserOnMachine()
{
    try
    {
        LockForUserOnMachine(false);
    }
    catch(Core::Exception &)
    {
        return false;
    }

    return true;
}

void Utils::UserMachineMutex::UnlockForUserOnMachine()
{
    if(!IsLockedForUserOnMachine())
        return;

    _usermachinelockfile->unlock();
    _usermachinelockfile->close();

    _release_mutex();
}

bool Utils::UserMachineMutex::IsLockedForUserOnMachine() const
{
    return _i_own_mutex;
}

QString Utils::UserMachineMutex::FileNameForUserMachineLock() const
{
    return QFileInfo(*_usermachinelockfile).absoluteFilePath();
}

void Utils::UserMachineMutex::_grab_mutex(bool block)
{
    if(FileNameForUserMachineLock() == QString::null)
        throw Core::Exception("The machine-lock file has not been set.  You must "
                              "provide an identifier and optional modifier to use this function");

    if(IsLockedForUserOnMachine())
        throw Core::LockException("I already own the lock!");


    if(block)
    {
        _get_mutex_reference()->lock();
        _i_own_mutex = true;
    }
    else
        _i_own_mutex = _get_mutex_reference()->tryLock();

    if(!_i_own_mutex)
        throw Core::LockException("Lock held by someone else in this process!");
}

void Utils::UserMachineMutex::_release_mutex()
{
    _get_mutex_reference()->unlock();
    _i_own_mutex = false;
}

QMutex *Utils::UserMachineMutex::_get_mutex_reference()
{
    process_locks_lock.lock();

    QString s = FileNameForUserMachineLock();
    if(!process_locks.contains(s))
        process_locks.insert(s, new QMutex());

    QMutex *ret = process_locks.value(s);

    process_locks_lock.unlock();
    return ret;
}
