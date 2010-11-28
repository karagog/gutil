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
#include "datatablebase.h"
using namespace GUtil;

DataObjects::SharedRowData::SharedRowData(const DataObjects::DataTable &t,
                                          const Custom::GVariantList &vals)
            :_table(new DataObjects::DataTable(t)),
            _tuple(new DataObjects::GVariantCollection(vals))
{
    _tuple->Resize(_table->ColumnCount());
}

DataObjects::SharedRowData::SharedRowData(const DataObjects::SharedRowData &o)
        :Custom::GSharedData(o),
        _table(new DataObjects::DataTable(o.Table())),
        _tuple(new DataObjects::GVariantCollection(o.Tuple()))
{}

DataObjects::SharedRowData::~SharedRowData(){
    delete _table;
    delete _tuple;
}


DataObjects::DataTable &DataObjects::SharedRowData::Table(){
    return *_table;
}

const DataObjects::DataTable &DataObjects::SharedRowData::Table() const{
    return *_table;
}

DataObjects::GVariantCollection &DataObjects::SharedRowData::Tuple(){
    return *_tuple;
}

const DataObjects::GVariantCollection &DataObjects::SharedRowData::Tuple() const{
    return *_tuple;
}

bool DataObjects::SharedRowData::operator ==(const SharedRowData &o) const
{
    return this == &o;
}

bool DataObjects::SharedRowData::operator !=(const SharedRowData &o) const
{
    return !(*this == o);
}
