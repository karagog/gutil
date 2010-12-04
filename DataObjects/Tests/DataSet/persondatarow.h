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

#ifndef PERSONDATAROW_H
#define PERSONDATAROW_H

#include "DataObjects/DataSet/datarow.h"
#include "Custom/gvariant.h"
#include <QUuid>

class PeopleTable;

class PersonDataRow :
        public GUtil::DataObjects::DataRow
{
    //template<class T> friend class GUtil::DataObjects::DataRowCollection;
    //template<class T> friend class GUtil::DataObjects::DataTableBase;

public:

    PersonDataRow(const PersonDataRow &o, bool clone = false)
        :DataRow(o, clone)
    {}


    // So we return the correct type of table
    DECLARE_PARENT_TABLE_TYPE( PeopleTable );


    // With these convenient macros we declare strongly-typed data accessors
    //  Note: You can also use an integer to index the column instead of a string.
    //   The performance would be slightly better, but with a string you don't have
    //   to worry about the order of the columns.
    ROW_PROPERTY(Name, QString, "name");
    ROW_PROPERTY(LastName, QString, "lastname");
    READONLY_ROW_PROPERTY(Id, QUuid, "id");

};

#endif // PERSONDATAROW_H
