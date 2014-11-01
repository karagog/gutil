/*Copyright 2010-2014 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_NO_DATABASE_FUNCTIONALITY


#include <gutil/databaselogger.h>
#include <gutil/databaseutils.h>
#include <exception>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QResource>
#include <QVariant>
USING_NAMESPACE_GUTIL;
using namespace std;

NAMESPACE_GUTIL1(Qt);


static void __open_and_validate_connection(QSqlDatabase &db, const QString &table)
{
    if(db.connectionName().isEmpty())
        throw Exception<>("No connection string set");

    if(table.isEmpty())
        throw Exception<>("No table name given");

    if(!db.isValid())
        throw Exception<>("Invalid database. You must first add the database with QSqlDatabase::addDatabase()");

    if(!db.isOpen())
    {
        // Try to open the database if it's not already open
        if(!db.open())
            throw Exception<>(String::Format("Could not open log database: %s", db.lastError().text().toUtf8().constData()));
    }
}

static void __init_db(const QString &connStr, const QString &tableName)
{
    QSqlDatabase db(QSqlDatabase::database(connStr));
    __open_and_validate_connection(db, tableName);

    QString sql_resource;
    if(db.driverName() == "QSQLITE")
        sql_resource = ":/GUtil/sql/create_log_database.sqlite.sql";
    else
        throw NotImplementedException<>(QString("SQL Driver '%1'").arg(db.driverName()).toUtf8());

    QResource rs(sql_resource);
    if(!rs.isValid()) throw BuildException<>();

    QByteArray sql((char *)rs.data(), rs.size());
    if(rs.isCompressed())
        sql = qUncompress(sql);

    sql.replace("%TABLE_NAME%", tableName.toUtf8());
    DatabaseUtils::ExecuteScript(db, sql);
}

DatabaseLogger::DatabaseLogger(const char *conn_str, const char *table_name)
    :m_connStr(conn_str), m_tableName(table_name)
{
    __init_db(m_connStr, m_tableName);
}

void DatabaseLogger::Log(const LoggingData &d) noexcept
{
    QSqlQuery q(QSqlDatabase::database(GetConnectionString()));
    q.prepare(QString("INSERT INTO %1"
                      " (Date, Severity, Title, Message)"
                      " VALUES (?, ?, ?, ?)")
              .arg(GetTableName()));
    q.addBindValue(DatabaseUtils::ConvertDateToString(QDateTime::fromTime_t(d.LogTime)));
    q.addBindValue((int)d.MessageLevel);
    q.addBindValue(d.Title.ToQString());
    q.addBindValue(d.Message.ToQString());
    if(!q.exec())
        qDebug("Log to database failed...");
}


END_NAMESPACE_GUTIL1;

#endif // DATABASE_FUNCTIONALITY
