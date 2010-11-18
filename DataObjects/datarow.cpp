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
#include "datarowcollection.h"
#include "qvariantcollection.h"
#include "qvarianthelpers.h"
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
using namespace GUtil;

DataObjects::DataRow::DataRow(const DataObjects::DataTable &dt, const QVariantList &vals)
    :_row_data(new SharedRowData(&dt.table_data(), vals))
{}

DataObjects::DataRow::DataRow(const DataRow &o)
{
    *this = o;
}

DataObjects::DataRow::DataRow(SharedRowData *rd)
    :_row_data(rd)
{}

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
    if(index < 0 || index >= row_data().Tuple().Count())
        THROW_NEW_GUTIL_EXCEPTION( Core::IndexOutOfRangeException,
                QString("Tried index %1 of %2")
                .arg(index).arg(row_data().Tuple().Count()).toStdString() );

    return row_data().Tuple()[index];
}

QVariant &DataObjects::DataRow::operator [](const QString &column_header)
{
    int index = Table().GetColumnIndex(column_header);
    if(index == -1)
        THROW_NEW_GUTIL_EXCEPTION( Core::IndexOutOfRangeException,
                QString("Column not found: '%1'").arg(column_header).toStdString() );
    return (*this)[index];
}

DataObjects::DataTable DataObjects::DataRow::Table() const
{
    return DataTable(row_data().TableData());
}

int DataObjects::DataRow::Index() const
{
    return Table().Rows().IndexOf(*this);
}

int DataObjects::DataRow::ColumnCount() const
{
    return Table().ColumnCount();
}

DataObjects::SharedRowData &DataObjects::DataRow::row_data() const
{
    return *_row_data;
}

void DataObjects::DataRow::set_number_of_columns(int cols)
{
    row_data().Tuple().Resize(cols);
}

QVariant DataObjects::DataRow::At(int index) const
{
    return row_data().Tuple().At(index);
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
        DataObjects::QVariantHelpers::WriteXml(row_data().Tuple().At(i), sw);

    // Derived classes serialize their data here
    write_xml_protected(sw);

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

        row_data().Tuple().Clear();

        for(int i = 0; i < cnt; i++)
            row_data().Tuple().Add(DataObjects::QVariantHelpers::ReadXml(sr));

        // Derived classes initialize their data here
        read_xml_protected(sr);

        while(sr.readNext() != QXmlStreamReader::EndElement ||
              sr.name() != ROW_XML_ID);
    }
}

