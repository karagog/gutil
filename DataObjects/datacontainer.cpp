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
{}

DataObjects::DataContainer::DataContainer(const DataContainer &other)
{
    foreach(QString k, other.keys())
        setValue(k, other.getValue(k));
}

void DataObjects::DataContainer::setValue(const QString &key, const QByteArray &value)
{
    _values[key] = value;
}

QByteArray DataObjects::DataContainer::getValue(const QString &key) const
{
    return _values.value(key);
}

bool DataObjects::DataContainer::remove(const QString &key)
{
    QMap<QString, QByteArray>::iterator it = _values.find(key);

    if(it == _values.end())
        return false;

    _values.erase(it);
    return true;
}

bool DataObjects::DataContainer::contains(const QString &key)
{
    return _values.contains(key);
}

void DataObjects::DataContainer::clear()
{
    _values.clear();
}

QStringList DataObjects::DataContainer::keys() const
{
    return _values.keys();
}

QByteArray &DataObjects::DataContainer::at(const QString &key)
{
    return _values[key];
}

QByteArray &DataObjects::DataContainer::operator [](const QString &key)
{
    return _values[key];
}

QByteArray DataObjects::DataContainer::toXml()
{
    QByteArray xmlstr;
    QXmlStreamWriter sw(&xmlstr);
    sw.setAutoFormatting(false);

    sw.writeStartDocument();
    sw.writeStartElement("settings");

    foreach(QString s, _values.keys())
    {
        // Don't bother writing empty settings, because they'll be defaulted to a
        //  null string anyways if they're not found
        QByteArray v = _values[s];
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

void DataObjects::DataContainer::fromXml(const QByteArray &dat) throw()
{
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

        _values.insert(sr.name().toString(), QByteArray(tmp.c_str(), tmp.length()));

        // Read in the end element tag
        sr.readNext();
    }
}
