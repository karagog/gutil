/*Copyright 2010-2012 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_DATABASEUTILS_H
#define GUTIL_DATABASEUTILS_H

#include "gutil_macros.h"
#include <QString>
#include <QDateTime>

class QSqlQuery;
class QSqlDatabase;

NAMESPACE_GUTIL2(QT, Utils);


/** A static class that defines functions which are useful for
    Qt's database implementation.
*/
class DatabaseUtils
{
public:

    /** Throws an exception with all the important query info */
    static void ThrowQueryException(const QSqlQuery &);

    /** This will execute a sql script one line at a time,
        where each line is terminated by a semi-colon
    */
    static void ExecuteScript(QSqlDatabase &, const QString &script_sql);


    /** For use in the string functions */
    enum DateResolution
    {
        DR_Minutes,
        DR_Seconds
    };

    /** Turns a QDateTime into a string that can be put into a database.
        The string has the property that, when sorted lexicographically it
        will also be sorted chronologically.
    */
    static QString ConvertDateToString(const QDateTime &, DateResolution dr = DR_Seconds);

    /** Converts a string created by ConvertDateToString() back into a
        QDateTime
    */
    static QDateTime ConvertStringToDate(const QString &);

};


END_NAMESPACE_GUTIL2;

#endif // GUTIL_DATABASEUTILS_H
