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

#ifndef DOGSTABLE_H
#define DOGSTABLE_H

#include "DataObjects/DataSet/datatablebase.h"

#define NUM_DOG_COLUMNS 4

class DogRow;

class DogTable :
        public GUtil::DataObjects::DataTableBase<DogRow>
{
    friend class DogOwnerDataSet;

public:

    // We customize the way we init, so that by default our people have a name and
    //  identity
    DogTable()
        :GUtil::DataObjects::DataTableBase<DogRow>("Dogs", NUM_DOG_COLUMNS)
    {
        _init();
    }

    // We don't do anything special on copy, but we have to declare it anyways
    DogTable(const DogTable &o)
        :GUtil::DataObjects::DataTableBase<DogRow>(o)
    {}


protected:

    DogTable(const DataSet &o)
        :GUtil::DataObjects::DataTableBase<DogRow>(o, "Dogs", NUM_DOG_COLUMNS)
    {
        _init();
    }


private:
    void _init(){
        // Initialize your custom column keys and labels
        SetColumnHeaders(QStringList("name") << "nickname" << "breed" << "owner_id",
                         QStringList("Name") << "NickName" << "Breed" << "Owner Id");
    }
};

#endif // DOGSTABLE_H
