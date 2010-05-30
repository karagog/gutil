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

class mutex_record_t
{
public:
    mutex_record_t()
    {
        count = 1;
        mut = new QReadWriteLock();
    }
    ~mutex_record_t(){ delete mut; }

    QReadWriteLock *mut;
    int count;
};

QMap<QString, mutex_record_t *> mutexes;

File_Manager::File_Manager(const QString &unique_id, bool is_secondary)
{
    my_id = unique_id;
    file_location = get_file_loc(my_id);

    if(mutexes.contains(my_id))
        mutexes[my_id]->count++;
    else
        mutexes.insert(my_id, new mutex_record_t());

    if(!is_secondary)
    {
        QSqlDatabase::addDatabase("QSQLITE");
        reset();
    }
}

File_Manager::~File_Manager()
{
    mutex_record_t *r = mutexes.value(my_id);
    r->count -= 1;
    if(r->count == 0)
    {
        r->mut->lockForWrite();

        mutexes.remove(my_id);

        r->mut->unlock();
        delete r;
    }
}

int File_Manager::addFile(const QString &data)
{
    mutexes.value(my_id)->mut->lockForWrite();

    QSqlDatabase dbase;
    get_database(dbase);

    int max_id = 0;
    QSqlQuery q("SELECT COUNT(id), MAX(id) FROM files", dbase);
    q.exec();
    if(q.first() && (q.value(0).toInt() > 0))
    {
        max_id = q.value(1).toInt() + 1;
    }
    q.clear();

    q.prepare("INSERT INTO files (id, data) VALUES (:id, :data)");
    q.bindValue(":id", max_id);
    q.bindValue(":data", data, QSql::Binary);
    if(!q.exec())
    {
        close_database(dbase);
        throw GUtil::Exception(q.lastError().text().toStdString());
    }

    close_database(dbase);
    return max_id;
}

QString File_Manager::getFile(int id) const
{
    mutexes.value(my_id)->mut->lockForRead();

    QSqlDatabase dbase;
    get_database(dbase);

    QSqlQuery q("SELECT data FROM files WHERE id=:id");
    q.bindValue(":id", QVariant(id));

    if(!q.exec())
    {
        close_database(dbase);
        throw GUtil::Exception(q.lastError().text().toStdString());
    }
    if(!q.first())
    {
        close_database(dbase);
        throw GUtil::Exception("File not found");
    }

    QByteArray ba = q.value(0).toByteArray();

    close_database(dbase);

    return QString::fromStdString(string(ba.constData(), ba.length()));
}

QList<int> File_Manager::idList()
{
    mutexes.value(my_id)->mut->lockForRead();

    QList<int> ret;
    QSqlDatabase dbase;
    get_database(dbase);

    QSqlQuery q("SELECT id FROM files");
    if(q.exec())
    {
        while(q.next())
        {
            ret.append(q.value(0).toInt());
        }
    }

    close_database(dbase);
    return ret;
}

void File_Manager::close_database(QSqlDatabase &dbase) const
{
    dbase.close();
    mutexes.value(my_id)->mut->unlock();
}

bool File_Manager::get_database(QSqlDatabase &dbase) const
{
    dbase = QSqlDatabase::database();
    dbase.setDatabaseName(file_location);

    if(!dbase.open())
    {
        return false;
    }

    return true;
}

void File_Manager::reset()
{
    mutexes.value(my_id)->mut->lockForWrite();

    QSqlDatabase dbase;
    get_database(dbase);

    QSqlQuery q(dbase);
    q.exec("DROP TABLE \"files\"");
    q.exec("CREATE TABLE \"files\" ("
               "\"id\" INTEGER NOT NULL,"
               "\"data\" BLOB NOT NULL)");
    q.exec("CREATE INDEX idx ON files(id)");
    dbase.close();

    mutexes.value(my_id)->mut->unlock();
}

QString File_Manager::filename()
{
    return file_location;
}

QString File_Manager::get_file_loc(const QString &id)
{
    return QDesktopServices::storageLocation(QDesktopServices::TempLocation)
            + QString("/%1.tempfile").arg(id);
}
