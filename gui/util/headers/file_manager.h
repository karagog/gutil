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

#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <QString>
#include <QUuid>
#include <QSqlDatabase>

// Use this class to manage binary data.  This is useful for storing and accessing
//  lots of files without consuming lots of memory

class File_Manager
{
public:
    // If 'is_secondary' is true, then it won't automatically reset the file when
    //  it initializes itself.  Use this to preserve existing data
    File_Manager(QString unique_id, bool is_secondary = false);
    ~File_Manager();

    // The name of the SQlite database file
    QString filename();

    // Clear all files
    void reset();

    // Use these as an interface to access files (accepts and returns binary data strings)
    int addFile(const QString &data);
    QString getFile(int id) const;

private:
    QString file_location;
    QString my_id;

    bool get_database(QSqlDatabase &) const;
    static QString get_file_loc(const QString &id);
    void close_transaction(QSqlDatabase &dbase) const;
};

#endif // FILE_MANAGER_H
