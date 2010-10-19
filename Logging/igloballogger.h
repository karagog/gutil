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

#ifndef IGLOBALLOGGER_H
#define IGLOBALLOGGER_H

#include <QString>

namespace GUtil
{
    namespace Core
    {
        class Exception;
    }

    namespace Logging
    {
        class AbstractLogger;

        // A class to allow any class to log to a global location
        class IGlobalLogger
        {
        public:

            // You should never have to instantiate a logger yourself; simply derive from it
            IGlobalLogger();
            virtual ~IGlobalLogger();

            // Returns the logger id by which you can reference the inserted logger
            int SetupLogger(GUtil::Logging::AbstractLogger *);

            // These convenience functions set up a file/console logger to the default logger index
            int SetupFileLogger(const QString &filename);
            int SetupConsoleLogger();

            void TakeDownLogger(int logger_id = -1);

            void SetDefaultLogger(int);
            int GetDefaultLogger();

            void ClearLog(int logger_id = -1);

            void LogMessage(const QString &msg = QString::null,
                                   const QString &title = QString::null,
                                   int logger_id = -1);
            void LogWarning(const QString &msg = QString::null,
                                   const QString &title = QString::null,
                                   int logger_id = -1);
            void LogError(const QString &msg = QString::null,
                                 const QString &title = QString::null,
                                 int logger_id = -1);

            void LogException(const GUtil::Core::Exception &, int logger_id = -1);
            void LogException(const std::exception &, int logger_id = -1);

            // The message type enum is declared in abstractlogger
            void Log(const QString &, const QString &, int logger_id, int message_type_enum);

        private:
            static void _takedown_logger(int);
            static int _setup_logger(GUtil::Logging::AbstractLogger *, int logger_id);

            void _translate_logger_id(int &);

            int _default_logger_id;
        };
    }
}

#endif // IGLOBALLOGGER_H
