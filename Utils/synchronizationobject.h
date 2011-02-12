/*Copyright 2011 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef SYNCHRONIZATIONOBJECT_H
#define SYNCHRONIZATIONOBJECT_H

#include "gutil_macros.h"
#include <QReadWriteLock>
#include <QWaitCondition>

GUTIL_BEGIN_NAMESPACE(Utils);


// A locking object that releases another lock when its own lock is
//  not available.  Then automatically grabs the lock again when its
//  lock becomes available.  The lock passed in must be the lock that protects
//  this object, otherwise the implementation doesn't work.
class SynchronizationObject
{
public:
    // T is a type of lock (QMutex or QReadWriteLock)
    template <class T> inline void LockForRead(T *holding_lock){
        while(!lock.tryLockForRead())
            waitcondition.wait(holding_lock);
    }

    template <class T> inline void LockForWrite(T *holding_lock){
        while(!lock.tryLockForWrite())
            waitcondition.wait(holding_lock);
    }

    inline void Unlock(){
        lock.unlock();
        waitcondition.wakeAll();
    }

private:
    QReadWriteLock lock;
    QWaitCondition waitcondition;
};


GUTIL_END_NAMESPACE;

#endif // SYNCHRONIZATIONOBJECT_H