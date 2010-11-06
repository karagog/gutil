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

#include "datatuple.h"
#include "qvarianthelpers.h"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
using namespace GUtil;

DataObjects::DataTuple::DataTuple(int size)
    :Interfaces::ICollection<QVariant>(size)
{}

DataObjects::DataTuple::DataTuple(const DataTuple &o)
    :Interfaces::ICollection<QVariant>(o)
{}

DataObjects::DataTuple::~DataTuple(){}


#define TUPLE_XML_ID "tuple"

void DataObjects::DataTuple::WriteXml(QXmlStreamWriter &sw) const
{
    sw.writeStartElement(TUPLE_XML_ID);
    sw.writeAttribute("s", QString("%1").arg(Count()));

    for(int i = 0; i < Count(); i++)
        DataObjects::QVariantHelpers::WriteXml(this->Value(i), sw);

    sw.writeEndElement();
}

void DataObjects::DataTuple::ReadXml(QXmlStreamReader &sr)
        throw(Core::XmlException)
{
    if(sr.readNextStartElement())
    {
        if(sr.name() != TUPLE_XML_ID)
        {
            THROW_GUTIL_EXCEPTION( Core::XmlException(QString("Unrecognized XML node: %1")
                                     .arg(sr.name().toString()).toStdString()) )
        }

        int cnt = sr.attributes().at(0).value().toString().toInt();

        ClearValues();

        for(int i = 0; i < cnt; i++)
            Add(DataObjects::QVariantHelpers::ReadXml(sr));

        while(sr.readNext() != QXmlStreamReader::EndElement ||
              sr.name() != TUPLE_XML_ID);
    }
}
