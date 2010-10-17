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
#include "Custom/myxmlstreamreader.h"
#include "Core/Tools/stringhelpers.h"
#include "Core/exception.h"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <Utils/filelogger.h>
using namespace GUtil;

DataAccess::DataObjects::DataContainer::DataContainer()
    : QMap<QString, QByteArray>(), Core::Interfaces::IXmlSerializable(false)
{}

DataAccess::DataObjects::DataContainer::DataContainer(const DataContainer &other)
    : QMap<QString, QByteArray>(other), Core::Interfaces::IXmlSerializable(false)
{}

std::string DataAccess::DataObjects::DataContainer::ToXml()
{
    QString xmlstr;
    QXmlStreamWriter sw(&xmlstr);
    sw.setAutoFormatting(IsXmlHumanReadableFormat());

    sw.writeStartDocument();
    sw.writeStartElement("settings");

    foreach(QString s, keys())
    {
        // Don't bother writing empty settings, because they'll be defaulted to a
        //  null string anyways if they're not found
        QByteArray v = value(s);
        if(v.length() == 0)
            continue;

        sw.writeStartElement(s);
        sw.writeAttribute("v", QString::fromStdString(Core::Tools::StringHelpers::toBase64(
                std::string(v.constData(), v.length()))));
        sw.writeEndElement();
    }

    sw.writeEndElement();
    sw.writeEndDocument();

    return xmlstr.toStdString();
}

void DataAccess::DataObjects::DataContainer::FromXml(const std::string &dat) throw(Core::XmlException)
{
    clear();

    Utils::FileLogger fl("xml_log.log");
    fl.LogMessage(QString::fromStdString(dat), "Reading xml document");

    Custom::myXmlStreamReader sr(dat.c_str());

    //Read in the startDocument tag
    if(!sr.ReadStartDocument())
        throw Core::XmlException("Invalid Xml Document");

    if(!sr.ReadTilNextStartElement())  //Read in settings root
        return;

    while(sr.ReadTilNextStartElement())
    {
        fl.LogMessage(sr.name().toString());

        if(!sr.attributes().hasAttribute("v"))
            throw Core::XmlException("XML not in correct format");

        std::string tmp = Core::Tools::StringHelpers::fromBase64(
                sr.attributes().value("v").toString().toStdString());

        insert(sr.name().toString(), QByteArray(tmp.c_str(), tmp.length()));

        // Read in the end element tag
        //sr.readNext();
    }
}
