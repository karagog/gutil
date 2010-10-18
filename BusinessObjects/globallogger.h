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

#ifndef GLOBALLOGGER_H
#define GLOBALLOGGER_H

#include <QObject>
class QString;

namespace GUtil
{
    namespace Core
    {
        class Exception;
    }

    namespace BusinessObjects
    {
        class AbstractLogger;

        // A static class to manage a global set of logging objects
        class GlobalLogger : public QObject
        {
            Q_OBJECT
        public:

            // Use this for connecting to the static slots
            static GlobalLogger *Instance();

            // Returns the logger id by which you can reference the inserted logger
            static int SetupLogger(GUtil::BusinessObjects::AbstractLogger *, int logger_id = -1);
            static int SetupDefaultLogger(const QString &filename);

            static void TakeDownLogger(int);
            static void TakeDownDefaultLogger();

            static void ClearLog(int logger_id = 0);


        public slots:
            static void LogMessage(const QString &msg = QString::null,
                                   const QString &title = QString::null,
                                   int logger_id = 0);
            static void LogWarning(const QString &msg = QString::null,
                                   const QString &title = QString::null,
                                   int logger_id = 0);
            static void LogError(const QString &msg = QString::null,
                                 const QString &title = QString::null,
                                 int logger_id = 0);

            static void LogException(const GUtil::Core::Exception &, int logger_id = 0);
            static void LogException(const std::exception &, int logger_id = 0);

            // The message type enum is declared in abstractlogger
            static void Log(const QString &, const QString &, int logger_id, int message_type_enum);

        protected:
            GlobalLogger();

        private:
            static void _takedown_logger(int);
        };
    }
}

#endif // GLOBALLOGGER_H
