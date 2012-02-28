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

#ifndef GUTIL_DATABASELOGGER_H
#define GUTIL_DATABASELOGGER_H

#include "gutil_abstractlogger.h"
#include <QString>

class QSqlDatabase;

NAMESPACE_GUTIL2(QT, Logging);


/** Logs stuff to a database */
class DatabaseLogger :
        public GUtil::Logging::AbstractLogger
{
public:

    /** Creates a null database logger.  You must set the connection string
        prior to using.
    */
    inline DatabaseLogger() :AbstractLogger(NULL), _p_TableName("LOG"){}

    /** Creates a database logger with the given database connection string.
        \param conn_str The identifier of the database in Qt's database list.
        \param table_name The name of the table in the database.  By default we log to
        the 'LOG' table
    */
    inline explicit DatabaseLogger(const QString &conn_str, const QString &table_name = "LOG")
        :AbstractLogger(NULL),
          _p_ConnectionString(conn_str),
          _p_TableName(table_name)
    {}
    virtual ~DatabaseLogger(){}

    /** The connection string from a database registered with Qt's sql module. */
    PROPERTY(ConnectionString, QString);

    /** The table in the database which we log to.  The default is LOG */
    PROPERTY(TableName, QString);

    /** Creates the log table if it doesn't already exist.
        You should validate the table after calling this.
        \sa ValidateDatabase()
    */
    void InitializeDatabase() const;

    /** Validates the connection and the database schema to see if
        it can receive log messages.
    */
    bool ValidateDatabase() const;

    /** Overridden from AbstractLogger to log to the database. */
    virtual void Log(const GUtil::DataObjects::String &message,
                     const GUtil::DataObjects::String &title,
                     MessageLevelEnum ml);


private:

    void _open_and_validate_connection(QSqlDatabase &) const;

};


END_NAMESPACE_GUTIL2;


#endif // GUTIL_DATABASELOGGER_H
