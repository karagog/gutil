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

#ifndef PEOPLETABLE_H
#define PEOPLETABLE_H

#include "DataObjects/DataSet/datatable.h"
#include "persondatarow.h"
#include "gutil_macros.h"

class PeopleTable :
        public GUtil::DataTable
{
    friend class DogOwnerDataSet;

public:

    DECLARE_DATATABLE_ROW_TYPE( PersonDataRow );

    // We customize the way we init, so that by default our people have a name and
    //  identity
    PeopleTable();
    PeopleTable(const PeopleTable &o);


protected:

    virtual void init_new_row(GUtil::DataRow &r);


private:

    void _init(){

        SetTableName("People");

        // Initialize your custom column keys and labels
        SetColumnHeaders(QStringList("name") << "lastname" << "id",
                         QStringList("Name") << "Last Name" << "Identifier");

        // We set the id column as the primary key for the table
        AddKeyColumn(2);
    }

};

#endif // PEOPLETABLE_H
