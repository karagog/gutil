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
#include "datacolumncollection.h"
#include "qvariantcollection.h"
using namespace GUtil;

DataObjects::SharedSetData::SharedSetData()
{
    _tables = new DataObjects::DataTableCollection(this);
}

DataObjects::SharedSetData::SharedSetData(const DataObjects::SharedSetData &o)
{
    _tables = new DataObjects::DataTableCollection(o.Tables());
}

DataObjects::SharedSetData::~SharedSetData()
{
    delete _tables;
}

DataObjects::DataTableCollection &DataObjects::SharedSetData::Tables() const
{
    return *_tables;
}





DataObjects::SharedTableData::SharedTableData(SharedSetData *sd)
{
    _rows = new DataObjects::DataRowCollection(this);
    _columns = new DataObjects::DataColumnCollection;

    SetSetData(sd);
}

DataObjects::SharedTableData::SharedTableData(
        const DataObjects::SharedTableData &d)
            :QSharedData(d),
            name(d.name)
{
    _rows = new DataObjects::DataRowCollection(d.Rows());
    _columns = new DataObjects::DataColumnCollection(d.Columns());

    SetSetData(d._set_data);
}

DataObjects::SharedTableData::~SharedTableData()
{
    delete _rows;
    delete _columns;
}

DataObjects::SharedSetData *DataObjects::SharedTableData::SetData() const
{
    return _set_data;
}

void DataObjects::SharedTableData::SetSetData(SharedSetData *sd)
{
    _set_data = sd;
}

DataObjects::DataRowCollection &DataObjects::SharedTableData::Rows() const
{
    return *_rows;
}

DataObjects::DataColumnCollection &DataObjects::SharedTableData::Columns() const
{
    return *_columns;
}







DataObjects::SharedRowData::SharedRowData(DataObjects::SharedTableData *td,
                                       const QVariantList &vals)
{
    _tuple = new DataObjects::QVariantCollection(vals);
    SetTableData(td);
}

DataObjects::SharedRowData::SharedRowData(const DataObjects::SharedRowData &o)
    :QSharedData(o),
    _tuple(o._tuple)
{
    _tuple = new DataObjects::QVariantCollection(o.Tuple());
    SetTableData(o._table_data);
}

DataObjects::SharedRowData::~SharedRowData()
{
    delete _tuple;
}

DataObjects::SharedTableData *DataObjects::SharedRowData::TableData() const
{
    return _table_data;
}

void DataObjects::SharedRowData::SetTableData(SharedTableData *dt)
{
    _table_data = dt;
    _tuple->Resize(dt->Columns().Count());
}

DataObjects::QVariantCollection &DataObjects::SharedRowData::Tuple() const
{
    return *_tuple;
}
