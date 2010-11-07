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
#include "Custom/gsemaphore.h"
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
    row_data = new RowData();

    set_table(dt);
}

DataObjects::DataRow &DataObjects::DataRow::operator =(const DataObjects::DataRow &o)
{
    row_data = o.row_data;
    return *this;
}

bool DataObjects::DataRow::operator ==(const DataObjects::DataRow &o) const
{
    return row_data->identifier == o.row_data->identifier;
}

bool DataObjects::DataRow::operator !=(const DataRow &o) const
{
    return !(*this == o);
}

DataObjects::DataRow DataObjects::DataRow::Clone() const
{
    DataObjects::DataRow ret(*this);
    ret.row_data.detach();
    return ret;
}

QVariant &DataObjects::DataRow::operator [](int index)
{
    if(index < 0 || index >= row_data->tuple.Count())
        THROW_NEW_GUTIL_EXCEPTION( Core::IndexOutOfRangeException,
                QString("Tried index %1 of %2")
                .arg(index).arg(row_data->tuple.Count()).toStdString() )

    return row_data->tuple[index];
}

QVariant &DataObjects::DataRow::operator [](const QString &column_header)
{
    int index = Table().GetColumnIndex(column_header);
    if(index == -1)
        THROW_NEW_GUTIL_EXCEPTION( Core::IndexOutOfRangeException,
                QString("Column not found: '%1'").arg(column_header).toStdString() )
    return (*this)[index];
}

DataObjects::DataTable &DataObjects::DataRow::Table()
{
    if(row_data->table == 0)
        THROW_NEW_GUTIL_EXCEPTION( Core::NullReferenceException, "Table not set" )

    return *row_data->table;
}

int DataObjects::DataRow::Index() const
{
    if(row_data->table == 0)
        return -1;

    return row_data->table->Rows().find_row_by_id(row_data->identifier);
}

int DataObjects::DataRow::ColumnCount() const
{
    if(row_data->table == 0)
        return 0;

    return row_data->table->ColumnCount();
}

void DataObjects::DataRow::set_table(DataObjects::DataTable *t)
{
    row_data->table = t;

    if(t)
        set_number_of_columns(t->ColumnCount());
}

void DataObjects::DataRow::set_number_of_columns(int cols)
{
    row_data->tuple.Resize(cols);
}





void DataObjects::DataRow::WriteXml(QXmlStreamWriter &sw) const
{
    row_data->tuple.WriteXml(sw);
}

void DataObjects::DataRow::ReadXml(QXmlStreamReader &sr)
        throw(Core::XmlException)
{
    row_data->tuple.ReadXml(sr);
}



DataObjects::DataRow::RowData::RowData(DataObjects::DataTable *t)
{
    table = t;
    identifier = QUuid::createUuid();
}

DataObjects::DataRow::RowData::RowData(const DataObjects::DataRow::RowData &o)
    :QSharedData(o)
{
    table = o.table;
    tuple = o.tuple;

    identifier = QUuid::createUuid();
}













DataObjects::DataRowCollection::DataRowCollection(DataTable *dt)
{
    _table = dt;
}

DataObjects::DataRowCollection::~DataRowCollection(){}

int DataObjects::DataRowCollection::find_row_by_id(const QUuid &row_id)
{
    int ret = -1;

    for(int i = 0; i < Count(); i++)
    {
        if(Value(i).row_data->identifier == row_id)
        {
            ret = i;
            break;
        }
    }

    return ret;
}

void DataObjects::DataRowCollection::onAdd(void *v, int index)
{
    DataObjects::DataRow *dr = (DataObjects::DataRow *)v;

    dr->row_data->table = _table;
    dr->set_number_of_columns(_table->ColumnCount());
}
