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

#include "datarow.h"
#include "datatable.h"
using namespace GUtil;

DataObjects::DataRow::DataRow()
{
    _init_data_row(0);
}

DataObjects::DataRow::DataRow(DataObjects::DataTable *dt)
{
    _init_data_row(dt);
}

DataObjects::DataRow::DataRow(const DataRow &o)
{
    *this = o;
}

DataObjects::DataRow::~DataRow(){}

void DataObjects::DataRow::_init_data_row(DataTable *dt)
{
    table = dt;

    if(table != 0)
        Resize(table->columnCount());
}

DataObjects::DataRow &DataObjects::DataRow::operator =(const DataObjects::DataRow &o)
{
    table->removeRow(row_index);
    table = o.table;

    ClearValues();

    for(int i = 0; i < o.ColumnCount(); i++)
        SetValue(i, o.Value(i));

    return *this;
}

DataObjects::DataTable &DataObjects::DataRow::Table()
{
    return *table;
}

QVariant &DataObjects::DataRow::At(int index)
{
    return (*this)[index];
}

int DataObjects::DataRow::Index() const
{
    return row_index;
}

int DataObjects::DataRow::ColumnCount() const
{
    return table->ColumnCount();
}

void DataObjects::DataRow::WriteXml(QXmlStreamWriter &) const
{

}

void DataObjects::DataRow::ReadXml(QXmlStreamReader &)
        throw(Core::XmlException)
{

}





DataObjects::DataRowCollection::DataRowCollection(DataTable *dt)
{
    _table = dt;
}

DataObjects::DataRowCollection::~DataRowCollection(){}

void DataObjects::DataRowCollection::onAdd(void *v, int index)
{
    DataRow *dr = *((DataRow **)v);
    if(dr == 0)
        dr = new DataObjects::DataRow(_table);

    dr->table = _table;
    dr->row_index = index;
}
