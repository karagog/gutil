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

#include "myxmlstreamreader.h"
#include <QXmlStreamWriter>
using namespace GUtil;

Custom::myXmlStreamReader::myXmlStreamReader()
    :QXmlStreamReader()
{

}

Custom::myXmlStreamReader::myXmlStreamReader(const QString &data)
    :QXmlStreamReader(data)
{

}

Custom::myXmlStreamReader::myXmlStreamReader(QIODevice *data)
    :QXmlStreamReader(data)
{

}

Custom::myXmlStreamReader::myXmlStreamReader(const QByteArray &data)
    :QXmlStreamReader(data)
{

}

Custom::myXmlStreamReader::myXmlStreamReader(const char *data)
    :QXmlStreamReader(data)
{

}

QString Custom::myXmlStreamReader::InnerText()
{
    QString ret = "";
    QXmlStreamWriter sw(&ret);
    sw.setAutoFormatting(false);

    if(tokenType() == myXmlStreamReader::StartElement)
    {
        int t;
        while((t = readNext()) != myXmlStreamReader::EndElement &&
              t != myXmlStreamReader::EndDocument &&
              t != myXmlStreamReader::Invalid)
        {
            _read_inner_xml(sw);
        }
    }

    return ret;
}

QString Custom::myXmlStreamReader::InnerXml()
{
    QString ret;
    QXmlStreamWriter sw(&ret);
    sw.setAutoFormatting(false);

    _read_inner_xml(sw);

    return ret;
}

void Custom::myXmlStreamReader::_read_inner_xml(QXmlStreamWriter &sw)
{
    if(tokenType() == myXmlStreamReader::StartElement)
    {
        sw.writeStartElement(name().toString());

        foreach(QXmlStreamAttribute a, attributes())
        {
            sw.writeAttribute(a.name().toString(), a.value().toString());
        }

        int t;
        while((t = readNext()) != myXmlStreamReader::EndElement &&
              t != myXmlStreamReader::EndDocument &&
              t != myXmlStreamReader::Invalid)
        {
            if(t == myXmlStreamReader::Characters)
                sw.writeCharacters(text().toString());
            else
                _read_inner_xml(sw);
        }

        sw.writeEndElement();
    }
}



