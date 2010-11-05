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
using namespace GUtil;

DataObjects::DataRow::DataRow(DataObjects::DataTable *dt, int len)
    :Interfaces::ICollection<QVariant>(len)
{
    table = dt;
}

DataObjects::DataRow::~DataRow(){}

DataObjects::DataRow::DataRow(const DataRow &o)
{
    *this = o;
}

DataObjects::DataRow &DataObjects::DataRow::operator =(const DataObjects::DataRow &o)
{
    table = o.table;

    ClearValues();
    for(int i = 0; i < o.Size(); i++)
        SetValue(i, o.Value(i));

    return *this;
}

DataObjects::DataTable &DataObjects::DataRow::Table()
{
    return *table;
}

int DataObjects::DataRow::Index() const
{
    return row_index;
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
    DataRow *dr = (DataRow *)v;
    dr->table = _table;
    dr->row_index = index;
}
