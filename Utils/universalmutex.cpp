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

#include "universalmutex.h"
#include <QFile>
using namespace GUtil;
using namespace Utils;

UniversalMutex::UniversalMutex(const QString &file_path)
    :_machine_mutex(file_path),
    _is_locked(false)
{
    SetFilePath(file_path);

    connect(&_fsw, SIGNAL(fileChanged(QString)), this, SLOT(lock_file_updated()));
}

void UniversalMutex::lock_file_updated()
{
    _condition_file_updated.wakeAll();
}

bool UniversalMutex::Lock(bool block)
{
    bool ret = false;
    _fail_if_locked();

    while(block)
    {
        try
        {
            _machine_mutex.LockMutexOnMachine(block);
        }
        catch(Core::LockException &)
        {
            // The only time we hit an exception is if the call to 'LockMutexOnMachine' does not block
            break;
        }

        // After we have the machine lock, call our own locking function
        _lock(block);

        // Then we verify the file after the fact, to make sure our ID is still in the file
        if(HasLock(false))
        {
            _is_locked = true;
            ret = true;
            break;
        }
        else
            _machine_mutex.UnlockForMachine();
    }

    return ret;
}

void UniversalMutex::Unlock()
{
    _unlock();
    _machine_mutex.UnlockForMachine();
    _is_locked = false;
}

void UniversalMutex::SetFilePath(const QString &fp)
{
    _fail_if_locked();

    _machine_mutex.SetUserMachineLockFileName(fp);

    // Create the file if it doesn't exist
    if(!QFile::exists(file_path))
    {
        QFile f(file_path);
        f.open(QFile::WriteOnly);
        f.close();
    }


    if(_lock_file_set())
        _fsw.removePath(GetFilepath());

    _fsw.addPath(file_path);
}

QString UniversalMutex::GetFilepath() const
{
    return _machine_mutex.FileNameForMachineLock();
}



#define UNIVERSAL_MUTEX_MODIFIER "UNIVERSAL_LOCK"

void UniversalMutex::_lock(bool block)
{
    QFile f(_lock_file_path = QString("%1." + UNIVERSAL_MUTEX_MODIFIER)
            .arg(GetFilepath()));

    f.open(QFile::ReadWrite);
    {
        f.write((_id = QUuid::createUuid()).toString());
    }
    f.close();
}

void UniversalMutex::_unlock()
{
    QFile f(_lock_file_path);

    f.open(QFile::ReadWrite);
    {
        f.resize(0);
    }
    f.close();
}

bool UniversalMutex::HasLock(bool from_cache) const
{
    if(from_cache)
        return _is_locked;

    QFile f(_lock_file_path);
    QUuid tmp_id;

    f.open(QFile::ReadOnly);
    {
        tmp_id = f.readAll();
    }
    f.close();

    return tmp_id == _id;
}

void UniversalMutex::CleanupLockFile() const
{
    _fail_if_locked();

    QFile::remove(GetFilepath());
}

void UniversalMutex::_fail_if_locked() const
{
    if(!_lock_file_set())
        THROW_NEW_GUTIL_EXCEPTION( Core::LockException,
                                   "No file path set!" );

    if(_is_locked)
        THROW_NEW_GUTIL_EXCEPTION( Core::LockException,
                                   "Cannot complete operation while mutex is locked" );
}
