/*Copyright 2011 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#include "iqxmlserializable.h"
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
GUTIL_USING_NAMESPACE(Interfaces);
GUTIL_USING_CORE_NAMESPACE(DataObjects);

String IQXmlSerializable::ToXmlString(bool h) const
{
    return ToXmlQString(h);
}

void IQXmlSerializable::FromXmlString(const String &xml)
        throw(GUtil::Core::XmlException<false>)
{
    FromXmlQString(xml);
}

QString IQXmlSerializable::ToXmlQString(bool h) const
{
    QString ret;
    QXmlStreamWriter w(&ret);
    w.setAutoFormatting(h);

    WriteXml(w);

    return ret;
}

void IQXmlSerializable::FromXmlQString(const QString &xml)
        throw(GUtil::Core::XmlException<false>)
{
    QXmlStreamReader r(xml);
    ReadXml(r);
}
