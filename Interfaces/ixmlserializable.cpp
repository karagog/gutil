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

#include "ixmlserializable.h"
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
using namespace GUtil;

std::string Interfaces::IXmlSerializable::ToXml()
{
    QString ret;
    QXmlStreamWriter w(&ret);
    w.setAutoFormatting(IsXmlHumanReadableFormat());

    WriteXml(w);

    return ret.toStdString();
}

void Interfaces::IXmlSerializable::FromXml(const std::string &xml)
        throw(GUtil::Core::XmlException)
{
    QXmlStreamReader r(xml.c_str());
    ReadXml(r);
}
