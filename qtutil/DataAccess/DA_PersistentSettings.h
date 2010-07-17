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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QMap>
#include <QString>
#include "DA_UserMachineLock.h"

namespace GQtUtil
{
    namespace DataAccess
    {
        // This encapsulates persistent settings for your application
        class DA_PersistentSettings : public DA_UserMachineLock
        {
            Q_OBJECT
        public:

            DA_PersistentSettings(const QString &identity, QObject *parent = 0);
            ~DA_PersistentSettings();

        private:
            bool lock();
            void unlock();
        };
    }
}

#endif // SETTINGS_H
