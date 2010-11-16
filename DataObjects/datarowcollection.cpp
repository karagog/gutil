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
        Value(i).CloneTo(o[i]);

    return o;
}

DataObjects::DataRow DataObjects::DataRowCollection::create_blank_item()
{
    return DataRow( DataTable(_table_data) );
}

void DataObjects::DataRowCollection::validate_new_item(const DataObjects::DataRow &r) const
        throw(Core::ValidationException)
{
    if(Table() != r.Table())
        THROW_NEW_GUTIL_EXCEPTION(Core::ValidationException,
                                  "The row does not belong to this table.  "
                                  "If you still want to add it, then call 'ImportRow' "
                                  "on the parent table.");
    else if(Contains(r))
        THROW_NEW_GUTIL_EXCEPTION(Core::ValidationException,
                                  "Row already exists in the table");
}

DataObjects::DataTable DataObjects::DataRowCollection::Table() const
{
    return DataTable(_table_data);
}
