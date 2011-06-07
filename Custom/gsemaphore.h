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

#ifndef GSEMAPHORE_H
#define GSEMAPHORE_H

#include <QSemaphore>
#include <QReadWriteLock>

namespace GUtil
{
    namespace Custom
    {
        // A lockable semaphore
        class GSemaphore :
                public QSemaphore,
                public QReadWriteLock
        {
        public:

            inline GSemaphore(int init_val = 0)
                :QSemaphore(init_val)
            {}

            inline void Up(int n = 1){
                release(n);
            }

            inline void Down(int n = 1){
                acquire(n);
            }

            inline bool IsEmpty(){
                return available() == 0;
            }

        };
    }
}

#endif // GSEMAPHORE_H
