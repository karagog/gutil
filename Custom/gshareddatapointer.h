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

#ifndef GSHAREDDATAPOINTER_H
#define GSHAREDDATAPOINTER_H

#include <QExplicitlySharedDataPointer>
#include <QReadWriteLock>

namespace GUtil
{
    namespace Custom
    {
        // Like a normal shared data pointer, but with a read-write lock attached

        template <typename T> class GSharedDataPointer :
                public QExplicitlySharedDataPointer<T>
        {
        private:

            class SharedLock : public QSharedData, public QReadWriteLock
            {
            public:

                SharedLock() : QSharedData(), QReadWriteLock(QReadWriteLock::NonRecursive){}
                SharedLock(const SharedLock &o)
                    : QSharedData(o), QReadWriteLock(QReadWriteLock::NonRecursive){}

                virtual ~SharedLock(){}
            };

            QExplicitlySharedDataPointer<SharedLock> _lock;


        public:
            GSharedDataPointer() :
                    QExplicitlySharedDataPointer<T>(){
                _lock = new SharedLock;
            }

            inline GSharedDataPointer(T *sharedData) :
                    QExplicitlySharedDataPointer<T>(sharedData){
                _lock = new SharedLock;
            }

            inline GSharedDataPointer(const GSharedDataPointer<T> &o) :
                    QExplicitlySharedDataPointer<T>(o){
                _lock = o._lock;
            }

            template <typename X> inline GSharedDataPointer(const GSharedDataPointer<X> &o) :
                    QExplicitlySharedDataPointer<T>(o){
                _lock = o._lock;
            }

            GSharedDataPointer &operator =(const GSharedDataPointer &o){
                ((QExplicitlySharedDataPointer<T> &)(*this)) = o;
                _lock = o._lock;
                return *this;
            }

            // Use these methods to lock the shared data.  You must be careful
            //   to use locks carefully; the class provides a convenient lock that
            //   any objects sharing the data can use, but you have to verify your
            //   own locking mechanism.
            inline void Lock();
            inline void LockForRead();
            inline void LockForWrite();

            inline bool TryLock();
            inline bool TryLockForRead(int timeout = -1);
            inline bool TryLockForWrite(int timeout = -1);

            inline void Unlock(){ _lock->unlock(); }

            inline void detach(){
                QExplicitlySharedDataPointer<T>::detach();
                _lock.detach();
            }

            inline void reset(){
                QExplicitlySharedDataPointer<T>::reset();
                _lock.reset();
            }
        };
    }
}

#include "gshareddatapointer.cpp"

#endif // GSHAREDDATAPOINTER_H
