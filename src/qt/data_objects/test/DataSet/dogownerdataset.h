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

#ifndef DOGOWNERDATASET_H
#define DOGOWNERDATASET_H

#include "dogtable.h"
#include "peopletable.h"
#include "DataObjects/DataSet/dataset.h"
#include "DataObjects/DataSet/sharedsetdata.h"

class DogOwnerDataSet :
        public GUtil::DataSet
{
public:

    DogOwnerDataSet(int num_tables = 0)
        :DataSet(num_tables){
        _init();
    }

    DogOwnerDataSet(const DogOwnerDataSet &o)
        :DataSet(o){
        _init();
    }


    TABLE_ACCESSOR(People, PeopleTable, 0);
    TABLE_ACCESSOR(Dog, DogTable, 1);

private:

    void _init(){
        set_data().Tables.Add(PeopleTable());
        set_data().Tables.Add(DogTable());

        // Can't add any more tables
        set_data().Tables.SetReadOnly(true);
    }
};

#endif // DOGOWNERDATASET_H
