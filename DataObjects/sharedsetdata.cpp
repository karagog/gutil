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

#include "sharedtabledata.h"
#include "dataset.h"
#include "datatablebase.h"
#include "datatablecollectionbase.h"
#include "datatablecollection.h"
#include "datarowcollectionbase.h"
#include "datarowcollection.h"
#include "datacolumncollection.h"
#include "gvariantcollection.h"
using namespace GUtil;

DataObjects::SharedSetData::SharedSetData(int num_tables)
{
    tables = new DataObjects::DataTableCollection(this);

    if(num_tables > 0)
        tables->Resize(num_tables);
}

DataObjects::SharedSetData::SharedSetData(const DataObjects::SharedSetData &o)
{
    tables = new DataObjects::DataTableCollection(o.Tables());
}

DataObjects::SharedSetData::~SharedSetData()
{
    delete tables;
}

DataObjects::DataTableCollection &DataObjects::SharedSetData::Tables() const
{
    return *tables;
}








