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

DataObjects::DataTableCollectionBase::DataTableCollectionBase(DataObjects::SharedSetData *d)
{
    _set_data = d;
}

DataObjects::DataTableCollectionBase::DataTableCollectionBase(const DataTableCollectionBase &o)
{
    o.CloneTo(*this);
}

DataObjects::DataTableCollectionBase &DataObjects::DataTableCollectionBase::CloneTo(
        DataTableCollectionBase &o) const
{
    o.Resize(Size());

    for(int i = 0; i < Size(); i++)
        At(i).CloneTo(o[i]);

    return o;
}

void DataObjects::DataTableCollectionBase::on_add_dereferenced(DataObjects::DataTable &t) const
{
    // Remove ourselves from our parent set
    SharedSetData *sd;
    if((sd = t.table_data().SetData()))
        sd->Tables().RemoveOne(t);

    // Set our new set data to this one
    t.table_data().SetSetData(_set_data);
}

void DataObjects::DataTableCollectionBase::validate_new_item_dereferenced(const DataTable &t) const
        throw(Core::ValidationException)
{
    if(Contains(t))
        THROW_NEW_GUTIL_EXCEPTION(Core::ValidationException,
                                  "Table already exists in data set");
}
