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

#include "gsharedlock.h"
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
        public:
            GSharedDataPointer() :
                    QExplicitlySharedDataPointer<T>(){}

            inline GSharedDataPointer(T *sharedData) :
                    QExplicitlySharedDataPointer<T>(sharedData){}

            inline GSharedDataPointer(const GSharedDataPointer<T> &o) :
                    QExplicitlySharedDataPointer<T>(o){
                *this = o;
            }

            template <typename X> inline GSharedDataPointer(const GSharedDataPointer<X> &o) :
                    QExplicitlySharedDataPointer<T>(o){
                *this = o;
            }

            GSharedDataPointer &operator =(const GSharedDataPointer &o){
                ((QExplicitlySharedDataPointer<T> &)(*this)) = o;
                _lock = o._lock;
                return *this;
            }

            // Use this lock to lock the shared data.  You must be careful
            //   to use locks carefully; the class provides a convenient lock that
            //   any objects sharing the data can use, but you have to verify your
            //   own locking mechanism.
            GSharedLock &SharedLock(){ return _lock; }

            void detach(){
                QExplicitlySharedDataPointer<T>::detach();
                _lock.Detach();
            }

            void reset(){
                QExplicitlySharedDataPointer<T>::reset();
                _lock.Detach();
            }

        private:
            GSharedLock _lock;
        };
    }
}

#endif // GSHAREDDATAPOINTER_H
