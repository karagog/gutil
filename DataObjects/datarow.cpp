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
#include "qvarianthelpers.h"
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
using namespace GUtil;

DataObjects::DataRow::DataRow(DataObjects::DataTable *dt, const QVariantList &vals)
    :row_data(new RowData(dt, vals))
{}

DataObjects::DataRow::DataRow(const DataRow &o)
{
    *this = o;
}

DataObjects::DataRow::~DataRow(){}

DataObjects::DataRow &DataObjects::DataRow::operator =(const DataObjects::DataRow &o)
{
    row_data = o.row_data;
    return *this;
}

bool DataObjects::DataRow::operator ==(const DataObjects::DataRow &o) const
{
    return row_data == o.row_data;
}

bool DataObjects::DataRow::operator !=(const DataRow &o) const
{
    return !(*this == o);
}

DataObjects::DataRow &DataObjects::DataRow::CloneTo(DataObjects::DataRow &o) const
{
    o = *this;
    o.row_data.detach();
    return o;
}

QVariant &DataObjects::DataRow::operator [](int index)
{
    if(index < 0 || index >= row_data->tuple.Count())
        THROW_NEW_GUTIL_EXCEPTION( Core::IndexOutOfRangeException,
                QString("Tried index %1 of %2")
                .arg(index).arg(row_data->tuple.Count()).toStdString() );

    return row_data->tuple[index];
}

QVariant &DataObjects::DataRow::operator [](const QString &column_header)
{
    int index = Table().GetColumnIndex(column_header);
    if(index == -1)
        THROW_NEW_GUTIL_EXCEPTION( Core::IndexOutOfRangeException,
                QString("Column not found: '%1'").arg(column_header).toStdString() );
    return (*this)[index];
}

DataObjects::DataTable &DataObjects::DataRow::Table()
{
    if(row_data->table == 0)
        THROW_NEW_GUTIL_EXCEPTION( Core::NullReferenceException, "Table not set" );

    return *row_data->table;
}

int DataObjects::DataRow::Index() const
{
    if(row_data->table == 0)
        return -1;

    return row_data->table->Rows().IndexOf(*this);
}

int DataObjects::DataRow::ColumnCount() const
{
    return row_data->tuple.Count();
}

void DataObjects::DataRow::set_table(DataObjects::DataTable *dt)
{
    row_data->SetTable(dt);
}

void DataObjects::DataRow::set_number_of_columns(int cols)
{
    row_data->tuple.Resize(cols);
}

QVariant DataObjects::DataRow::At(int index) const
{
    return row_data->tuple.Value(index);
}

bool DataObjects::DataRow::Equals(const DataObjects::DataRow &rhs) const
{
    bool ret = false;
    if(ColumnCount() == rhs.ColumnCount())
    {
        int i;
        for(i = 0; i < ColumnCount(); i++)
            if(At(i) != rhs.At(i))
                break;

        if(i == ColumnCount())
            ret = true;
    }
    return ret;
}




#define ROW_XML_ID "Row"

void DataObjects::DataRow::WriteXml(QXmlStreamWriter &sw) const
{
    sw.writeStartElement(ROW_XML_ID);
    sw.writeAttribute("s", QString("%1").arg(ColumnCount()));

    for(int i = 0; i < ColumnCount(); i++)
        DataObjects::QVariantHelpers::WriteXml(row_data->tuple.Value(i), sw);

    sw.writeEndElement();
}

void DataObjects::DataRow::ReadXml(QXmlStreamReader &sr)
        throw(Core::XmlException)
{
    if(sr.readNextStartElement())
    {
        if(sr.name() != ROW_XML_ID)
            THROW_NEW_GUTIL_EXCEPTION( Core::XmlException, QString("Unrecognized XML node: %1")
                                     .arg(sr.name().toString()).toStdString() );

        int cnt = sr.attributes().at(0).value().toString().toInt();

        row_data->tuple.Clear();

        for(int i = 0; i < cnt; i++)
            row_data->tuple.Add(DataObjects::QVariantHelpers::ReadXml(sr));

        while(sr.readNext() != QXmlStreamReader::EndElement ||
              sr.name() != ROW_XML_ID);
    }
}



DataObjects::DataRow::RowData::RowData(DataObjects::DataTable *t,
                                       const QVariantList &vals)
                                           :tuple(vals)
{
    SetTable(t);
}

DataObjects::DataRow::RowData::RowData(const DataObjects::DataRow::RowData &o)
    :QSharedData(o),
    tuple(o.tuple)
{
    SetTable(o.table);
}

void DataObjects::DataRow::RowData::SetTable(DataTable *dt)
{
    if((table = dt))
        tuple.Resize(dt->ColumnCount());
}













DataObjects::DataRowCollection::DataRowCollection(DataTable *dt)
{
    _table = dt;
}

DataObjects::DataRowCollection::DataRowCollection(const DataObjects::DataRowCollection &o)
{
    o.CloneTo(*this);
}

DataObjects::DataRowCollection &DataObjects::DataRowCollection::CloneTo(
        DataObjects::DataRowCollection &o) const
{
    o._table = _table;
    o.Resize(Size());

    // Clone each row explicitly; each row must detach itself from the shared pointer
    for(int i = 0; i < Count(); i++)
        this->Value(i).CloneTo(o[i]);

    return o;
}

DataObjects::DataRow DataObjects::DataRowCollection::create_blank_item() const
{
    return DataRow(_table);
}
