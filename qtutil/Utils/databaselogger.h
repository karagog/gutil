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

#ifndef DATABASELOGGER_H
#define DATABASELOGGER_H

#include "Private/abstractlogger.h"
#include <QSqlDatabase>

namespace GQtUtil
{
    namespace Utils
    {
        class PubSubSystem;

        // Logs stuff to a database

        class DatabaseLogger : public AbstractLogger
        {
            Q_OBJECT
        public:
            explicit DatabaseLogger(PubSubSystem *pss = 0, QObject *parent = 0);

        protected:
            virtual bool PreLogMessage();
            virtual void LogMessage_protected(const QString &, MessageLevelEnum);
            virtual void PostLogMessage();

        private:
            QSqlDatabase _dbase;
        };
    }
}
#endif // DATABASELOGGER_H
