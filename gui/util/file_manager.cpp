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

QMap<QString, QReadWriteLock*> mutexes;

File_Manager::File_Manager(QString unique_id)
{
    my_id = unique_id;
    file_location = get_file_loc(my_id);

    if(!mutexes.contains(my_id))
        mutexes.insert(my_id, new QReadWriteLock());

    QSqlDatabase::addDatabase("QSQLITE");

    reset();
}

File_Manager::~File_Manager()
{
    foreach(QString k, mutexes.keys())
    {
        delete mutexes.value(k);
    }
}

int File_Manager::addFile(const QString &data)
{
    mutexes.value(my_id)->lockForWrite();

    QSqlDatabase dbase;
    get_database(dbase);

    QSqlQuery q("INSERT INTO files (id, data) VALUES (:id, :data)", dbase);
    q.bindValue(":id", QVariant(max_id));
    q.bindValue(":data", data, QSql::Binary);
    if(!q.exec())
        throw GUtil::Exception(q.lastError().text().toStdString());
    int ret = max_id++;

    dbase.commit();
    dbase.close();
    mutexes.value(my_id)->unlock();
    return ret;
}

QString File_Manager::getFile(int id) const
{
    mutexes.value(my_id)->lockForRead();

    QSqlDatabase dbase;
    get_database(dbase);

    QSqlQuery q("SELECT data FROM files WHERE id=:id");
    q.bindValue(":id", QVariant(id));

    if(!q.exec())
        throw GUtil::Exception(q.lastError().text().toStdString());
    if(!q.first())
        throw GUtil::Exception("File not found");

    QByteArray ba = q.value(0).toByteArray();

    dbase.close();
    mutexes.value(my_id)->unlock();

    return QString::fromStdString(string(ba.constData(), ba.length()));
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
    mutexes.value(my_id)->lockForWrite();

    QSqlDatabase dbase;
    get_database(dbase);

    QSqlQuery q(dbase);
    q.exec("DROP TABLE \"files\"");
    q.exec("CREATE TABLE \"files\" ("
               "\"id\" INTEGER NOT NULL,"
               "\"data\" BLOB NOT NULL)");
    q.exec("CREATE INDEX idx ON files(id)");
    dbase.close();

    max_id = 0;

    mutexes.value(my_id)->unlock();
}

QString File_Manager::get_file_loc(const QString &id)
{
    return QDesktopServices::storageLocation(QDesktopServices::TempLocation)
            + QString("/%1.tempfile").arg(id);
}
