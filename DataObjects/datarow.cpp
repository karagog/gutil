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
#include "datatablebase.h"
#include "datarowcollectionbase.h"
#include "datarowcollection.h"
#include "Gvariantcollection.h"
#include "Custom/gvariant.h"
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
using namespace GUtil;
using namespace Custom;

DataObjects::DataRow::DataRow(const DataObjects::DataTable &dt,
                              const GVariantList &vals)
    :_row_data(new SharedRowData(dt, vals))
{}

DataObjects::DataRow::DataRow(const DataRow &o)
{
    *this = o;
}

DataObjects::DataRow::DataRow(Custom::GSharedData *rd)
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
    copy_shared_data(o._row_data);

    return o;
}

void DataObjects::DataRow::copy_shared_data(
        Custom::GSharedDataPointer< Custom::GSharedData > &dest) const
{
    dest = new SharedRowData(row_data());
}

Custom::GVariant &DataObjects::DataRow::operator [](int index)
{
    return row_data().Tuple()[index];
}

const Custom::GVariant &DataObjects::DataRow::operator [](int index) const
{
    return row_data().Tuple()[index];
}

Custom::GVariant &DataObjects::DataRow::operator [](const QString &column_header)
{
    return At(row_data().Table().GetColumnIndex(column_header));
}

const Custom::GVariant &DataObjects::DataRow::operator [](const QString &column_header) const
{
    return At(row_data().Table().GetColumnIndex(column_header));
}

//DataObjects::DataTable &DataObjects::DataRow::Table()
//{
//    return row_data().Table();
//}

//const DataObjects::DataTable &DataObjects::DataRow::Table() const
//{
//    return row_data().Table();
//}

int DataObjects::DataRow::Index() const
{
    return row_data().Table().Rows().IndexOf(*this);
}

int DataObjects::DataRow::ColumnCount() const
{
    return row_data().Table().ColumnCount();
}

DataObjects::SharedRowData &DataObjects::DataRow::row_data()
{
    return (SharedRowData &)*_row_data;
}

const DataObjects::SharedRowData &DataObjects::DataRow::row_data() const
{
    return (const SharedRowData &)*_row_data;
}

void DataObjects::DataRow::set_number_of_columns(int cols)
{
    row_data().Tuple().Resize(cols);
}

Custom::GVariant &DataObjects::DataRow::At(int index)
{
    return row_data().Tuple().At(index);
}

const Custom::GVariant &DataObjects::DataRow::At(int index) const
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
            if(At(i).NotEquals(rhs.At(i)))
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
        GVariant::ToXml(row_data().Tuple().At(i), sw);

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
            row_data().Tuple().Add( GVariant::FromXml(sr) );

        // Derived classes initialize their data here
        read_xml_protected(sr);

        while(sr.readNext() != QXmlStreamReader::EndElement ||
              sr.name() != ROW_XML_ID);
    }
}

