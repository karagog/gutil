/*Copyright 2010-2013 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "varianttable.h"
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
USING_NAMESPACE_GUTIL1(DataObjects);

NAMESPACE_GUTIL2(QT, DataObjects);


#define DATATABLE_XML_ID        "GUtil_Table"
#define READ_ERR_STRING         "XML not in correct format"

void VariantTable::WriteXml(QXmlStreamWriter &sw) const
{
    sw.writeStartElement(DATATABLE_XML_ID);
    sw.writeAttribute("n", Name().ToBase64());
    sw.writeAttribute("r", QString("%1").arg(RowCount()));
    sw.writeAttribute("c", QString("%1").arg(ColumnCount()));

    // Write our column data
    for(DataColumnCollection::const_iterator iter(Columns().begin());
        iter != Columns().end(); ++iter)
    {
        sw.writeStartElement("c");
        Variant::ToXml(iter->GetKey().ConstData(), sw);
        Variant::ToXml(iter->GetName().ConstData(), sw);
        sw.writeEndElement();
    }

    // Write each row to xml
    for(ConstRowIterator iter(RowsBegin()); iter != RowsEnd(); ++iter)
        WriteRowXml(sw, *iter);

    sw.writeEndElement();
}

void VariantTable::WriteRowXml(QXmlStreamWriter &sw, const DataRow<QVariant> &dr)
{
    sw.writeStartElement("r");
    for(DataRow<QVariant>::const_iterator iter(dr.begin()); iter != dr.end(); ++iter)
        Variant::ToXml(*iter, sw);
    sw.writeEndElement();
}

void VariantTable::ReadXml(QXmlStreamReader &sr)
{
    Rows().Clear();
    Columns().Clear();

    if(sr.readNextStartElement())
    {
        if(sr.name() != DATATABLE_XML_ID)
            THROW_NEW_GUTIL_EXCEPTION2(XmlException, String::Format("Unrecognized XML Node: %s", sr.name().toString().toUtf8().constData()));

        SetName( String(sr.attributes().at(0).value().toString()).FromBase64() );
        int rows = sr.attributes().at(1).value().toString().toInt();
        int cols = sr.attributes().at(2).value().toString().toInt();

        for(int i = 0; i < cols; i++)
        {
            if(!sr.readNextStartElement() || sr.name() != "c")
                THROW_NEW_GUTIL_EXCEPTION2(XmlException, "Unrecognized XML Data");
            String col_key( Variant::FromXml(sr).toString().toUtf8().constData() );
            String col_name( Variant::FromXml(sr).toString().toUtf8().constData() );
            AddColumn(col_key, col_name);
            while(sr.readNext() != QXmlStreamReader::EndElement);
        }

        for(int i = 0; i < rows; i++)
        {
            Utils::SharedSmartPointer< DataRow<QVariant> > r( CreateRow() );
            ReadRowXml(sr, *r);
            AddRow(r);
        }

        // Read to the end of the table's xml
        while(sr.readNext() != QXmlStreamReader::EndElement ||
              sr.name() != DATATABLE_XML_ID);
    }
    else
    {
        THROW_NEW_GUTIL_EXCEPTION2(XmlException, "Not a valid DataTable serialization");
    }
}

void VariantTable::ReadRowXml(QXmlStreamReader &sr, DataRow<QVariant> &dr)
{
    if(!sr.readNextStartElement())
        THROW_NEW_GUTIL_EXCEPTION2(XmlException, "Unrecognized XML Data");
    for(DataRow<QVariant>::iterator iter(dr.begin()); iter != dr.end(); ++iter)
        iter.SetValue(Variant::FromXml(sr));
    while(sr.tokenType() != QXmlStreamReader::EndElement || sr.name() != "r") sr.readNext();
}


END_NAMESPACE_GUTIL2;
