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

#ifndef GUTIL_NO_DATABASE_FUNCTIONALITY


#include "binarydatastore.h"
#include <gutil/databaseutils.h>
#include <gutil/variant.h>
#include <QFile>
#include <QVariant>
#include <QByteArray>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
USING_NAMESPACE_GUTIL;

NAMESPACE_GUTIL1(Qt);


#define BDS_TABLE_NAME "data"
#define BDS_ID_COLUMN           "id"
#define BDS_ID_COLUMN_SQL       BDS_ID_COLUMN " INTEGER PRIMARY KEY"
#define BDS_SIZE_COLUMN         "size"
#define BDS_SIZE_COLUMN_SQL     BDS_SIZE_COLUMN " INTEGER NOT NULL"
#define BDS_VERSION_COLUMN      "version"
#define BDS_VERSION_COLUMN_SQL  BDS_VERSION_COLUMN " TEXT NOT NULL"
#define BDS_DATA_COLUMN         "data"
#define BDS_DATA_COLUMN_SQL     BDS_DATA_COLUMN " BLOB"


BinaryDataStore::BinaryDataStore()
{
    throw NotImplementedException<>();
}

BinaryDataStore::BinaryDataStore(const QString &filename)
    :m_fileName(filename),
      m_connString(QUuid::createUuid().toString())
{
    bool db_existed( QFile::exists(filename) );

    try
    {
        QSqlDatabase db( QSqlDatabase::addDatabase("QSQLITE", m_connString) );
        if(!db.isValid())
            throw Exception<>("Unable to use SQLite functionality");

        db.setDatabaseName(filename);
        if(!db.open()){
            throw Exception<>(String::Format("Cannot open database: %s", db.lastError().text().toUtf8().constData()));
        }

        if(db_existed)
        {
            // Check to make sure the existing database is in the expected format
            QStringList tables( db.tables() );
            if(1 != tables.count() || tables[0] != BDS_TABLE_NAME)
                throw Exception<>("Database has unexpected format");

            QSqlQuery q(db);
            q.prepare("SELECT sql FROM sqlite_master WHERE tbl_name = '" BDS_TABLE_NAME "'");
            DatabaseUtils::ExecuteQuery(q);

            if(!q.next())
                throw Exception<>("Database has unexpected format");

            QString table_sql(q.record().value(0).toString());
            if(-1 == table_sql.indexOf(BDS_ID_COLUMN_SQL) ||
                    -1 == table_sql.indexOf(BDS_SIZE_COLUMN_SQL) ||
                    -1 == table_sql.indexOf(BDS_DATA_COLUMN_SQL) ||
                    -1 == table_sql.indexOf(BDS_VERSION_COLUMN_SQL))
                throw Exception<>("Database has unexpected format");
        }
        else
        {
            // Initialize a new database
            QSqlQuery q(db);
            q.prepare("CREATE TABLE " BDS_TABLE_NAME " ("
                      BDS_ID_COLUMN_SQL ","
                      BDS_SIZE_COLUMN_SQL ","
                      BDS_VERSION_COLUMN_SQL ","
                      BDS_DATA_COLUMN_SQL
                      ");");
            DatabaseUtils::ExecuteQuery(q);
        }
    }
    catch(...)
    {
        QSqlDatabase::removeDatabase(m_connString);
        throw;
    }
}

BinaryDataStore::~BinaryDataStore()
{
    QSqlDatabase::removeDatabase(m_connString);
}


Pair<int, QUuid> BinaryDataStore::AddData(const QByteArray &data)
{
    Pair<int, QUuid> ret;
    ret.Second = QUuid::createUuid();

    QSqlQuery q(QSqlDatabase::database(m_connString));
    q.prepare("INSERT INTO " BDS_TABLE_NAME
              " (" BDS_SIZE_COLUMN "," BDS_DATA_COLUMN "," BDS_VERSION_COLUMN ")"
              " VALUES (?,?,?)");
    q.addBindValue(data.length());
    q.addBindValue(data, QSql::Binary);
    q.addBindValue(ret.Second.toString());
    DatabaseUtils::ExecuteQuery(q);

    ret.First = q.lastInsertId().toInt();
    return ret;
}

void BinaryDataStore::RemoveData(int id)
{
    QSqlQuery q(QSqlDatabase::database(m_connString));
    q.prepare("DELETE FROM " BDS_TABLE_NAME " WHERE " BDS_ID_COLUMN "=?");
    q.addBindValue(id);
    DatabaseUtils::ExecuteQuery(q);
}

QByteArray BinaryDataStore::GetData(int id) const
{
    QByteArray ret;

    QSqlQuery q(QSqlDatabase::database(m_connString));
    q.prepare("SELECT data FROM " BDS_TABLE_NAME " WHERE " BDS_ID_COLUMN "=?");
    q.addBindValue(id);
    DatabaseUtils::ExecuteQuery(q);

    if(q.next())
        ret = q.record().value(0).toByteArray();

    return ret;
}

QUuid BinaryDataStore::SetData(int id, const QByteArray &data)
{
    QUuid new_version(QUuid::createUuid());

    QSqlQuery q(QSqlDatabase::database(m_connString));
    q.prepare("UPDATE " BDS_TABLE_NAME
              " SET " BDS_SIZE_COLUMN "=?," BDS_DATA_COLUMN "=?," BDS_VERSION_COLUMN "=?"
              " WHERE " BDS_ID_COLUMN "=?");
    q.addBindValue(data.length());
    q.addBindValue(data, QSql::Binary);
    q.addBindValue(new_version.toString());
    q.addBindValue(id);
    DatabaseUtils::ExecuteQuery(q);

    return new_version;
}

int BinaryDataStore::GetSize(int id) const
{
    int ret(-1);

    QSqlQuery q(QSqlDatabase::database(m_connString));
    q.prepare("SELECT " BDS_SIZE_COLUMN " FROM " BDS_TABLE_NAME " WHERE " BDS_ID_COLUMN "=?");
    q.addBindValue(id);
    DatabaseUtils::ExecuteQuery(q);

    if(q.next())
        ret = q.record().value(0).toInt();

    return ret;
}

QUuid BinaryDataStore::GetVersion(int id) const
{
    QUuid ret;

    QSqlQuery q(QSqlDatabase::database(m_connString));
    q.prepare("SELECT " BDS_VERSION_COLUMN " FROM " BDS_TABLE_NAME " WHERE " BDS_ID_COLUMN "=?");
    q.addBindValue(id);
    DatabaseUtils::ExecuteQuery(q);

    if(q.next())
        ret = q.record().value(0).toString();

    return ret;
}

void BinaryDataStore::Clear()
{
    QSqlQuery q(QSqlDatabase::database(m_connString));
    q.prepare("DELETE FROM " BDS_TABLE_NAME);
    DatabaseUtils::ExecuteQuery(q);
}

Vector< Pair<int, QUuid> > BinaryDataStore::GetIds() const
{
    Vector< Pair<int, QUuid> > ret;

    QSqlQuery q(QSqlDatabase::database(m_connString));
    q.prepare("SELECT " BDS_ID_COLUMN "," BDS_VERSION_COLUMN " FROM " BDS_TABLE_NAME);
    DatabaseUtils::ExecuteQuery(q);

    while(q.next())
        ret.PushBack(Pair<int, QUuid>(q.record().value(0).toInt(), QUuid(q.record().value(1).toString())));

    return ret;
}


END_NAMESPACE_GUTIL1;

#endif // GUTIL_NO_DATABASE_FUNCTIONALITY
