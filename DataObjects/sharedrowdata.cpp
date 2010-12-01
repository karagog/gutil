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
            _tuple(new DataObjects::ObservableGVariantCollection<SharedRowData>(t.ColumnCount()))
{
    for(int i = 0; i < vals.count() && i < Table().ColumnCount(); i++)
        _tuple->At(i) = vals[i];

    _tuple->SetValueChangedFunction(this, &SharedRowData::_value_changed);
}

DataObjects::SharedRowData::SharedRowData(const DataObjects::SharedRowData &o)
        :Custom::GSharedData(o),
        _table(new DataObjects::DataTable(o.Table())),
        _tuple(new DataObjects::ObservableGVariantCollection<SharedRowData>(o.Tuple()))
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

DataObjects::ObservableGVariantCollection<SharedRowData> &DataObjects::SharedRowData::Tuple(){
    return *_tuple;
}

const DataObjects::ObservableGVariantCollection<SharedRowData> &DataObjects::SharedRowData::Tuple() const{
    return *_tuple;
}

void DataObjects::SharedRowData::
        _value_changed(const Custom::GVariant &orig, const Custom::GVariant &newval)
{
    if(orig != newval)
    {
        if(!IsDirty())
            SetDirty(true);
    }
}

void DataObjects::SharedRowData::on_set_dirty(bool d)
{
    Table().table_data().SetDirty(d);
}
