/*Copyright 2010-2013 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_BINARYDATASTORE_H
#define GUTIL_BINARYDATASTORE_H

#ifndef GUTIL_NO_DATABASE_FUNCTIONALITY

#include "gutil_vector.h"
#include "gutil_pair.h"
#include <QString>
#include <QUuid>

NAMESPACE_GUTIL2(QT, BusinessObjects);


/** Use this class to manage binary data without consuming lots of memory.

    It implements an on-disk cache of the data, so it doesn't sit wastefully
    in memory.  This is useful for storing and accessing lots of data without
    consuming lots of memory.

    The functions will use exceptions to tell you when something goes wrong.  Beware.
*/
class BinaryDataStore
{
    QString m_fileName;
    QString m_connString;
public:

    /** Constructs an empty BinaryDataStore.  You must call Initialize() before using. */
    BinaryDataStore();

    /** Deconstructs the BDS. */
    ~BinaryDataStore();

    /** Initializes a BinaryDataStore.
     *
     *  Throws an exception with a helpful message if something goes wrong.
     *
     *  \param filename The filename of the database file accessed by this instance.
     *  If the file does not exist it is created by this object.
    */
    void Initialize(const QString &filename);

    /** Uninitializes the BDS.  You must call Initialize again after calling this. */
    void Uninitialize();

    /** Returns true if the BDS has been initialized. */
    inline bool IsInitialized() const{ return !m_connString.isEmpty(); }

    /** Removes all values from the database. */
    void Clear();

    /** Returns the file location of the data cache. */
    inline const QString &GetFileName() const{ return m_fileName; }
    
    /** Returns the number of pieces of data this object is caching. */
    int CountData() const;


    /** Add a file to storage.  Adds the byte array to the on-disk cache.
     *
        \returns A pair of data describing the new cache.
        The integer is the id assigned to the data cache.  You will use this
        id to access the data via GetData().
        The QUuid is the version id of the data.  This is how you will know if
        the value ever changes from behind your back.  Other instances will update
        the version number and you can detect this change.

        Throws an exception if something fails.
    */
    ::GUtil::DataObjects::Pair<int, QUuid> AddData(const QByteArray &);


    /** Remove a file from storage. The size of the cache file will shrink about the size of the file.
        \param id The id of the data you want to remove.
        \sa AddData()
    */
    void RemoveData(int id);


    /** Retrieve data from the cache.

        \param id The id of the data you want to retrieve.  This is the same Id
        returned by AddData() when you added the data to the cache.

        \returns The data assigned to the given id.  If the Id is not present in
        the cache, then a null array is returned.
    */
    QByteArray GetData(int id) const;

    /** Sets the data at the given id.  Nothing happens if the id doesn't exist.
     *  \returns The new version id for the data.  You can store this value and
     *  compare with future values to know if the data changed.
    */
    QUuid SetData(int id, const QByteArray &data);


    /** Returns whether the cache contains data with that id.  */
    inline bool HasData(int id) const{ return -1 != GetSize(id); }


    /** Get the size (number of bytes) of a cached piece of data.
        \param id The id of the data you want to query.
        \returns The size (in bytes) of the data, or -1 if the id is not found.
    */
    int GetSize(int id) const;

    /** Returns the version id of the given data referenced by id.
     *  If the id wasn't found, then a null QUuid is returned.
    */
    QUuid GetVersion(int id) const;

    /** Returns the list of ids contained by the database cache, paired with
     *  that item's version id.
    */
    GUtil::DataObjects::Vector< GUtil::DataObjects::Pair<int, QUuid> > GetIds() const;

};


END_NAMESPACE_GUTIL2;

#endif // DATABASE_FUNCTIONALITY

#endif // GUTIL_BINARYDATASTORE_H
