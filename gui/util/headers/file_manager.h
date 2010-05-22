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
#include <QMap>
#include <QUuid>

// Use this class to manage binary data.  This is useful for storing and accessing
//  lots of files without consuming lots of memory

class File_Manager
{
public:
    // If 'is_secondary' is true, then it won't automatically reset the file when
    //  it initializes itself.  Use this to preserve existing data
    File_Manager(const QString &passphrase = QUuid::createUuid().toString(),
                 bool is_secondary = false);

    // Clear all files
    void reset();

    QString passphrase();

    // Call beginTransaction before accessing any of the files
    bool beginTransaction();

    // Use these as an interface to access files (accepts and returns binary data strings)
    unsigned int addFile(const QString &data);
    QString getFile(unsigned int id) const;

    // Be sure to call endTransaction when you're done to commit the data you've
    //  changed and/or to free up the resources it uses to remember the files
    bool endTransaction();

private:
    QString file_location;
    QString encryption_passphrase;
    QMap<unsigned int, QString> _map;
    unsigned int max_id;

    bool _transaction;
    bool _changed;
    bool _was_reset;

    static QString get_new_file_loc();
};

#endif // FILE_MANAGER_H
