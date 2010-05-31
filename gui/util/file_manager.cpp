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
        worker = 0;
    }
    ~mutex_record_t(){ delete mut; delete worker; }

    QReadWriteLock *mut;
    int count;
    File_Manager_worker *worker;
};

QMap<QString, mutex_record_t *> mutexes;
QReadWriteLock mutex_lock;

File_Manager_worker::File_Manager_worker(const QString &id,
                                         bool is_secondary, QObject *parent)
    :QThread(parent)
{
    max_file_id = 0;
    my_id = id;
    file_location = get_file_loc(id);

    if(!is_secondary)
    {
        QSqlDatabase::addDatabase("QSQLITE");
        reset();
    }
}

File_Manager_worker::~File_Manager_worker()
{

}

void File_Manager_worker::run()
{
    // Process the items in both queues
    mutex_lock.lockForRead();
    mutexes.value(my_id)->mut->lockForWrite();

    QSqlDatabase dbase;
    get_database(dbase);
    if(!dbase.open())
    {
        mutexes.value(my_id)->mut->unlock();
        mutex_lock.unlock();
        throw GUtil::Exception("Could not open database");
    }


    // First process the remove queue
    int lim = rem_q.count();
    for(int i = 0; i < lim; i++)
    {
        // Remove each item one by one
        if(add_q.contains(rem_q.at(i)))
        {
            add_q.remove(rem_q.at(i));
        }
        else
        {
            QSqlQuery q("DELETE FROM files WHERE id=:id");
            q.bindValue(":id", rem_q.at(i));
            if(!q.exec() || (q.numRowsAffected() != 1))
            {
                QString err = q.lastError().text();
                mutexes.value(my_id)->mut->unlock();
                mutex_lock.unlock();
                throw GUtil::Exception(err.toStdString());
            }
        }
    }

    rem_q.clear();


    // Then add items from the add queue
    lim = add_q.keys().count();
    for(int i = 0; i < lim; i++)
    {
        QSqlQuery q("INSERT INTO files (id, data) VALUES (:id, :data)");
        q.bindValue(":id", add_q.keys().at(i));
        q.bindValue(":data", add_q.value(add_q.keys().at(i)), QSql::Binary);
        if(!q.exec())
        {
            QString err = q.lastError().text();
            mutexes.value(my_id)->mut->unlock();
            mutex_lock.unlock();
            throw GUtil::Exception(err.toStdString());
        }
    }

    add_q.clear();

    dbase.close();
    mutexes.value(my_id)->mut->unlock();
    mutex_lock.unlock();
}

int File_Manager_worker::addFile(const QString &data)
{
    mutex_lock.lockForRead();
    mutexes.value(my_id)->mut->lockForWrite();

    int ret = max_file_id++;
    add_q.insert(ret, data);

    mutexes.value(my_id)->mut->unlock();
    mutex_lock.unlock();

    return ret;
}

void File_Manager_worker::removeFile(int id)
{
    mutex_lock.lockForRead();
    mutexes.value(my_id)->mut->lockForWrite();

    rem_q.append(id);

    mutexes.value(my_id)->mut->unlock();
    mutex_lock.unlock();
}

QString File_Manager_worker::getFile(int id)
{
    mutex_lock.lockForRead();
    mutexes.value(my_id)->mut->lockForRead();

    QString retval;

    // First check quickly if we're holding it in our memory cache
    if(add_q.contains(id))
    {
        retval = add_q.value(id);
    }
    else
    {
        QSqlDatabase dbase;
        get_database(dbase);
        if(!dbase.open())
        {
            mutexes.value(my_id)->mut->unlock();
            mutex_lock.unlock();
            throw GUtil::Exception("Could not open database");
        }

        QSqlQuery q("SELECT data FROM files WHERE id=:id");
        q.bindValue(":id", QVariant(id));

        if(!q.exec())
        {
            dbase.close();
            mutexes.value(my_id)->mut->unlock();
            mutex_lock.unlock();
            throw GUtil::Exception(q.lastError().text().toStdString());
        }

        if(q.first())
        {
            QByteArray ba = q.value(0).toByteArray();
            retval = QString::fromStdString(string(ba.constData(), ba.length()));
        }
        else
        {
            dbase.close();
            mutexes.value(my_id)->mut->unlock();
            mutex_lock.unlock();
            throw GUtil::Exception("File not found");
        }
    }

    mutexes.value(my_id)->mut->unlock();
    mutex_lock.unlock();

    return retval;
}

// Clear all files
void File_Manager_worker::reset()
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

    max_file_id = 0;
    add_q.clear();
    rem_q.clear();

    mutexes.value(my_id)->mut->unlock();
    mutex_lock.unlock();
}

void File_Manager_worker::get_database(QSqlDatabase &dbase) const
{
    dbase = QSqlDatabase::database();
    dbase.setDatabaseName(file_location);
}

QString File_Manager_worker::get_file_loc(const QString &id)
{
    return QDesktopServices::storageLocation(QDesktopServices::TempLocation)
            + QString("/%1.tempfile").arg(id);
}

QList<int> File_Manager_worker::idList()
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

    for(int i = 0; i < add_q.uniqueKeys().count(); i++)
        ret.append(add_q.uniqueKeys().at(i));

    dbase.close();
    mutexes.value(my_id)->mut->unlock();
    mutex_lock.unlock();
    return ret;
}

bool File_Manager_worker::in_map(int id)
{
    mutex_lock.lockForRead();
    mutexes.value(my_id)->mut->lockForRead();

    bool ret = false, found = false;
    if(rem_q.contains(id))
    {
        found = true;
        ret = false;
    }
    else
    {
        if(add_q.contains(id))
        {
            found = true;
            ret = true;
        }
    }

    if(found)
    {
        mutexes.value(my_id)->mut->unlock();
        mutex_lock.unlock();
        return ret;
    }

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

    ret = q.size();
    mutexes.value(my_id)->mut->unlock();
    mutex_lock.unlock();
    return ret;
}


File_Manager::File_Manager(const QString &unique_id, bool is_secondary)
{
    my_id = unique_id;

    mutex_lock.lockForWrite();
    if(mutexes.contains(unique_id))
    {
        mutexes.value(unique_id)->mut->lockForWrite();

        mutexes[unique_id]->count++;
        worker = mutexes[unique_id]->worker;

        mutexes.value(unique_id)->mut->unlock();
    }
    else
    {
        mutexes.insert(unique_id, new mutex_record_t());
        mutex_lock.unlock();
        worker = new File_Manager_worker(unique_id, is_secondary, this);
        mutex_lock.lockForRead();
        mutexes.value(unique_id)->worker = worker;
    }

    mutex_lock.unlock();
}

File_Manager::~File_Manager()
{
    mutex_lock.lockForRead();

    mutex_record_t *r = mutexes.value(my_id);
    r->count -= 1;
    if(r->count == 0)
    {
        mutex_lock.unlock();
        mutex_lock.lockForWrite();

        r->mut->lockForWrite();

        mutexes.remove(my_id);

        r->mut->unlock();
        delete r;
    }

    mutex_lock.unlock();
}

int File_Manager::addFile(const QString &data)
{
    return worker->addFile(data);
}

void File_Manager::removeFile(int id)
{
    return worker->removeFile(id);
}

QString File_Manager::getFile(int id) const
{
    return worker->getFile(id);
}

bool File_Manager::hasFile(int id)
{
    return worker->in_map(id);
}

void File_Manager::pushToDisk()
{
    worker->start();
}

QList<int> File_Manager::idList()
{
    return worker->idList();
}

void File_Manager::reset()
{
    worker->reset();
}

