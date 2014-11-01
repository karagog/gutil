/*Copyright 2010-2015 George Karagoulis

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

#include <gutil/ilog.h>
#include <QString>

class QSqlDatabase;

NAMESPACE_GUTIL1(Qt);


/** Logs stuff to a database */
class DatabaseLogger : public GUtil::ILog
{
    const QString m_connStr;
    const QString m_tableName;
public:

    /** Creates a database logger with the given database connection string.
        \param conn_str The identifier of the database in Qt's database list.
        \param table_name The name of the table in the database.  By default we log to
        the 'LOG' table
    */
    explicit DatabaseLogger(const char *conn_str, const char *table_name = "LOG");
    virtual ~DatabaseLogger() {}

    /** The connection string from a database registered with Qt's sql module. */
    const QString &GetConnectionString() const{ return m_connStr; }

    /** The table in the database which we log to.  The default is LOG */
    const QString &GetTableName() const{ return m_tableName; }

    virtual void Log(const LoggingData &) noexcept;

};


END_NAMESPACE_GUTIL1;


#endif // GUTIL_DATABASELOGGER_H
