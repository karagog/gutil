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

#ifndef GLOBALLOGGER_H
#define GLOBALLOGGER_H

#include "Core/exception.h"
#include <QString>
#include <QMap>

class QReadWriteLock;

namespace GUtil
{
    namespace Logging
    {
        class AbstractLogger;

        class GlobalLogger
        {
        public:

            // You can pass one of these as a logger Id to reference
            enum LoggerIdEnum
            {
                DebugId = -3,   // A special id for the debug logger
                DefaultId = -2,
                NewId = -1
            };

            // You are responsible for managing your own logger memory
            static int SetupLogger(AbstractLogger *, int logger_id = NewId);
            static int SetupDefaultLogger(AbstractLogger *);

            static void TakeDownLogger(int logger_id = DefaultId);
            static void ClearLog(int logger_id = DefaultId);

            static void SetDefaultLoggerID(int);
            static int GetDefaultLoggerID();


            // Global Logging Methods:
            static void LogMessage(
                    const QString &msg,
                    const QString &title = QString::null,
                    int logger_id = DefaultId);

            static void LogWarning(
                    const QString &msg,
                    const QString &title = QString::null,
                    int logger_id = DefaultId);

            static void LogError(
                    const QString &msg,
                    const QString &title = QString::null,
                    int logger_id = DefaultId);

            static void LogException(const GUtil::Core::Exception &, int logger_id = DefaultId);
            static void LogException(const std::exception &, int logger_id = DefaultId);


        protected:

            static void Log(const QString &,
                            const QString &,
                            int logger_id,
                            int message_level);


        private:

            static QMap<int, Logging::AbstractLogger *> _logger_list;
            static QReadWriteLock _logger_list_lock;

            static int _setup_logger(GUtil::Logging::AbstractLogger *, int logger_id);
            static void _takedown_logger(int logger_id);

            static void _translate_logger_id(int &, bool allow_new_id)
                    throw(GUtil::Core::ArgumentException);

            static int _default_logger_id;

        };
    }
}

#endif // GLOBALLOGGER_H
