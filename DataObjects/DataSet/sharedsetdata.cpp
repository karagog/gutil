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

#include "sharedsetdata.h"
#include "sharedtabledata.h"
#include "dataset.h"
#include "datatable.h"
#include "datatablecollection.h"
#include "datarowcollection.h"
#include "datarowcollection.h"
#include "datacolumncollection.h"
#include "DataObjects/gvariantcollection.h"
using namespace GUtil;

DataObjects::SharedSetData::SharedSetData(int num_tables)
    :_tables(new DataTableCollection(this))
{
    if(num_tables > 0)
        _tables->Resize(num_tables);
}

DataObjects::SharedSetData::SharedSetData(const DataObjects::SharedSetData &o)
    :GSharedData(o),
      _tables(new DataTableCollection(o.Tables()))
{}

DataObjects::SharedSetData::~SharedSetData()
{
    delete _tables;
}

DataObjects::DataTableCollection &DataObjects::SharedSetData::Tables()
{
    return *_tables;
}

const DataObjects::DataTableCollection &DataObjects::SharedSetData::Tables() const
{
    return *_tables;
}
