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
#include "Core/Tools/stringhelpers.h"
#include "Core/exception.h"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
using namespace GUtil;

DataObjects::DataContainer::DataContainer()
    :Interfaces::IXmlSerializable(),
    Interfaces::IReadOnlyObject()
{}

DataObjects::DataContainer::DataContainer(const DataContainer &other)
    :Interfaces::IXmlSerializable(),
    Interfaces::IReadOnlyObject(other)
{}

void DataObjects::DataContainer::setValue(const QString &key, const QByteArray &value)
{
    FailIfReadOnly();

    _data[key] = value;
}

QByteArray DataObjects::DataContainer::getValue(const QString &key) const
{
    return _data.value(key);
}

bool DataObjects::DataContainer::remove(const QString &key)
{
    FailIfReadOnly();

    QMap<QString, QByteArray>::iterator it = _data.find(key);

    if(it == _data.end())
        return false;

    _data.erase(it);
    return true;
}

bool DataObjects::DataContainer::contains(const QString &key)
{
    return _data.contains(key);
}

void DataObjects::DataContainer::clear()
{
    FailIfReadOnly();

    _data.clear();
}

QStringList DataObjects::DataContainer::keys() const
{
    return _data.keys();
}

QByteArray DataObjects::DataContainer::toXml()
{
    QByteArray xmlstr;
    QXmlStreamWriter sw(&xmlstr);
    sw.setAutoFormatting(false);

    sw.writeStartDocument();
    sw.writeStartElement("settings");

    foreach(QString s, _data.keys())
    {
        // Don't bother writing empty settings, because they'll be defaulted to a
        //  null string anyways if they're not found
        QByteArray v = _data[s];
        if(v.length() == 0)
            continue;

        sw.writeStartElement(s);
        sw.writeAttribute("v", QString::fromStdString(Core::Tools::StringHelpers::toBase64(
                std::string(v.constData(), v.length()))));
        sw.writeEndElement();
    }

    sw.writeEndElement();
    sw.writeEndDocument();

    return xmlstr;
}

void DataObjects::DataContainer::fromXml(const QByteArray &dat)
{
    FailIfReadOnly();

    clear();

    QXmlStreamReader sr(dat);
    //Read in the startDocument tag
    sr.readNext();

    if(!sr.readNextStartElement())  //Read in settings root
        throw Core::Exception("XML not recognized");

    while(sr.readNextStartElement())
    {
        std::string tmp = Core::Tools::StringHelpers::fromBase64(
                sr.attributes().value("v").toString().toStdString());

        _data.insert(sr.name().toString(), QByteArray(tmp.c_str(), tmp.length()));

        // Read in the end element tag
        sr.readNext();
    }
}
