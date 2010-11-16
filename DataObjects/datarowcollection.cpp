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

#include "datatable.h"
#include "datarowcollection.h"
using namespace GUtil;

DataObjects::DataRowCollection::DataRowCollection(SharedTableData *td)
{
    _table_data = td;
}

DataObjects::DataRowCollection::DataRowCollection(const DataObjects::DataRowCollection &o)
{
    o.CloneTo(*this);
}

DataObjects::DataRowCollection &DataObjects::DataRowCollection::CloneTo(
        DataObjects::DataRowCollection &o) const
{
    o._table_data = _table_data;
    o.Resize(Size());

    // Clone each row explicitly; each row must detach itself from the shared pointer
    for(int i = 0; i < Count(); i++)
        At(i).CloneTo(o[i]);

    return o;
}

DataObjects::DataRow *DataObjects::DataRowCollection::create_blank_item()
{
    return new DataRow( DataTable(_table_data) );
}

void DataObjects::DataRowCollection::validate_new_item(const DataObjects::DataRow *const r) const
        throw(Core::ValidationException)
{
    if(Table() != r->Table())
        THROW_NEW_GUTIL_EXCEPTION(Core::ValidationException,
                                  "The row does not belong to this table.  "
                                  "If you still want to add it, then call 'ImportRow' "
                                  "on the parent table.");
    else if(Contains(*r))
        THROW_NEW_GUTIL_EXCEPTION(Core::ValidationException,
                                  "Row already exists in the table");
}

DataObjects::DataTable DataObjects::DataRowCollection::Table() const
{
    return DataTable(_table_data);
}

DataObjects::DataRow &DataObjects::DataRowCollection::At(int index) const
{
    return *(this->value_protected(index));
}

DataObjects::DataRow &DataObjects::DataRowCollection::Add(const DataObjects::DataRow &r)
{
    return *add_protected(new DataRow(r));
}

DataObjects::DataRow &DataObjects::DataRowCollection::Insert(const DataObjects::DataRow &r, int index)
{
    return *insert_protected(new DataRow(r), index);
}

int DataObjects::DataRowCollection::Count() const
{
    return count_protected();
}

int DataObjects::DataRowCollection::Size() const
{
    return size_protected();
}

int DataObjects::DataRowCollection::IndexOf(const DataRow &r) const
{
    DataRow r2(r);
    return indexOf_protected(&r2);
}

bool DataObjects::DataRowCollection::Contains(const DataRow &r) const
{
    DataRow r2(r);
    return contains_protected(&r2);
}

void DataObjects::DataRowCollection::Remove(int ind)
{
    remove_protected(ind);
}

void DataObjects::DataRowCollection::RemoveOne(const DataRow &r)
{
    DataRow r2(r);
    removeOne_protected(&r2);
}

void DataObjects::DataRowCollection::RemoveAll(const DataRow &r)
{
    DataRow r2(r);
    removeAll_protected(&r2);
}

void DataObjects::DataRowCollection::Clear()
{
    clear_protected();
}

void DataObjects::DataRowCollection::Resize(int sz)
{
    resize_protected(sz);
}

DataObjects::DataRow &DataObjects::DataRowCollection::operator [](int index)
{
    return *value_protected(index);
}

const DataObjects::DataRow &DataObjects::DataRowCollection::operator [](int index) const
{
    return *value_protected(index);
}

void DataObjects::DataRowCollection::on_remove(DataObjects::DataRow **r) const
{
    delete *r;
}

bool DataObjects::DataRowCollection::compare_equality(const DataRow *const lhs,
                                                      const DataRow *const rhs) const
{
    return *lhs == *rhs;
}
