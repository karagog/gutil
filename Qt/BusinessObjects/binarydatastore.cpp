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

#ifdef DATABASE_FUNCTIONALITY


#include "binarydatastore.h"
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
USING_NAMESPACE_GUTIL2(QT, DataAccess);
USING_NAMESPACE_GUTIL2(QT, DataObjects);
USING_NAMESPACE_GUTIL1(DataObjects);
USING_NAMESPACE_GUTIL1(Utils);

NAMESPACE_GUTIL2(QT, BusinessObjects);


#define BS_TABLE_NAME "data"

BinaryDataStore::BinaryDataStore(const QString &id)
    :dbio(new GDatabaseIODevice(QString("%1_database").arg(id))),
      _file_location(_get_file_loc(id)),
      _max_id(0)
{}

int BinaryDataStore::AddFile(const QByteArray &data, int id)
{
    int ret(id);
    DatabaseValueParameters params(
            dbio->GetBlankValueParameters(BS_TABLE_NAME));

    if(id <= 0 || !HasFile(id))
    {
        // Insert a new record
        VariantTable t(dbio->GetBlankTable(BS_TABLE_NAME));
        {
            SharedSmartPointer< DataRow<QVariant> > r(t.AddRow());
            (*r)["size"] = data.length();
            (*r)["data"] = data;
        }

        if(id <= 0)
            id = GetFreeId();

        t[0]["id"] = id;

        dbio->Insert(t);
        ret = dbio->LastInsertId();

        _ids.Insert(ret);
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
    DatabaseSelectionParameters s(
            dbio->GetBlankSelectionParameters(BS_TABLE_NAME));

    s.FilterValues()["id"] = id;
    dbio->Delete(s);

    _ids.RemoveAll(id);
}

QByteArray BinaryDataStore::GetFile(int id)
{
    DatabaseSelectionParameters s(
            dbio->GetBlankSelectionParameters(BS_TABLE_NAME));
    s.FilterValues()["id"] = id;

    VariantTable t(dbio->Select(s, QStringList("data")));

    if(t.RowCount() == 0)
        THROW_NEW_GUTIL_EXCEPTION2( Exception, "File not found" );

    return t[0]["data"].Value().toByteArray();
}

int BinaryDataStore::GetSize(int id)
{
    DatabaseSelectionParameters s(
            dbio->GetBlankSelectionParameters(BS_TABLE_NAME));
    s.FilterValues()["id"] = id;

    VariantTable t(dbio->Select(s, QStringList("size")));

    if(t.RowCount() == 0)
        THROW_NEW_GUTIL_EXCEPTION2( Exception, "File not found" );

    return t[0]["size"].Value().toInt();
}

void BinaryDataStore::Initialize()
{
    if(QFile::exists(_file_location))
        QFile::remove(_file_location);

    QSqlDatabase::addDatabase(
            "QSQLITE",
            dbio->GetIdentity())
            .setDatabaseName(_file_location);

    dbio->OpenDatabaseConnection();

    // Create a table with two columns, an integer primary key and blob,
    //  dropping the table if it already exists
    PairList<QString, QString> pl;
    pl
            << Pair<QString, QString>("id", "INTEGER")
            << Pair<QString, QString>("size", "INTEGER")
            << Pair<QString, QString> ("data", "BLOB");
    dbio->CreateTable(BS_TABLE_NAME, pl, 0, true);
}

void BinaryDataStore::Clear()
{
    _ids.Clear();
    dbio->CloseDatabaseConnection();

    if(QFile::exists(_file_location))
        QFile::remove(_file_location);
}

QString BinaryDataStore::_get_file_loc(const QString &id)
{
    return QDesktopServices::storageLocation(QDesktopServices::TempLocation)
            + QString("/%1.GUTIL.sqlite").arg(id);
}

int BinaryDataStore::GetFreeId()
{
    while(_ids.Contains(++_max_id))
    {
        // Once we hit 4 billion we could roll over into negatives
        if(_max_id < 0)
            _max_id = 1;
    }
    return _max_id;
}


END_NAMESPACE_GUTIL2;

#endif // DATABASE_FUNCTIONALITY
