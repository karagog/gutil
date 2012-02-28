/*Copyright 2010-2012 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifdef GUI_FUNCTIONALITY

#include "gutil_universalmutex.h"
#include "gutil_extendedexception.h"
#include <QFile>
NAMESPACE_GUTIL2(QT, Utils);


#define UNIVERSAL_MUTEX_MODIFIER "UNIVERSAL_LOCK"

UniversalMutex::UniversalMutex(const QString &file_path,
                               const QUuid &id,
                               const QStringList &ids,
                               QObject *parent)
    :QThread(parent),
      _machine_mutex(file_path),
      m_id(id),
      m_Ids(ids),
      _is_locked(false)
{
    if(!m_id.isNull())
        m_Ids.append(m_id);

    if(!file_path.isNull())
        SetFilePath(file_path);

    connect(&_fsw, SIGNAL(fileChanged(QString)), this, SLOT(lock_file_updated()));
}

UniversalMutex::~UniversalMutex()
{
    Unlock();

    // Kill the thread
    if(isRunning())
        terminate();
    wait();
}

void UniversalMutex::lock_file_updated()
{
    // If we have the lock, start the background thread to check if we've just lost it
    if(HasLock())
    {
        if(!isRunning())
            start();
    }
}

void UniversalMutex::Lock()
{
    _lockfile_lock.lockForWrite();
    try
    {
        _fail_if_locked();

        // This might throw a LockException if you don't block and it fails
        _machine_mutex.LockMutexOnMachine(false);

        // After we have the machine lock, call our own locking function
        try
        {
            _lock();
        }
        catch(Exception<> &)
        {
            // Unlock the machine lock before propagating the exception
            _machine_mutex.UnlockForMachine();
            throw;
        }

        // Then we verify the file after the fact, to make sure our ID is still in the file
        if(_has_lock(false))
            _is_locked = true;
    }
    catch(Exception<> &)
    {
        // Unlock ourselves before propagating the exception
        _lockfile_lock.unlock();
        throw;
    }

    GASSERT(_is_locked);

    _lockfile_lock.unlock();
}

void UniversalMutex::Unlock()
{
    _lockfile_lock.lockForWrite();
    {
        _unlock();
    }
    _lockfile_lock.unlock();
}

void UniversalMutex::SetFilePath(const QString &fp)
{
    _lockfile_lock.lockForWrite();
    try
    {
        _fail_if_locked();

        // Remove the old file path from the watch list
        if(_lock_file_set())
            _fsw.removePath(_lock_file_path);


        // Set the path on our machine mutex, and our lock file path
        _machine_mutex.SetUserMachineLockFileName(fp);
        _lock_file_path = QString("%1." UNIVERSAL_MUTEX_MODIFIER).arg(fp);


        // Create our watch file if it doesn't exist
        if(_lock_file_set())
        {
            if(!QFile::exists(fp))
            {
                QFile f(_lock_file_path);
                f.open(QFile::WriteOnly);
                f.close();
            }

            // Add the new path to the watch list
            _fsw.addPath(_lock_file_path);
        }
    }
    catch(Exception<> &)
    {
        _lockfile_lock.unlock();
        throw;
    }
    _lockfile_lock.unlock();
}

QString UniversalMutex::GetFilepath() const
{
    return _machine_mutex.FileNameForMachineLock();
}

void UniversalMutex::_lock()
{
    if(_lock_file_path.isNull())
        THROW_NEW_GUTIL_EXCEPTION2(Exception,
                                   "You must first set the lockfile path");
    QFile f(_lock_file_path);

    f.open(QFile::ReadWrite);
    {
        bool unrecognized_guid(false);

        // First determine if someone else has the lock
        if(f.size() > 0)
        {
            QUuid tmp(f.readAll().constData());
            if(!tmp.isNull())
                unrecognized_guid = !m_Ids.contains(tmp);
        }

        if(unrecognized_guid)
        {
            f.close();
            THROW_NEW_GUTIL_EXCEPTION2( LockException,
                                       QString("Someone else already owns the universal lock: %1")
                                       .arg(GetFilepath()).toAscii().constData());
        }

        f.resize(0);
        f.write(m_id.toString().toAscii());
    }
    f.close();
}

void UniversalMutex::_unlock()
{
    if(!_has_lock(true))
        return;

    QFile f(_lock_file_path);

    f.open(QFile::ReadWrite);
    {
        f.resize(0);
    }
    f.close();
    f.remove();

    _machine_mutex.UnlockForMachine();
    _is_locked = false;
}

bool UniversalMutex::HasLock()
{
    bool ret;
    _lockfile_lock.lockForRead();
    {
        ret = _has_lock(true);
    }
    _lockfile_lock.unlock();
    return ret;
}

bool UniversalMutex::_has_lock(bool from_cache) const
{
    if(from_cache)
        return _is_locked;

    QUuid tmp_id;
    QFile f(_lock_file_path);

    f.open(QFile::ReadOnly);
    tmp_id = f.readAll().constData();
    f.close();

    return tmp_id == m_id;
}

void UniversalMutex::_fail_if_locked() const
{
//    if(!_lock_file_set())
//        THROW_NEW_GUTIL_EXCEPTION2( LockException,
//                                   "No file path set!" );

    if(_is_locked)
        THROW_NEW_GUTIL_EXCEPTION2( LockException,
                                   "Cannot complete operation while mutex is locked" );
}

void UniversalMutex::run()
{
    _lockfile_lock.lockForWrite();
    {
        // Read in the file and check if we still have the lock

        // (if our cache says we're locked, but reading the file says we're not, then we've
        //   lost the lock somehow)
        if(_has_lock(true) && !_has_lock(false))
        {
            _is_locked = false;

            _machine_mutex.UnlockForMachine();
            emit NotifyLostLock();
        }
    }
    _lockfile_lock.unlock();
}


END_NAMESPACE_GUTIL2;

#endif // GUI_FUNCTIONALITY
