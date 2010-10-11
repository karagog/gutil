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

#ifndef APPLICATIONLOCK_H
#define APPLICATIONLOCK_H
#include <QString>
#include "DA_ConfigFile.h"
#include "Core/exception.h"

class QtLockedFile;

namespace GUtil
{
    namespace DataAccess
    {
        // An easy to use inter-process locking mechanism, which also has a backend
        //   datastore for setting and accessing persistent data
        class DA_UserMachineLock : public DA_ConfigFile
        {
        public:
            DA_UserMachineLock(const QString &,
                               const QString &modifier = "",
                               QObject *parent = 0);
            ~DA_UserMachineLock();

            // Using this class is as simple as these two functions
            void lock();
            void unlock();

            bool isLocked() const;

            QString fileName() const;

        private:
            QtLockedFile *_lf_lock;
        };
    }
}

#endif // APPLICATIONLOCK_H
