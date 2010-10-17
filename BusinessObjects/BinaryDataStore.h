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

#include "Core/Interfaces/ireadonlyobject.h"
#include <QString>
#include <QMutex>
#include <QSqlDatabase>
#include <QMap>

// Use this class to manage binary data.  This is useful for storing and accessing
//  lots of files without consuming lots of memory

namespace GUtil
{
    namespace BusinessObjects
    {
        class BinaryDataStore : public Core::Interfaces::IReadOnlyObject
        {
        public:

            explicit BinaryDataStore(const QString&, bool primary = true);
            virtual ~BinaryDataStore();

            //  (All of the public functions can be treated atomically,
            //   and can be executed safely from multiple threads)

            // For manipulating files
            int addFile(const QString &);
            int addFile(int, const QString &);
            void removeFile(int);
            QString getFile(int);
            bool hasFile(int id);

            // Clear all files
            void reset();

            // List the id's that we've got
            QList<int> idList();

            // Set aside the list of ids
            bool reserveIdList(const QList<int> &);

        private:
            QString my_id;
            QString file_location;

            int get_free_file_id(QSqlDatabase &);

            void prep_database(QSqlDatabase &);
            static QString get_file_loc(const QString &id);

            int add_file(int, const QString &, QSqlDatabase&);
            bool has_file(int id, QSqlDatabase &);
            void remove_file(int, QSqlDatabase &);

            void _execute_insertion(QSqlQuery &, int, const QString &);
        };
    }
}

#endif // FILE_MANAGER_H
