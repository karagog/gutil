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

#ifndef AbstractDatabaseLogger_H
#define AbstractDatabaseLogger_H

#include "abstractlogger.h"
#include <QSqlDatabase>

namespace GUtil
{
    namespace Logging
    {
        // Logs stuff to a database

        class AbstractDatabaseLogger : public AbstractLogger
        {
            Q_OBJECT
        protected:
            explicit AbstractDatabaseLogger(QObject *parent = 0);
            virtual ~AbstractDatabaseLogger();

        private:
            QSqlDatabase _dbase;
        };
    }
}
#endif // AbstractDatabaseLogger_H
