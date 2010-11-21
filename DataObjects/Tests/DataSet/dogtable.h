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

#include "DataObjects/datatablebase.h"

class DogRow;

class DogTable :
        public GUtil::DataObjects::DataTableBase<DogRow>
{
public:

    // We customize the way we init, so that by default our people have a name and
    //  identity
    DogTable()
        :GUtil::DataObjects::DataTableBase<DogRow>("Dogs", 2)
    {
        // Initialize your custom column keys and labels
        SetColumnHeaders(QStringList("name") << "nickname" << "breed",
                         QStringList("Name") << "NickName" << "Breed");
    }

    // We don't do anything special on copy, but we have to declare it anyways
    DogTable(const DogTable &o)
        :GUtil::DataObjects::DataTableBase<DogRow>(o)
    {}
};

#endif // DOGSTABLE_H
