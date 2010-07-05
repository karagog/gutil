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

#include "DA_BinaryDataStore.h"
#include "stringhelpers.h"
#include "encryption.h"
#include "exception.h"
#include <QDesktopServices>
#include <QFile>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QVariant>
#include <QMutex>
#include <QReadWriteLock>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlResult>
#include <QMap>
#include <QMessageBox>
#include <QByteArray>
using namespace GUtil;
using namespace GUtil::QtUtil::DataAccess;

// A class to keep track of our mutexes
class mutex_record_t
{
public:
    mutex_record_t()
    {
        count = 1;
        mut = new QReadWriteLock();
    }
    ~mutex_record_t(){ delete mut;}

    QReadWriteLock *mut;
    int count;
};

QMap<QString, mutex_record_t *> mutexes;
QReadWriteLock mutex_lock;

DA_BinaryDataStore::DA_BinaryDataStore(const QString &id, bool primary)
{
    my_id = id;
    file_location = get_file_loc(id);

    mutex_lock.lockForWrite();
    if(mutexes.contains(id))
    {
        mutexes.value(id)->mut->lockForWrite();

        mutexes[id]->count++;

        mutexes.value(id)->mut->unlock();
    }
    else
    {
        mutexes.insert(id, new mutex_record_t());
    }
    mutex_lock.unlock();

    if(primary)
    {
        if(QFile::exists(file_location))
            QFile::remove(file_location);

        QSqlDatabase::addDatabase("QSQLITE").setDatabaseName(file_location);
        reset();
    }
}

DA_BinaryDataStore::~DA_BinaryDataStore()
{

}

int DA_BinaryDataStore::addFile(const QString &data)
{
    mutex_lock.lockForRead();
    mutexes.value(my_id)->mut->lockForWrite();

    QSqlDatabase dbase;
    prep_database(dbase);

    int ret = get_free_file_id(dbase);
    add_file(ret, data, dbase);

    dbase.close();
    mutexes.value(my_id)->mut->unlock();
    mutex_lock.unlock();

    return ret;
}

int DA_BinaryDataStore::addFile(int id, const QString &data)
{
    mutex_lock.lockForRead();
    mutexes.value(my_id)->mut->lockForWrite();

    QSqlDatabase dbase;
    prep_database(dbase);

    int ret = add_file(id, data, dbase);

    dbase.close();
    mutexes.value(my_id)->mut->unlock();
    mutex_lock.unlock();

    return ret;
}

int DA_BinaryDataStore::add_file(int id, const QString &data, QSqlDatabase &dbase)
{
    if(has_file(id, dbase))
        remove_file(id, dbase);

    QSqlQuery q("INSERT INTO files (id, data) VALUES (:id, :data)", dbase);

    _execute_insertion(q, id, data);

    return id;
}

void DA_BinaryDataStore::_execute_insertion(QSqlQuery &q, int id, const QString &data)
{
    q.bindValue(":id", id);
    q.bindValue(":data", data, QSql::Binary);
    if(!q.exec())
        throw GUtil::Exception(q.lastError().text().toStdString());
}

void DA_BinaryDataStore::removeFile(int id)
{
    mutex_lock.lockForRead();
    mutexes.value(my_id)->mut->lockForWrite();

    QSqlDatabase dbase;
    prep_database(dbase);

    remove_file(id, dbase);

    dbase.close();
    mutexes.value(my_id)->mut->unlock();
    mutex_lock.unlock();
}

void DA_BinaryDataStore::remove_file(int id, QSqlDatabase &dbase)
{
    // Remove each item one by one
    QSqlQuery q("DELETE FROM files WHERE id=:id", dbase);
    q.bindValue(":id", id);
    q.exec();
}

QString DA_BinaryDataStore::getFile(int id)
{
    mutex_lock.lockForRead();
    mutexes.value(my_id)->mut->lockForRead();

    QSqlDatabase dbase;
    prep_database(dbase);

    QSqlQuery q("SELECT data, COUNT(data) FROM files WHERE id=:id", dbase);
    q.bindValue(":id", id);
    q.exec();

    if(q.first() && (q.value(1).toInt() == 1))
    {
        QByteArray ba = q.value(0).toByteArray();
        mutexes.value(my_id)->mut->unlock();
        mutex_lock.unlock();
        return QString::fromStdString(string(ba.constData(), ba.length()));
    }

    dbase.close();
    mutexes.value(my_id)->mut->unlock();
    mutex_lock.unlock();
    throw GUtil::Exception("File not found");
}

// Clear all files
void DA_BinaryDataStore::reset()
{
    mutex_lock.lockForRead();
    mutexes.value(my_id)->mut->lockForWrite();

    QSqlDatabase dbase;
    prep_database(dbase);

    QSqlQuery q(dbase);
    q.exec("DROP TABLE \"files\"");
    q.exec("CREATE TABLE \"files\" ("
               "\"id\" INTEGER NOT NULL,"
               "\"data\" BLOB NOT NULL)");
    q.exec("CREATE INDEX idx ON files(id)");
    dbase.close();

    mutexes.value(my_id)->mut->unlock();
    mutex_lock.unlock();
}

void DA_BinaryDataStore::prep_database(QSqlDatabase &dbase)
{
    dbase = QSqlDatabase::database();
    if(!dbase.open())
    {
        mutexes.value(my_id)->mut->unlock();
        mutex_lock.unlock();
        throw GUtil::Exception("Cannot open database");
    }
}

QString DA_BinaryDataStore::get_file_loc(const QString &id)
{
    return QDesktopServices::storageLocation(QDesktopServices::TempLocation)
            + QString("/%1.tempfile").arg(id);
}

QList<int> DA_BinaryDataStore::idList()
{
    mutex_lock.lockForRead();
    mutexes.value(my_id)->mut->lockForRead();

    QList<int> ret;
    QSqlDatabase dbase;
    prep_database(dbase);

    QSqlQuery q("SELECT id FROM files");
    if(q.exec())
    {
        while(q.next())
        {
            ret.append(q.value(0).toInt());
        }
    }

    dbase.close();
    mutexes.value(my_id)->mut->unlock();
    mutex_lock.unlock();
    return ret;
}

bool DA_BinaryDataStore::hasFile(int id)
{
    mutex_lock.lockForRead();
    mutexes.value(my_id)->mut->lockForRead();

    QSqlDatabase dbase;
    prep_database(dbase);

    bool ret = has_file(id, dbase);

    mutexes.value(my_id)->mut->unlock();
    mutex_lock.unlock();
    return ret;
}

bool DA_BinaryDataStore::has_file(int id, QSqlDatabase &dbase)
{
    QSqlQuery q("SELECT id FROM files WHERE id=:id", dbase);
    q.bindValue(":id", id);
    q.exec();
    return q.first();
}

int DA_BinaryDataStore::get_free_file_id(QSqlDatabase &dbase)
{
    // You must already have a lock on the database before using this function!

    int max_id = 0;
    bool found_id = false;

    QSqlQuery q("SELECT COUNT(id),MAX(id) FROM files", dbase);
    q.exec();
    if(q.first())
    {
        if(q.value(0).toInt() > 0)
        {
            max_id = q.value(1).toInt();
            found_id = true;
        }
    }

    bool first_time = true;
    do
    {
        if(first_time)
        {
            first_time = false;
            if(found_id)
                max_id++;
        }
        else
            max_id++;

        if(max_id < 0)
            max_id = 0;

    // Make sure the id isn't taken
    }while(has_file(max_id, dbase));

    return max_id;
}

bool DA_BinaryDataStore::reserveIdList(const QList<int> &list)
{
    mutex_lock.lockForRead();
    mutexes.value(my_id)->mut->lockForWrite();

    bool ret = true;
    QSqlDatabase dbase;
    prep_database(dbase);

    QSqlQuery q("INSERT INTO files (id, data) VALUES (:id, :data)", dbase);

    foreach(int id, list)
        _execute_insertion(q, id, "");

    dbase.close();
    mutexes.value(my_id)->mut->unlock();
    mutex_lock.unlock();

    return ret;
}
