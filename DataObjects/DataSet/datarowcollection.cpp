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

DataRowCollection::DataRowCollection(const DataTable &t)
    :_table(new DataTable(t)){}

DataRowCollection::~DataRowCollection(){
    delete _table;
}

DataRowCollection &DataRowCollection::CloneTo(DataRowCollection &o) const
{
    if(o._table)
        delete o._table;
    o._table = new DataTable(Table());

    o.Clear();

    // Clone each row explicitly; each row must detach itself from the shared pointer
    for(int i = 0; i < Collection<DataRow>::Count(); i++)
    {
        DataRow dr(Table());
        Collection<DataRow>::At(i).CloneTo(dr);
        o.Add(dr);
    }

    return o;
}

void DataRowCollection::validate_new_item(const DataRow &i)
        throw(Core::ValidationException)
{
    Table().validate_new_row(i);
}