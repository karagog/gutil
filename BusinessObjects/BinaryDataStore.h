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
#include "DataAccess/gdatabaseiodevice.h"
#include <QString>
#include <QSet>

// Use this class to manage binary data.  This is useful for storing and accessing
//  lots of files without consuming lots of memory

namespace GUtil
{
    namespace BusinessObjects
    {
        class BinaryDataStore :
                public Core::Interfaces::IReadOnlyObject
        {
        public:

            explicit BinaryDataStore(const QString &);
            ~BinaryDataStore();

            //  (All of the public functions can be treated atomically,
            //   and can be executed safely from multiple threads)

            // For manipulating files
            int AddFile(const QByteArray &, int id = -1);
            void RemoveFile(int);
            QByteArray GetFile(int) const;
            bool HasFile(int id) const;

            // Get the size of a file
            int GetSize(int id) const;

            // Remove all files
            void Reset();

            inline QSet<int> GetIds() const { return _ids; }

            inline QString FileName() const{
                return _file_location;
            }


        protected:

            DataAccess::GDatabaseIODevice *dbio;


        private:

            QString _file_location;

            QSet<int> _ids;

            static QString _get_file_loc(const QString &id);

        };
    }
}

#endif // FILE_MANAGER_H
