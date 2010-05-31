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
                                         QMap<int, QString> &add, QMutex &add_m,
                                         QList<int> &rem, QMutex &rem_m,
                                         bool is_secondary, QObject *parent)
    :QThread(parent)
{
    add_q = &add;
    rem_q = &rem;

    add_mutex = &add_m;
    rem_mutex = &rem_m;

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
    rem_mutex->lock();

    int lim = rem_q->count();
    for(int i = 0; i < lim; i++)
    {
        // Remove each item one by one

    }

    rem_q->clear();
    rem_mutex->unlock();


    // Then add items from the add queue
    add_mutex->lock();

    lim = add_q->count();
    for(int i = 0; i < lim; i++)
    {
        QSqlQuery q("INSERT INTO files (id, data) VALUES (:id, :data)");
        q.bindValue(":id", max_file_id - (lim - i));
        q.bindValue(":data", add_q->value(i), QSql::Binary);
        if(!q.exec())
        {
            QString err = q.lastError().text();
            add_mutex->unlock();
            mutexes.value(my_id)->mut->unlock();
            mutex_lock.unlock();
            throw GUtil::Exception(err.toStdString());
        }
    }

    add_q->clear();
    add_mutex->unlock();

    dbase.close();
    mutexes.value(my_id)->mut->unlock();
    mutex_lock.unlock();
}

int File_Manager_worker::addFile(const QString &data)
{
    int ret = max_file_id++;
    add_mutex->lock();

    add_q->insert(ret, data);

    add_mutex->unlock();

    return ret;
}

QString File_Manager_worker::getFile(int id)
{
    // We must wait until this thread has completed before getting files
    while(isRunning())
        wait();

    QString retval;
    mutex_lock.lockForRead();
    mutexes.value(my_id)->mut->lockForRead();

    // First check quickly if we're holding it in our memory cache
    add_mutex->lock();
    if(add_q->contains(id))
    {
        retval = add_q->value(id);
    }
    else
    {
        QSqlDatabase dbase;
        get_database(dbase);
        if(!dbase.open())
        {
            add_mutex->unlock();
            mutexes.value(my_id)->mut->unlock();
            mutex_lock.unlock();
            throw GUtil::Exception("Could not open database");
        }

        QSqlQuery q("SELECT data FROM files WHERE id=:id");
        q.bindValue(":id", QVariant(id));

        if(!q.exec())
        {
            dbase.close();
            add_mutex->unlock();
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
            add_mutex->unlock();
            mutexes.value(my_id)->mut->unlock();
            mutex_lock.unlock();
            throw GUtil::Exception("File not found");
        }
    }

    add_mutex->unlock();
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

    add_mutex->lock();
    add_q->clear();
    add_mutex->unlock();

    rem_mutex->lock();
    rem_q->clear();
    rem_mutex->unlock();

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

    for(int i = 0; i < add_q->uniqueKeys().count(); i++)
        ret.append(add_q->uniqueKeys().at(i));

    dbase.close();
    mutexes.value(my_id)->mut->unlock();
    mutex_lock.unlock();
    return ret;
}



File_Manager::File_Manager(const QString &unique_id, bool is_secondary)
{
    my_id = unique_id;

    mutex_lock.lockForRead();
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
        worker = new File_Manager_worker(unique_id,
                                         add_q, add_q_mutex,
                                         rem_q, rem_q_mutex,
                                         is_secondary, this);
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

QString File_Manager::getFile(int id) const
{
    return worker->getFile(id);
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

