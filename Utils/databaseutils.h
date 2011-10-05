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

#ifndef DATABASEUTILS_H
#define DATABASEUTILS_H

#include "gutil.h"
#include <QString>
#include <QDateTime>

class QSqlQuery;
class QSqlDatabase;

GUTIL_BEGIN_NAMESPACE(Utils);


class DatabaseUtils
{
public:

    static void ThrowQueryException(const QSqlQuery &);

    // This will execute a sql script one line at a time,
    //   where each line is terminated by a semi-colon
    static void ExecuteScript(QSqlDatabase &, const QString &script_sql);


    // For string manipulation
    enum DateResolution
    {
        DR_Minutes,
        DR_Seconds
    };

    static QString ConvertDateToString(const QDateTime &, DateResolution);
    static QDateTime ConvertStringToDate(const QString &);

};


GUTIL_END_NAMESPACE;

#endif // DATABASEUTILS_H
