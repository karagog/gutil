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

#include "BinaryDataStore.h"
#include "DataAccess/gdatabaseiodevice.h"
#include "Core/Utils/stringhelpers.h"
#include "Core/Utils/encryption.h"
#include "Core/exception.h"
#include <QDesktopServices>
#include <QFile>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QVariant>
#include <QMutex>
#include <QReadWriteLock>
#include <QSqlDatabase>
#include <QMap>
#include <QMessageBox>
#include <QByteArray>
using namespace GUtil;
using namespace DataObjects;
using namespace DataAccess;
using namespace BusinessObjects;
using namespace std;

#define BS_TABLE_NAME "data"

BusinessObjects::BinaryDataStore::BinaryDataStore(const QString &id)
    :IReadOnlyObject(false),
    _my_id(id),
    _file_location(_get_file_loc(id)),
    _max_id(0),
    _database_connection_string(QString("%1_database").arg(id))
{
    if(!QSqlDatabase::database(_database_connection_string).isValid())
    {
        if(QFile::exists(_file_location))
            QFile::remove(_file_location);

        QSqlDatabase::addDatabase(
                "QSQLITE",
                _database_connection_string)
                .setDatabaseName(_file_location);
    }

    _dbio = new GDatabaseIODevice(_database_connection_string, &_file_manager);

    _file_manager.InsertIntoBundle(_dbio);
    _file_manager.SetDropInput(true);

    Reset();
}

int BusinessObjects::BinaryDataStore::AddFile(const QString &data, int id)
{
    FailIfReadOnly();

    int ret(id);
    DatabaseValueParameters params(
            _dbio->GetBlankValueParameters(BS_TABLE_NAME));
    params.ColumnOptions()[1] |= GDatabaseIODevice::Binary;

    if(id == -1 || !HasFile(id))
    {
        // Insert a new record
        DataTable t(_dbio->GetBlankTable(BS_TABLE_NAME));
        t.AddNewRow(Custom::GVariantList() << Custom::GVariant() << data);

        if(id != -1)
            t[0][0] = id;

        _dbio->Insert(t, params);
        ret = _dbio->LastInsertId();

        _ids.insert(ret);
    }
    else
    {
        // Update an existing record
        DatabaseSelectionParameters select(
                _dbio->GetBlankSelectionParameters(BS_TABLE_NAME));
        select.FilterValues()[0] = id;
        params.Values()[1] = data;

        _dbio->Update(select, params);
    }

    return ret;
}

void BusinessObjects::BinaryDataStore::RemoveFile(int id)
{
    FailIfReadOnly();

    DatabaseSelectionParameters s(
            _dbio->GetBlankSelectionParameters(BS_TABLE_NAME));

    s.FilterValues()[0] = id;
    _dbio->Delete(s);

    _ids.remove(id);
}

QString BusinessObjects::BinaryDataStore::GetFile(int id)
{
    DatabaseSelectionParameters s(
            _dbio->GetBlankSelectionParameters(BS_TABLE_NAME));
    s.FilterValues()[0] = id;

    DataTable t(_dbio->Select(s, QList<int>() << 1));

    if(t.RowCount() == 0)
        THROW_NEW_GUTIL_EXCEPTION2( Core::Exception, "File not found" );

    return t[0][1].toString();
}

// Clear all files
void BusinessObjects::BinaryDataStore::Reset()
{
    FailIfReadOnly();

    // Create a table with two columns, an integer primary key and blob,
    //  dropping the table if it already exists
    Custom::GPairList<QString, QString> pl;
    pl <<
            QPair<QString, QString>("a", "INTEGER") <<
            QPair<QString, QString> ("b", "BLOB");
    _dbio->CreateTable(BS_TABLE_NAME, pl, 0, true);
}

QString BusinessObjects::BinaryDataStore::_get_file_loc(const QString &id)
{
    return QDesktopServices::storageLocation(QDesktopServices::TempLocation)
            + QString("/%1.tempfile").arg(id);
}

bool BusinessObjects::BinaryDataStore::HasFile(int id)
{
    return _ids.contains(id);
}
