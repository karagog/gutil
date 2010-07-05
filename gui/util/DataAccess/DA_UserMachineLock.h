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

class QtLockedFile;

namespace GUtil
{
    namespace QtUtil
    {
        namespace DataAccess
        {
            class DA_UserMachineLock
            {
            public:
                DA_UserMachineLock(const QString &);
                ~DA_UserMachineLock();

                bool Lock();
                void Unlock();

                QString Error() const;
                QString FileName() const;

            private:
                QtLockedFile *_lf;

                bool _locked;
                QString _error;
            };
        }
    }
}

#endif // APPLICATIONLOCK_H
