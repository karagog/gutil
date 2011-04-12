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

#ifndef ABSTRACTLOGGER_H
#define ABSTRACTLOGGER_H

#include <QDateTime>
#include <QString>
#include <QObject>
#include "Custom/gcoreapplication.h"

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
        class AbstractLogger :
                public QObject,

                // This allows us to be cleaned up by the GApplication
                public Custom::GCoreApplication::CleanupObject
        {
            Q_OBJECT
        public:
            enum MessageLevelEnum
            {
                Info = 0,
                Warning = 1,
                Error = 2
            };

            // Derived classes can customize the log method, but for most people you will want
            virtual void Log(const QString &message, const QString &title, MessageLevelEnum);

            void LogException(const std::exception &ex);

            // Clears the log file
            virtual void ClearLog();

            // Determines the level under which we won't log anything
            void SetMessageLevel(MessageLevelEnum);
            MessageLevelEnum MessageLevel();

            virtual ~AbstractLogger();


        public slots:

            // These are the logging functions
            void LogMessage(const QString &message, const QString &title = QString::null);
            void LogWarning(const QString &message, const QString &title = QString::null);
            void LogError(const QString &message, const QString &title = QString::null);

            void LogException(const GUtil::Core::Exception &ex);


        protected:
            explicit AbstractLogger(QObject *parent = 0);

            // Derived classes must provide a method to put the log message somewhere
            virtual void Log_protected(const QByteArray &, MessageLevelEnum) = 0;

            // You can customize your own logging format here
            virtual QString prepare_log_message(const QString &, const QString &, MessageLevelEnum,
                                              const QDateTime &dt = QDateTime::currentDateTime());


        private:
            MessageLevelEnum _message_level;

        };
    }
}

#endif // ABSTRACTLOGGER_H
