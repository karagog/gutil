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

#include "file_manager.h"
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
#include <iostream>
using namespace GUtil;
using namespace GUtil::QtUtil;

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

File_Manager::File_Manager(const QString &id, bool primary)
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
        QSqlDatabase::addDatabase("QSQLITE");
        reset();
    }
}

File_Manager::~File_Manager()
{

}

int File_Manager::addFile(const QString &data)
{
    mutex_lock.lockForRead();
    mutexes.value(my_id)->mut->lockForWrite();

    QSqlDatabase dbase;
    get_database(dbase);
    if(!dbase.open())
    {
        mutexes.value(my_id)->mut->unlock();
        mutex_lock.unlock();
        throw GUtil::Exception("Cannot open database");
    }

    int ret = get_max_file_id(dbase);

    QSqlQuery q("INSERT INTO files (id, data) VALUES (:id, :data)", dbase);
    q.bindValue(":id", ret);
    q.bindValue(":data", data, QSql::Binary);
    if(!q.exec())
    {
        QString err = q.lastError().text();
        dbase.close();
        mutexes.value(my_id)->mut->unlock();
        mutex_lock.unlock();
        throw GUtil::Exception(err.toStdString());
    }

    dbase.close();
    mutexes.value(my_id)->mut->unlock();
    mutex_lock.unlock();

    return ret;
}

void File_Manager::removeFile(int id)
{
    mutex_lock.lockForRead();
    mutexes.value(my_id)->mut->lockForWrite();

    QSqlDatabase dbase;
    get_database(dbase);
    if(!dbase.open())
    {
        mutexes.value(my_id)->mut->unlock();
        mutex_lock.unlock();
        throw GUtil::Exception("Cannot open database");
    }

    // Remove each item one by one
    QSqlQuery q("DELETE FROM files WHERE id=:id");
    q.bindValue(":id", id);
    if(!q.exec() || (q.numRowsAffected() != 1))
    {
        QString err = q.lastError().text();
        dbase.close();
        mutexes.value(my_id)->mut->unlock();
        mutex_lock.unlock();
        throw GUtil::Exception(err.toStdString());
    }

    dbase.close();
    mutexes.value(my_id)->mut->unlock();
    mutex_lock.unlock();
}

QString File_Manager::getFile(int id)
{
    mutex_lock.lockForRead();
    mutexes.value(my_id)->mut->lockForRead();

    QSqlDatabase dbase;
    get_database(dbase);
    if(!dbase.open())
    {
        mutexes.value(my_id)->mut->unlock();
        mutex_lock.unlock();
        throw GUtil::Exception("Could not open database");
    }

    QSqlQuery q("SELECT data FROM files WHERE id=:id", dbase);
    q.bindValue(":id", id);
    q.exec();

    if(q.first())
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
void File_Manager::reset()
{
    mutex_lock.lockForRead();
    mutexes.value(my_id)->mut->lockForWrite();

    QSqlDatabase dbase;
    get_database(dbase);
    if(!dbase.open())
    {
        mutexes.value(my_id)->mut->unlock();
        mutex_lock.unlock();
        throw GUtil::Exception("Cannot open database");
    }

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

void File_Manager::get_database(QSqlDatabase &dbase) const
{
    dbase = QSqlDatabase::database();
    dbase.setDatabaseName(file_location);
}

QString File_Manager::get_file_loc(const QString &id)
{
    return QDesktopServices::storageLocation(QDesktopServices::TempLocation)
            + QString("/%1.tempfile").arg(id);
}

QList<int> File_Manager::idList()
{
    mutex_lock.lockForRead();
    mutexes.value(my_id)->mut->lockForRead();

    QList<int> ret;
    QSqlDatabase dbase;
    get_database(dbase);
    if(!dbase.open())
    {
        mutexes.value(my_id)->mut->unlock();
        mutex_lock.unlock();
        throw GUtil::Exception("Cannot open database");
    }

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

bool File_Manager::hasFile(int id)
{
    mutex_lock.lockForRead();
    mutexes.value(my_id)->mut->lockForRead();

    QSqlDatabase dbase;
    get_database(dbase);
    if(!dbase.open())
    {
        mutexes.value(my_id)->mut->unlock();
        mutex_lock.unlock();
        throw GUtil::Exception("Cannot open database");
    }

    QSqlQuery q("SELECT id FROM files WHERE id=:id");
    q.bindValue(":id", id);
    q.exec();
    bool ret = q.first();

    mutexes.value(my_id)->mut->unlock();
    mutex_lock.unlock();
    return ret;
}

int File_Manager::get_max_file_id(QSqlDatabase &dbase)
{
    // You must already have a lock on the database before using this function!

    int max_id = 0;
    QSqlQuery q("SELECT COUNT(id),MAX(id) FROM files", dbase);
    q.exec();
    if(q.first())
    {
        if(q.value(0).toInt() > 0)
            max_id = q.value(1).toInt() + 1;
    }

    return max_id;
}
