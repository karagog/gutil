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

#include "datarowcollection.h"
#include "datatable.h"
GUTIL_USING_NAMESPACE( DataObjects );

DataRowCollection::DataRowCollection(SharedTableData *td)
    :_table(new DataTable(td)){}

DataRowCollection::DataRowCollection(SharedTableData *td, const DataRowCollection &o)
    :_table(new DataTable(td))
{
    _init_cloned_rows(o);
}

DataRowCollection::DataRowCollection(const DataRowCollection &o)
    :Collection<DataRow>(),
    _table(new DataTable(o.Table()))
{
    _init_cloned_rows(o);

    SetReadOnly(true);
}

DataRowCollection::~DataRowCollection(){
    delete _table;
}

void DataRowCollection::validate_new_item(const DataRow &i)
        throw(Core::ValidationException)
{
    Table().validate_new_row(i);

    if(Table() != i.Table())
        THROW_NEW_GUTIL_EXCEPTION2(Core::ValidationException,
                                  "The row does not belong to this table.  "
                                  "If you still want to add it, then call 'ImportRow' "
                                  "on the parent table.");
    else if(Contains(i))
        THROW_NEW_GUTIL_EXCEPTION2(Core::ValidationException,
                                  "Row already exists in the collection");
}

void DataRowCollection::_init_cloned_rows(const DataRowCollection &o)
{
    // Clone each row explicitly; each row must detach itself from the shared pointer
    for(int i = 0; i < o.Count(); i++)
    {
        const DataRow &r = o[i];
        Custom::GVariantList vals;

        for(int j = 0; j < r.ColumnCount(); j++)
            vals.append(r[j]);

        Add(DataRow(Table(), vals));
    }
}
