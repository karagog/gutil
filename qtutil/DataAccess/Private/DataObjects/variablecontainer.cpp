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

#include "variablecontainer.h"
#include "stringhelpers.h"
#include "exception.h"
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
using namespace GUtil;
using namespace GQtUtil::DataAccess::Private::DataObjects;

void VariableContainer::setValue(const QString &key, const QString &value)
{
    _values[key] = value;
}

QString VariableContainer::getValue(const QString &key)
{
    return _values.value(key);
}

bool VariableContainer::remove(const QString &key)
{
    QMap<QString, QString>::iterator it = _values.last()->find(key);

    if(it == _values.end())
        return false;

    _values.erase(it);
    return true;
}

bool VariableContainer::contains(const QString &key)
{
    return _values.contains(key);
}

void VariableContainer::clear()
{
    _values.clear();
}

QString &VariableContainer::at(const QString &key) const
{
    return _values[key];
}

QString &VariableContainer::operator [](const QString &key) const
{
    return _values[key];
}

QByteArray VariableContainer::toXml()
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
        QString v = _values[s];
        if(v.length() == 0)
            continue;

        QString encoded_string = QString::fromStdString(
                StringHelpers::toBase64(v.toStdString()));

        sw.writeStartElement(s);
        sw.writeAttribute("v", encoded_string);
        sw.writeEndElement();
    }

    sw.writeEndElement();
    sw.writeEndDocument();

    return xmlstr;
}

void VariableContainer::fromXml(const QByteArray &dat)
{
    clear();

    QXmlStreamReader sr(dat);
    //Read in the startDocument tag
    sr.readNext();

    if(!sr.readNextStartElement())  //Read in settings root
        throw GUtil::Exception("XML not recognized");

    while(sr.readNextStartElement())
    {
        QString decoded_string = QString::fromStdString(
                StringHelpers::fromBase64(sr.attributes().value("v").toString().toStdString()));
        _values.insert(sr.name().toString(), decoded_string);

        // Read in the end element tag
        sr.readNext();
    }
}
