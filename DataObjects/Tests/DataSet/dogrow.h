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

#ifndef DOGROW_H
#define DOGROW_H

#include "DataObjects/datarow.h"

class DogRow;
class DogTable;

typedef DataRowCollectionBase<DogRow> DogRowCollection;


// This is the simple version of a derived data row; one in which the row data
//  has nothing special to it, just property accessors for the columns
class DogRow :
        public GUtil::DataObjects::DataRow
{
    template<class T> friend class GUtil::DataObjects::DataRowCollectionBase;
    template<class T> friend class GUtil::DataObjects::DataTableBase;

public:

    DogRow(const DataRow &o)
        :DataRow(o)
    {}

    DECLARE_PARENT_TABLE_TYPE( DogTable );

    // These properties access the typed data
    ROW_PROPERTY(Name, QString, "name");
    ROW_PROPERTY(NickName, QString, "nickname");
    ROW_PROPERTY(Breed, QString, "breed");
    ROW_PROPERTY(OwnerId, QUuid, "owner_id");


protected:

    DogRow(const DataTableBase<DogRow> &tbl,
           const GUtil::Custom::GVariantList &vals = GUtil::Custom::GVariantList())

            // We pass in our own derivation of the shared data class
        :DataRow(tbl, vals)
    {}

};

#endif // DOGROW_H
