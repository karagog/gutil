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

#include "dogtable.h"
#include "DataObjects/DataSet/dataset.h"
USING_NAMESPACE_GUTIL1(QT);

DogTable::DogTable()
{
    _init_dog_table();
}

DogTable::DogTable(const DogTable &o)
    :GUtil::DataTable(o)
{}

void DogTable::_init_dog_table()
{
    SetTableName("Dogs");

    // Initialize your custom column keys and labels
    SetColumnHeaders(QStringList("name") << "nickname" << "breed" << "owner_id",
                     QStringList("Name") << "NickName" << "Breed" << "Owner Id");
}
