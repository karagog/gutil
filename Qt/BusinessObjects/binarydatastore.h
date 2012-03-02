/*Copyright 2010-2012 George Karagoulis

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

#ifndef GUTIL_NO_GUI_FUNCTIONALITY
#ifndef GUTIL_NO_DATABASE_FUNCTIONALITY

#include "gutil_databaseiodevice.h"
#include "gutil_set.h"
#include <QString>

namespace GUtil{ namespace QT{ namespace BusinessObjects{


/** Use this class to manage binary data without consuming lots of memory.

    It implements an on-disk cache of the data, so it doesn't sit wastefully
    in memory.  This is useful for storing and accessing lots of files without
    consuming lots of memory.
*/
class BinaryDataStore
{
public:

    /** Constructs a BinaryDataStore.

        \param identifier A unique identifier for the cache file.  If you have several
        instances in an application you will have to give them different identifiers.
    */
    explicit BinaryDataStore(const QString &identifier);


    /** Prepares the binary data store backend for storage. */
    void Initialize();


    /** Removes all values from the database and deletes the cache.

        You must call Initialize again if you want to continue using the instance.
        \sa Reset()
    */
    void Clear();


    /** Clear away all files and re-initialize the cache.
        \sa Clear()
    */
    inline void Reset(){
        Clear();
        Initialize();
    }


    /** Add a file to storage.  Adds the byte array to the on-disk cache.
        \param data The data you want to store.
        \param id The id you want to assign the data (will be automatically assigned otherwise)
        \returns The id that the data actually got.  You can use this as a handle to retrieve the data via GetFile().
        \sa HasFile(), GetFile(), RemoveFile(), GetFreeId()
    */
    int AddFile(const QByteArray &data, int id = -1);


    /** Remove a file from storage. The size of the cache file will shrink about the size of the file.
        \param id The id of the data you want to remove.
        \sa AddFile()
    */
    void RemoveFile(int id);


    /** Retrieve data from the cache.
        \param id The id of the data you want to retrieve.
    */
    QByteArray GetFile(int);


    /** Returns whether the cache contains data with that id.  */
    inline bool HasFile(int id) const{
        return _ids.Contains(id);
    }


    /** Returns an id that can be passed to AddFile() and not overwrite an existing piece of data. */
    int GetFreeId();


    /** Get the size (number of bytes) of a file. */
    int GetSize(int id);


    /** Returns the list of ids contained by this instance. */
    inline GUtil::DataObjects::Set<int> const &GetIds() const { return _ids; }


    /** Returns the absolute file location of the data cache. */
    inline QString FileName() const{
        return _file_location;
    }


private:

    Utils::SmartPointer<DataAccess::DatabaseIODevice> dbio;
    QString _file_location;

    GUtil::DataObjects::Set<int> _ids;
    int _max_id;

    static QString _get_file_loc(const QString &id);

};


}}}

#endif // DATABASE_FUNCTIONALITY
#endif // GUI_FUNCTIONALITY

#endif // FILE_MANAGER_H
