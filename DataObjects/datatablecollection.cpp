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

#include "dataset.h"
#include "datatablecollection.h"
using namespace GUtil;

DataObjects::DataTableCollection::DataTableCollection(DataObjects::SharedSetData *d)
{
    _set_data = d;
}

DataObjects::DataTableCollection::DataTableCollection(const DataTableCollection &o)
{
    o.CloneTo(*this);
}

DataObjects::DataTableCollection &DataObjects::DataTableCollection::CloneTo(
        DataTableCollection &o) const
{
    o.Resize(Size());

    for(int i = 0; i < Size(); i++)
        Value(i).CloneTo(o[i]);

    return o;
}

void DataObjects::DataTableCollection::on_add(DataObjects::DataTable &t) const
{
    t.table_data().SetSetData(_set_data);
}

DataObjects::DataTable DataObjects::DataTableCollection::create_blank_item() const
{
    return DataTable(DataSet(_set_data));
}

void DataObjects::DataTableCollection::validate_new_item(const DataTable &t) const
        throw(Core::ValidationException)
{
    if(Contains(t))
        THROW_NEW_GUTIL_EXCEPTION(Core::ValidationException,
                                  "Table already exists in data set");
}
