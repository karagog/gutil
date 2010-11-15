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
    :_row_data(new RowData(dt, vals))
{}

DataObjects::DataRow::DataRow(const DataRow &o)
{
    *this = o;
}

DataObjects::DataRow::~DataRow(){}

DataObjects::DataRow &DataObjects::DataRow::operator =(const DataObjects::DataRow &o)
{
    _row_data = o._row_data;
    return *this;
}

bool DataObjects::DataRow::operator ==(const DataObjects::DataRow &o) const
{
    return _row_data == o._row_data;
}

bool DataObjects::DataRow::operator !=(const DataRow &o) const
{
    return !(*this == o);
}

DataObjects::DataRow &DataObjects::DataRow::CloneTo(DataObjects::DataRow &o) const
{
    o = *this;
    o._row_data.detach();
    return o;
}

DataObjects::DataRow DataObjects::DataRow::Clone() const
{
    DataRow dr(*this);
    return CloneTo(dr);
}

QVariant &DataObjects::DataRow::operator [](int index)
{
    if(index < 0 || index >= _row_data->tuple.Count())
        THROW_NEW_GUTIL_EXCEPTION( Core::IndexOutOfRangeException,
                QString("Tried index %1 of %2")
                .arg(index).arg(_row_data->tuple.Count()).toStdString() );

    return _row_data->tuple[index];
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
    if(_row_data->Table() == 0)
        THROW_NEW_GUTIL_EXCEPTION( Core::NullReferenceException, "Table not set" );

    return *_row_data->Table();
}

int DataObjects::DataRow::Index() const
{
    if(_row_data->Table() == 0)
        return -1;

    return _row_data->Table()->Rows().IndexOf(*this);
}

int DataObjects::DataRow::ColumnCount() const
{
    return _row_data->Table()->ColumnCount();
}

DataObjects::DataRow::RowData &DataObjects::DataRow::row_data() const
{
    return *_row_data;
}

void DataObjects::DataRow::set_number_of_columns(int cols)
{
    _row_data->tuple.Resize(cols);
}

QVariant DataObjects::DataRow::At(int index) const
{
    return _row_data->tuple.Value(index);
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
        DataObjects::QVariantHelpers::WriteXml(_row_data->tuple.Value(i), sw);

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

        _row_data->tuple.Clear();

        for(int i = 0; i < cnt; i++)
            _row_data->tuple.Add(DataObjects::QVariantHelpers::ReadXml(sr));

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
    SetTable(o._table);
}

DataObjects::DataTable *DataObjects::DataRow::RowData::Table() const
{
    return _table;
}

void DataObjects::DataRow::RowData::SetTable(DataTable *dt)
{
    if((_table = dt))
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
        Value(i).CloneTo(o[i]);

    return o;
}

DataObjects::DataRow DataObjects::DataRowCollection::create_blank_item() const
{
    return DataRow(_table);
}

void DataObjects::DataRowCollection::validate_new_item(const DataObjects::DataRow &r) const
        throw(Core::ValidationException)
{
    if(r.row_data().Table() != _table)
        THROW_NEW_GUTIL_EXCEPTION(Core::ValidationException,
                                  "The row does not belong to this table.  "
                                  "If you still want to add it, then call 'ImportRow' "
                                  "on the parent table.");
    else if(Contains(r))
        THROW_NEW_GUTIL_EXCEPTION(Core::ValidationException,
                                  "Row already exists in the table");
}
