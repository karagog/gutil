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

#include "DataObjects/datatablebase.h"

class PersonDataRow;

class PeopleTable :
        public GUtil::DataObjects::DataTableBase<PersonDataRow>
{
    friend class DogOwnerDataSet;

public:

    // We customize the way we init, so that by default our people have a name and
    //  identity
    PeopleTable()
    {
        _init();
    }

    // We don't do anything special on copy, but we have to declare it anyways
    PeopleTable(const PeopleTable &o)
        :GUtil::DataObjects::DataTableBase<PersonDataRow>(o)
    {}


protected:

    PeopleTable(const DataSet &s)
        :GUtil::DataObjects::DataTableBase<PersonDataRow>(s)
    {
        _init();
    }

    virtual void init_new_row(DataRow &r){
        r.At(2) = QUuid::createUuid();
    }


private:

    void _init(){
        SetTableName("People");

        // Initialize your custom column keys and labels
        SetColumnHeaders(QStringList("name") << "lastname" << "id",
                         QStringList("Name") << "Last Name" << "Identifier");
    }

};

#endif // PEOPLETABLE_H
