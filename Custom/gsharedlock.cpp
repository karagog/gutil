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

#include "gsharedlock.h"
using namespace GUtil;

Custom::GSharedLock::GSharedLock()
    : _lock(new LockData)
{
    _init();
}

Custom::GSharedLock::GSharedLock(const Custom::GSharedLock &o)
{
    _init();
    *this = o;
}

void Custom::GSharedLock::_init()
{
    _clear_flags();
}

void Custom::GSharedLock::Detach()
{
    _lock.detach();

    _clear_flags();
}

void Custom::GSharedLock::_clear_flags()
{
    _i_own_readlock = false;
    _i_own_readlock = false;
}

Custom::GSharedLock &Custom::GSharedLock::operator =(const Custom::GSharedLock &o)
{
    _lock = o._lock;
    _clear_flags();
    return *this;
}


void Custom::GSharedLock::Lock()
{
    LockForWrite();
}

void Custom::GSharedLock::LockForRead()
{
    _lock->lockForRead();
    _i_own_readlock = true;
}

void Custom::GSharedLock::LockForWrite()
{
    _lock->lockForWrite();
    _i_own_writelock = true;
}

bool Custom::GSharedLock::TryLock(int timeout)
{
    return TryLockForWrite(timeout);
}

bool Custom::GSharedLock::TryLockForRead(int timeout)
{
    bool ret;

    if(timeout == -1)
        ret = _lock->tryLockForRead();
    else
        ret = _lock->tryLockForRead(timeout);

    if(ret)
        _i_own_readlock = true;

    return ret;
}

bool Custom::GSharedLock::TryLockForWrite(int timeout)
{
    bool ret;

    if(timeout == -1)
        ret = _lock->tryLockForWrite();
    else
        ret = _lock->tryLockForWrite(timeout);

    if(ret)
        _i_own_writelock = true;

    return ret;
}

void Custom::GSharedLock::Unlock()
{
    _lock->unlock();

    _i_own_readlock = false;
    _i_own_writelock = false;
}

bool Custom::GSharedLock::IsLocked() const
{
    return IsLockedForWrite();
}

bool Custom::GSharedLock::IsLockedForRead() const
{
    return _i_own_readlock;
}

bool Custom::GSharedLock::IsLockedForWrite() const
{
    return _i_own_writelock;
}
