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
#include "datatablebase.h"
#include "sharedsetdata.h"
using namespace GUtil;

DataObjects::DataTableCollection::DataTableCollection(DataObjects::SharedSetData *d, int size)
    :ResizableCollection<DataTable>(size),
    _data_set(new DataSet(d)){}

DataObjects::DataTableCollection::~DataTableCollection(){
    delete _data_set;
}

const DataObjects::DataSet &DataObjects::DataTableCollection::Set() const{
    return *_data_set;
}

DataObjects::DataSet &DataObjects::DataTableCollection::Set(){
    return *_data_set;
}


void DataObjects::DataTableCollection::on_add(DataObjects::DataTable *t) const
{
    SharedSetData *sd = t->table_data().SetData();
    if(Set() != DataSet(sd))
    {
        // Remove ourselves from our parent set
        if(sd != 0)
            sd->Tables().RemoveOne(*t);

        // Set our new set data to this one
        t->table_data().SetSetData(&_data_set->set_data());
    }
}

void DataObjects::DataTableCollection::validate_new_item(const DataObjects::DataTable &t) const
        throw(Core::ValidationException)
{
    if(Contains(t))
        THROW_NEW_GUTIL_EXCEPTION(Core::ValidationException,
                                  "Table already exists in data set");
}

DataObjects::DataTableCollection &DataObjects::DataTableCollection::CloneTo(
        DataObjects::DataTableCollection &o) const
{
    o.Clear();

    for(int i = 0; i < Collection<DataTable>::Size(); i++)
        o.Add(Collection<DataTable>::At(i).Clone());

    return o;
}
