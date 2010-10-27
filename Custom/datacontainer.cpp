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

#include "datacontainer.h"
#include "Core/Utils/stringhelpers.h"
#include "Core/exception.h"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
using namespace GUtil;

Custom::DataContainer::DataContainer()
    : QMap<QString, QByteArray>(), Interfaces::IQXmlSerializable(false)
{}

Custom::DataContainer::DataContainer(const DataContainer &other)
    : QMap<QString, QByteArray>(other), Interfaces::IQXmlSerializable(false)
{}

void Custom::DataContainer::WriteXml(QXmlStreamWriter &sw)
{
    sw.setAutoFormatting(IsXmlHumanReadableFormat());
    sw.writeStartElement("settings");

    foreach(QString s, keys())
    {
        // Don't bother writing empty settings, because they'll be defaulted to a
        //  null string anyways if they're not found
        QByteArray v = value(s);
        if(v.length() == 0)
            continue;

        sw.writeStartElement(s);
        sw.writeAttribute("v", QString::fromStdString(Core::Utils::StringHelpers::toBase64(
                std::string(v.constData(), v.length()))));
        sw.writeEndElement();
    }

    sw.writeEndElement();
}

#define READ_ERR_STRING "XML not in correct format"

void Custom::DataContainer::ReadXml(QXmlStreamReader &sr)
        throw(Core::XmlException)
{
    clear();

    if(!sr.readNextStartElement())
    {
        Core::XmlException ex(READ_ERR_STRING);
        ex.SetData("err", sr.errorString().toStdString());
        throw ex;
    }

    QString start_name = sr.name().toString();

    while(sr.readNextStartElement())
    {
        if(!sr.attributes().hasAttribute("v"))
            throw Core::XmlException(READ_ERR_STRING);

        std::string tmp = Core::Utils::StringHelpers::fromBase64(
                sr.attributes().value("v").toString().toStdString());

        insert(sr.name().toString(), QByteArray(tmp.c_str(), tmp.length()));

        // Read in the end element tag
        while(sr.readNext() != QXmlStreamReader::EndElement)
        {
            if(sr.tokenType() == QXmlStreamReader::Invalid)
            {
                Core::XmlException ex(READ_ERR_STRING);
                ex.SetData("err", sr.errorString().toStdString());
                throw ex;
            }
            else if(sr.tokenType() == QXmlStreamReader::StartElement)
            {
                Core::XmlException ex(READ_ERR_STRING);
                ex.SetData("err", "Encountered unexpected start element");
                throw ex;
            }
        }

        // Stop when we hit the end of the tag we started on
        if(sr.name() == start_name)
            break;
    }
}