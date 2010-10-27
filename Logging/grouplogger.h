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

#ifndef LOGGERGROUP_H
#define LOGGERGROUP_H

#include "abstractlogger.h"
#include <QSet>
#include <QReadWriteLock>

namespace GUtil
{
    namespace Logging
    {
        // A logger class that logs to a group of loggers

        // This class is thread-safe

        class GroupLogger : public AbstractLogger
        {
        public:
            GroupLogger();

            virtual void Log(const QString &message, const QString &title, MessageLevelEnum msg_lvl);
            virtual void ClearLog();

            void AddLogger(AbstractLogger *);
            void RemoveLogger(AbstractLogger *, bool delete_it = true);
            void RemoveAllLoggers(bool delete_them = true);
            bool ContainsLogger(AbstractLogger *);


        private:
            QSet<AbstractLogger*> _logger_set;
            QReadWriteLock _logger_set_lock;

            void _remove_logger(AbstractLogger *, bool);


        };
    }
}

#endif // LOGGERGROUP_H
