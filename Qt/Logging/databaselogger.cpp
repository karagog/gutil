/*Copyright 2010-2013 George Karagoulis

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


#include "gutil_databaselogger.h"
#include "gutil_databaseutils.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QResource>
#include <QVariant>
USING_NAMESPACE_GUTIL;
USING_NAMESPACE_GUTIL1(DataObjects);
USING_NAMESPACE_GUTIL2(QT, Utils);

NAMESPACE_GUTIL2(QT, Logging);


void DatabaseLogger::InitializeDatabase() const
{
    QSqlDatabase db(QSqlDatabase::database(GetConnectionString()));
    _open_and_validate_connection(db);

    QString sql_resource;
    if(db.driverName() == "QSQLITE")
        sql_resource = ":/GUtil/sql/create_log_database.sqlite.sql";
    else
        THROW_NEW_GUTIL_EXCEPTION2(NotImplementedException,
                                   QString("SQL Driver '%1'").arg(db.driverName()).toUtf8());

    QResource rs(sql_resource);
    if(!rs.isValid()) THROW_NEW_GUTIL_EXCEPTION(BuildException);

    QByteArray sql((char *)rs.data(), rs.size());
    if(rs.isCompressed())
        sql = qUncompress(sql);

    sql.replace("%TABLE_NAME%", GetTableName().toUtf8());

    DatabaseUtils::ExecuteScript(db, sql);
}

void DatabaseLogger::_open_and_validate_connection(QSqlDatabase &db) const
{
    if(GetConnectionString().isEmpty())
        THROW_NEW_GUTIL_EXCEPTION2(Exception, "No connection string set");

    if(!db.isValid())
        THROW_NEW_GUTIL_EXCEPTION2(Exception,
                                   "Invalid database.  "
                                   "You must first add the database with QSqlDatabase::addDatabase()");

    if(!db.isOpen())
    {
        // Try to open the database if it's not already open
        if(!db.open())
            THROW_NEW_GUTIL_EXCEPTION2(Exception, QString("Could not open database: %1").arg(db.lastError().text()).toUtf8());
    }
}

void DatabaseLogger::Log(const String &message, const String &title, MessageLevelEnum ml, time_t)
{
    QSqlQuery q(QString("INSERT INTO %1"
                        " (Date, Severity, Title, Message)"
                        " VALUES (?, ?, ?, ?)")
                .arg(GetTableName()),
                QSqlDatabase::database(GetConnectionString()));
    q.addBindValue(DatabaseUtils::ConvertDateToString(QDateTime::currentDateTime()));
    q.addBindValue((int)ml);
    q.addBindValue(title.ToQString());
    q.addBindValue(message.ToQString());
    q.exec();
}


END_NAMESPACE_GUTIL2;

#endif // DATABASE_FUNCTIONALITY
