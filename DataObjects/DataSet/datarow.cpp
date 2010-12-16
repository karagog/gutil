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
#include "datarowcollection.h"
#include "DataObjects/gvariantcollection.h"
#include "Custom/gvariant.h"
#include "DataObjects/updatablegvariantcollection.h"
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
using namespace GUtil;
using namespace Custom;

DataObjects::DataRow::DataRow(const DataObjects::DataTable &dt,
                              const GVariantList &vals)
    :ExplicitlySharedObject<SharedRowData>(new SharedRowData(dt, vals))
{}

DataObjects::DataRow::DataRow(const DataRow &o, bool clone)
    :ExplicitlySharedObject<SharedRowData>(o)
{
    if(clone)
        o.CloneTo(*this);
}

DataObjects::DataRow::DataRow(SharedRowData *rd)
    :ExplicitlySharedObject<SharedRowData>(rd)
{}

DataObjects::DataRow::~DataRow(){}

DataObjects::DataRow &DataObjects::DataRow::operator =(const DataObjects::DataRow &o)
{
    SetExplicitlySharedData(o.GetExplicitlySharedData());
    return *this;
}

DataObjects::DataRow &DataObjects::DataRow::CloneTo(DataObjects::DataRow &o) const
{
    (o = *this).Detach();

    return o;
}

Custom::UpdatableGVariant &DataObjects::DataRow::operator [](int index)
{
    return row_data().Tuple()[index];
}

const Custom::UpdatableGVariant &DataObjects::DataRow::operator [](int index) const
{
    return row_data().Tuple()[index];
}

Custom::UpdatableGVariant &DataObjects::DataRow::operator [](const QString &column_header)
{
    return At(row_data().Table().GetColumnIndex(column_header));
}

const Custom::UpdatableGVariant &DataObjects::DataRow::operator [](const QString &column_header) const
{
    return At(row_data().Table().GetColumnIndex(column_header));
}

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
    return *((SharedRowData *)GetExplicitlySharedData());
}

const DataObjects::SharedRowData &DataObjects::DataRow::row_data() const
{
    return *((SharedRowData *)GetExplicitlySharedData());
}

void DataObjects::DataRow::set_number_of_columns(int cols)
{
    row_data().Tuple().Resize(cols);
}

Custom::UpdatableGVariant &DataObjects::DataRow::At(int index)
{
    return row_data().Tuple().At(index);
}

const Custom::UpdatableGVariant &DataObjects::DataRow::At(int index) const
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

    sw.writeEndElement();
}

void DataObjects::DataRow::ReadXml(QXmlStreamReader &sr)
        throw(Core::XmlException)
{
    if(sr.readNextStartElement())
    {
        if(sr.name() != ROW_XML_ID)
            THROW_NEW_GUTIL_EXCEPTION2( Core::XmlException, QString("Unrecognized XML node: %1")
                                     .arg(sr.name().toString()).toStdString() );

        int cnt = sr.attributes().at(0).value().toString().toInt();

        row_data().Tuple().Clear();

        for(int i = 0; i < cnt; i++)
            row_data().Tuple().Add( GVariant::FromXml(sr) );

        while(sr.readNext() != QXmlStreamReader::EndElement ||
              sr.name() != ROW_XML_ID);
    }
}

void DataObjects::DataRow::row_value_about_to_change(int index, const Custom::GVariant &v)
{
    if(Table().KeyColumns().count() == 0 ||
       !Table().KeyColumns().contains(index))
        return;

    bool key_violation = false;
    for(int i = 0; !key_violation && i < Table().RowCount(); i++)
    {
        const DataRow &cur_row = Table().Rows()[i];

        if(cur_row == *this)
            continue;

        key_violation = true;
        foreach(int k, Table().KeyColumns())
        {
            if(k == index)
                key_violation = key_violation && (v == cur_row[k]);
            else
                key_violation = key_violation && (At(k) == cur_row[k]);

            if(!key_violation)
                break;
        }
    }

    if(key_violation)
        THROW_NEW_GUTIL_EXCEPTION2(Core::ValidationException,
                                  QString("Primary key violation trying to set index %1 "
                                          "to the value \"%2\"")
                                  .arg(index)
                                  .arg(v.toString())
                                  .toStdString());
}

void DataObjects::DataRow::commit_reject_changes(bool commit)
{
    if(commit)
        row_data().Tuple().CommitChanges();
    else
        row_data().Tuple().RejectChanges();
}
