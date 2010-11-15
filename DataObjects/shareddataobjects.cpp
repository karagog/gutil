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

#include "shareddataobjects.h"
#include "datatablecollection.h"
#include "datarowcollection.h"
using namespace GUtil;

DataObjects::SharedSetData::SharedSetData(DataSet *ds)
{
    tables = new DataObjects::DataTableCollection(ds);
}

DataObjects::SharedSetData::SharedSetData(const DataObjects::SharedSetData &o)
{
    tables = new DataObjects::DataTableCollection(o.tables);
}

DataObjects::SharedSetData::~SharedSetData()
{
    delete tables;
}





DataObjects::SharedTableData::SharedTableData(DataObjects::DataTable *t)
    :QSharedData()
{
    rows = new DataObjects::DataRowCollection(t);
}

DataObjects::SharedTableData::SharedTableData(
        const DataObjects::SharedTableData &d)
            :QSharedData(d),
            keys(d.keys),
            labels(d.labels),
            name(d.name)
{
    dataset = d.dataset;
    rows = new DataObjects::DataRowCollection(d.rows);
}

DataObjects::SharedTableData::~SharedTableData()
{
    delete rows;
}







DataObjects::SharedRowData::SharedRowData(DataObjects::DataTable *t,
                                       const QVariantList &vals)
                                           :tuple(vals)
{
    SetTable(t);
}

DataObjects::SharedRowData::SharedRowData(const DataObjects::SharedRowData &o)
    :QSharedData(o),
    tuple(o.tuple)
{
    SetTable(o._table);
}

DataObjects::DataTable *DataObjects::SharedRowData::Table() const
{
    return _table;
}

void DataObjects::SharedRowData::SetTable(DataTable *dt)
{
    if((_table = dt))
        tuple.Resize(dt->ColumnCount());
}

