/*Copyright 2014 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_LOCKEDPOINTER_H
#define GUTIL_LOCKEDPOINTER_H

#include "gutil_macros.h"
#include <mutex>
#include <memory>

NAMESPACE_GUTIL;


/** Implements a thread-guard for a pointer, so if you
 *  construct one of these around a pointer you will block
 *  until the lock is available and own it until this object
 *  destructs. Therefore you want to only use these sparingly,
 *  don't do any long operations while one is alive and destroy
 *  them immediately when you're done.
 *
 *  The use-case behind developing this is for global resources
 *  which may be accessed from multiple threads. If you use this class
 *  you don't need to make your resource thread-safe, because if everyone
 *  accesses it through the locked pointer it will be safe by default.
*/
template<class T>
class LockedPointer
{
public:

    /** Constructs a locked pointer around p, acquiring the lock until this
     *  object is destroyed.
    */
    LockedPointer(T *p, std::mutex &lock)
        :m_data(new data_t(p, lock)) {}

    T *Data() const{ return m_data->pointer; }
    T *operator -> () const{ return Data(); }
    T &operator * () const{ return *Data(); }


private:
    struct data_t{
        T *pointer;
        std::mutex &lock;

        data_t(T *p, std::mutex &l)
            :pointer(p), lock(l) {
            lock.lock();
        }
        ~data_t(){
            lock.unlock();
        }
    };
    std::shared_ptr<data_t> m_data;
};


}

#endif // GUTIL_LOCKEDPOINTER_H
