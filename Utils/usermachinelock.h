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

#include "Core/exception.h"
#include <QString>

class QtLockedFile;
class QMutex;

namespace GUtil
{
    namespace Utils
    {
        // An easy to use inter-process locking mechanism.

        // Classes can implement this interface to achieve locking across the
        //    machine for the user who invokes it
        class UserMachineLock
        {
        public:
            UserMachineLock(const QString &identifier = QString::null,
                            const QString &modifier = QString::null);
            virtual ~UserMachineLock();

            void SetUserMachineLockIdentifier(const QString &identifier = QString::null,
                                              const QString &modifier = QString::null);
            void SetUserMachineLockFileName(const QString &);

            // Use these functions to lock/unlock this object for the user's machine
            void LockForUserOnMachine(bool block = false)
                    throw(GUtil::Core::LockException,
                          GUtil::Core::Exception);
            bool TryLockForUserOnMachine();
            void UnlockForUserOnMachine();

            bool IsLockedForUserOnMachine() const;

            QString FileNameForUserMachineLock() const;

        private:
            QtLockedFile *_usermachinelockfile;

                        bool _grab_mutex(bool block);
			void _release_mutex();

			QMutex *_get_mutex_reference();

			bool _i_own_mutex;
        };
    }
}

#endif // APPLICATIONLOCK_H
