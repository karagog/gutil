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

#include "sharedrowdata.h"
#include "datatable.h"
#include "DataObjects/updatablegvariantcollection.h"
using namespace GUtil;

DataObjects::SharedRowData::SharedRowData(const DataObjects::DataTable &t,
                                          const Custom::GVariantList &vals)
            :UpdatableGVariantCollection(t.ColumnCount(), vals),
            _table(new DataObjects::DataTable(t))
{}

DataObjects::SharedRowData::SharedRowData(const DataObjects::SharedRowData &o)
        :GSharedData(o),
        UpdatableGVariantCollection(o),
        _table(new DataObjects::DataTable(o.Table()))
{}

DataObjects::SharedRowData::~SharedRowData(){
    delete _table;
}

void DataObjects::SharedRowData::on_value_about_to_change(int index, const Custom::GVariant &newvalue)
{
    DataRow(this).row_value_about_to_change(index, newvalue);
}
