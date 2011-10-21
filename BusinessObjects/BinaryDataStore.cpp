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

#ifdef DATABASE_FUNCTIONALITY


#include "BinaryDataStore.h"
#include "Core/Utils/stringhelpers.h"
#include "Core/Utils/encryption.h"
#include "Core/extendedexception.h"
#include <QDesktopServices>
#include <QFile>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QVariant>
#include <QMutex>
#include <QReadWriteLock>
#include <QMap>
#include <QMessageBox>
#include <QByteArray>
#include <QSqlDatabase>
GUTIL_USING_NAMESPACE(DataAccess);
GUTIL_USING_NAMESPACE(DataObjects);
GUTIL_USING_NAMESPACE(BusinessObjects);
using namespace std;

#define BS_TABLE_NAME "data"

BinaryDataStore::BinaryDataStore(const QString &id)
    :IReadOnlyObject(false),
    _file_location(_get_file_loc(id)),
    _max_id(0)
{
    dbio = new GDatabaseIODevice(QString("%1_database").arg(id));
}

BinaryDataStore::~BinaryDataStore()
{
    delete dbio;
}

int BinaryDataStore::AddFile(const QByteArray &data, int id)
{
    FailIfReadOnly();

    int ret(id);
    DatabaseValueParameters params(
            dbio->GetBlankValueParameters(BS_TABLE_NAME));

    if(id <= 0 || !HasFile(id))
    {
        // Insert a new record
        DataTable t(dbio->GetBlankTable(BS_TABLE_NAME));
        {
            DataRow r(t.AddNewRow(
                          DataObjects::GVariantCollection(t.ColumnCount())));
            r["size"] = data.length();
            r["data"] = data;
        }

        if(id <= 0)
            id = GetFreeId();

        t[0]["id"] = id;

        dbio->Insert(t);
        ret = dbio->LastInsertId();

        _ids.insert(ret);
    }
    else
    {
        // Update an existing record
        DatabaseSelectionParameters select(
                dbio->GetBlankSelectionParameters(BS_TABLE_NAME));
        select.FilterValues()["id"] = id;
        params.Values()["size"] = data.length();
        params.Values()["data"] = data;

        dbio->Update(select, params);
    }

    return ret;
}

void BinaryDataStore::RemoveFile(int id)
{
    FailIfReadOnly();

    DatabaseSelectionParameters s(
            dbio->GetBlankSelectionParameters(BS_TABLE_NAME));

    s.FilterValues()["id"] = id;
    dbio->Delete(s);

    _ids.remove(id);
}

QByteArray BinaryDataStore::GetFile(int id) const
{
    DatabaseSelectionParameters s(
            dbio->GetBlankSelectionParameters(BS_TABLE_NAME));
    s.FilterValues()["id"] = id;

    DataTable t(dbio->Select(s, QStringList("data")));

    if(t.RowCount() == 0)
        THROW_NEW_GUTIL_EXCEPTION2( Core::Exception, "File not found" );

    return t[0]["data"].toByteArray();
}

int BinaryDataStore::GetSize(int id) const
{
    DatabaseSelectionParameters s(
            dbio->GetBlankSelectionParameters(BS_TABLE_NAME));
    s.FilterValues()["id"] = id;

    DataTable t(dbio->Select(s, QStringList("size")));

    if(t.RowCount() == 0)
        THROW_NEW_GUTIL_EXCEPTION2( Core::Exception, "File not found" );

    return t[0]["size"].toInt();
}

void BinaryDataStore::Initialize()
{
    FailIfReadOnly();

    if(QFile::exists(_file_location))
        QFile::remove(_file_location);

    QSqlDatabase::addDatabase(
            "QSQLITE",
            dbio->GetIdentity())
            .setDatabaseName(_file_location);

    dbio->OpenDatabaseConnection();

    // Create a table with two columns, an integer primary key and blob,
    //  dropping the table if it already exists
    Custom::GPairList<QString, QString> pl;
    pl
            << QPair<QString, QString>("id", "INTEGER")
            << QPair<QString, QString>("size", "INTEGER")
            << QPair<QString, QString> ("data", "BLOB");
    dbio->CreateTable(BS_TABLE_NAME, pl, 0, true);
}

void BinaryDataStore::Clear()
{
    FailIfReadOnly();

    _ids.clear();
    dbio->CloseDatabaseConnection();

    if(QFile::exists(_file_location))
        QFile::remove(_file_location);
}

void BinaryDataStore::Reset()
{
    Clear();
    Initialize();
}

QString BinaryDataStore::_get_file_loc(const QString &id)
{
    return QDesktopServices::storageLocation(QDesktopServices::TempLocation)
            + QString("/%1.GUTIL.sqlite").arg(id);
}

bool BinaryDataStore::HasFile(int id) const
{
    return _ids.contains(id);
}

int BinaryDataStore::GetFreeId()
{
    while(_ids.contains(++_max_id))
    {
        // Once we hit 4 billion we could roll over into negatives
        if(_max_id < 0)
            _max_id = 1;
    }
    return _max_id;
}


#endif // DATABASE_FUNCTIONALITY
