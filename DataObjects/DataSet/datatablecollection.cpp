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
#include "datatablecollection.h"
#include "dataset.h"
#include "sharedtabledata.h"
#include "sharedsetdata.h"
using namespace GUtil;

DataObjects::DataTableCollection::DataTableCollection(DataObjects::SharedSetData *d, int size)
    :ResizableCollection<DataTable>(size),
    _dataset(new DataSet(d))
{}

DataObjects::DataTableCollection::DataTableCollection(const DataTableCollection &o)
    :ResizableCollection<DataTable>(),
    Core::Interfaces::IClonable<DataTableCollection>()
{
    o.CloneTo(*this);
}

DataObjects::DataTableCollection::~DataTableCollection(){
    delete _dataset;
}

const DataObjects::DataSet &DataObjects::DataTableCollection::Set() const{
    return *_dataset;
}

DataObjects::DataSet &DataObjects::DataTableCollection::Set(){
    return *_dataset;
}


void DataObjects::DataTableCollection::on_add(DataObjects::DataTable *t)
{
    DataSet &ds = t->table_data().GetDataSet();
    if(Set() != ds)
    {
        // Remove the table from its parent set
        if(!ds.IsNull())
            ds.Tables().RemoveOne(*t);

        // Set our new set data to this one
        t->table_data().SetDataSet(*_dataset);
    }
}

void DataObjects::DataTableCollection::validate_new_item(const DataObjects::DataTable &t)
        throw(Core::ValidationException)
{
    Set().validate_new_table(t);
}

DataObjects::DataTableCollection &DataObjects::DataTableCollection::CloneTo(
        DataObjects::DataTableCollection &o) const
{
    o.Clear();

    for(int i = 0; i < Collection<DataTable>::Size(); i++)
        o.Add(Collection<DataTable>::At(i).Clone());

    return o;
}
