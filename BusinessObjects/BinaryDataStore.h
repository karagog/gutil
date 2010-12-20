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

#include "giodevicebundlemanager.h"
#include "Core/Interfaces/ireadonlyobject.h"
#include <QString>
#include <QSet>

// Use this class to manage binary data.  This is useful for storing and accessing
//  lots of files without consuming lots of memory

namespace GUtil
{
    namespace DataAccess
    {
        class GDatabaseIODevice;
    }

    namespace BusinessObjects
    {
        class BinaryDataStore :
                public Core::Interfaces::IReadOnlyObject
        {
        public:

            explicit BinaryDataStore(const QString &);

            //  (All of the public functions can be treated atomically,
            //   and can be executed safely from multiple threads)

            // For manipulating files
            int AddFile(const QString &, int id = -1);
            void RemoveFile(int);
            QString GetFile(int);
            bool HasFile(int id);

            // Remove all files
            void Reset();

            inline QSet<int> GetIds() const { return _ids; }

            inline void SetAsyncWrite(bool b){
                _file_manager.SetAsyncWrite(b);
            }
            inline bool GetAsyncWrite() const{
                return _file_manager.GetAsyncWrite();
            }


        private:

            QString _my_id;
            QString _file_location;

            QSet<int> _ids;
            int _max_id;

            QString _database_connection_string;

            void _get_free_file_id();

            static QString _get_file_loc(const QString &id);

            // For asynchronous writes to the database
            GIODeviceBundleManager _file_manager;
            DataAccess::GDatabaseIODevice *_dbio;

        };
    }
}

#endif // FILE_MANAGER_H
