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

#ifndef LOGGER_H
#define LOGGER_H

#include <QDateTime>
#include <QString>

namespace GUtil
{
    namespace Core
    {
        class Exception;
    }

    namespace DataAccess
    {
        class GIODevice;
    }

    namespace Logging
    {
        class AbstractLogger
        {
        public:
            enum MessageLevelEnum
            {
                Info = 0,
                Warning = 1,
                Error = 2
            };


            // These are the logging functions
            void LogMessage(const QString &message, const QString &title = QString::null);
            void LogWarning(const QString &message, const QString &title = QString::null);
            void LogError(const QString &message, const QString &title = QString::null);

            void LogException(const GUtil::Core::Exception &ex);
                        void LogException(const std::exception &ex);

            virtual void Log(const QString &message, const QString &title, MessageLevelEnum);


            // Determines the level under which we won't log anything
            void SetMessageLevel(MessageLevelEnum);
            MessageLevelEnum GetMessageLevel();

            // Clears the log file
            virtual void ClearLog();

            virtual ~AbstractLogger();


        protected:
            explicit AbstractLogger();

            // This function actually writes the message somewhere useful
            virtual void Log_protected(const QByteArray &, MessageLevelEnum);

            // You can customize your own logging format
            virtual QString PrepareLogMessage(const QString &, const QString &, MessageLevelEnum,
                                              const QDateTime &dt = QDateTime::currentDateTime());

            // Derived classes must provide a transport by which to send log data.  This might be
            //   a file transport (if you want to log to a file), but it could also log to a generic
            //   stream or to a database using this abstract transport mechanism
            virtual DataAccess::GIODevice &TransportMechanism();


        private:
            MessageLevelEnum _message_level;

        };
    }
}

#endif // LOGGER_H
