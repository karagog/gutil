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
#include "datatablecollectionbase.h"
#include "datatablecollection.h"
#include "datarowcollectionbase.h"
#include "datarowcollection.h"
#include "datacolumncollection.h"
#include "gvariantcollection.h"
using namespace GUtil;

DataObjects::SharedSetData::SharedSetData(int num_tables)
{
    tables = new DataObjects::DataTableCollection(this);

    if(num_tables > 0)
        tables->Resize(num_tables);
}

DataObjects::SharedSetData::SharedSetData(const DataObjects::SharedSetData &o)
{
    tables = new DataObjects::DataTableCollection(o.Tables());
}

DataObjects::SharedSetData::~SharedSetData()
{
    delete tables;
}

DataObjects::DataTableCollection &DataObjects::SharedSetData::Tables() const
{
    return *tables;
}





DataObjects::SharedTableData::SharedTableData(SharedSetData *sd)
{
    rows = new DataObjects::DataRowCollection(this);
    columns = new DataObjects::DataColumnCollection;

    SetSetData(sd);
}

DataObjects::SharedTableData::SharedTableData(
        const DataObjects::SharedTableData &d)
            :QSharedData(d),
            name(d.name)
{
    rows = new DataObjects::DataRowCollection(d.Rows());
    columns = new DataObjects::DataColumnCollection(d.Columns());

    SetSetData(d.set_data);
}

DataObjects::SharedTableData::~SharedTableData()
{
    delete rows;
    delete columns;
}

DataObjects::SharedSetData *DataObjects::SharedTableData::SetData() const
{
    return set_data;
}

void DataObjects::SharedTableData::SetSetData(SharedSetData *sd)
{
    set_data = sd;
}

DataObjects::DataRowCollection &DataObjects::SharedTableData::Rows() const
{
    return *rows;
}

DataObjects::DataColumnCollection &DataObjects::SharedTableData::Columns() const
{
    return *columns;
}







DataObjects::SharedRowData::SharedRowData(const DataObjects::DataTable &t,
                                          const Custom::GVariantList &vals)
{
    tuple = new DataObjects::GVariantCollection(vals);
    SetTableData(&t.table_data());
}

DataObjects::SharedRowData::SharedRowData(const DataObjects::SharedRowData &o)
    :QSharedData(o),
    tuple(o.tuple)
{
    tuple = new DataObjects::GVariantCollection(o.Tuple());
    SetTableData(o.table_data);
}

DataObjects::SharedRowData::~SharedRowData()
{
    delete tuple;
}

DataObjects::SharedTableData *DataObjects::SharedRowData::TableData() const
{
    return table_data;
}

void DataObjects::SharedRowData::SetTableData(SharedTableData *dt)
{
    table_data = dt;
    tuple->Resize(dt->Columns().Count());
}

DataObjects::GVariantCollection &DataObjects::SharedRowData::Tuple() const
{
    return *tuple;
}
