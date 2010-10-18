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

namespace GUtil
{
    namespace Core
    {
        class Exception;
    }

    namespace BusinessObjects
    {
        class AbstractLogger;

        class GlobalLogger : public QObject
        {
            Q_OBJECT
        public:
            // You shouldn't ever have to instantiate one of these yourself
            explicit GlobalLogger(QObject *parent = 0);

            // You can use this instance to connect to the static slots
            static GlobalLogger *Instance();

            static int SetupLogger(AbstractLogger *);

            static int SetupFileLogger(const QString &filename);
            static int SetupConsoleLogger();

            static void TakedownLogger(int logger_id = -1);

            static void ClearLog(int logger_id = -1);

            static void SetDefaultLogger(int);
            static int GetDefaultLogger();

        public slots:
            static void LogMessage(const QString &msg = QString::null,
                                   const QString &title = QString::null,
                                   int logger_id = -1);
            static void LogWarning(const QString &msg = QString::null,
                                   const QString &title = QString::null,
                                   int logger_id = -1);
            static void LogError(const QString &msg = QString::null,
                                 const QString &title = QString::null,
                                 int logger_id = -1);

            static void LogException(const GUtil::Core::Exception &, int logger_id = -1);
            static void LogException(const std::exception &, int logger_id = -1);

            static void Log(const QString &, const QString &, int logger_id, int message_level);

        };
    }
}

#endif // GLOBALLOGGER_H
