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

#include "gvariant.h"
#include "Core/Utils/encryption.h"
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QDate>
#include <QBitArray>
#include <QRegExp>
#include <QStringList>
using namespace GUtil;

#define XMLID "GVariant"

Custom::GVariant::GVariant()
    :QVariant()
{
}

Custom::GVariant::GVariant(const QVariant &o)
    :QVariant(o)
{
}

Custom::GVariant::GVariant(Type t)
    :QVariant(t)
{
}

Custom::GVariant::GVariant(const QString &s)
    :QVariant(s)
{
}

Custom::GVariant::GVariant(const QByteArray &b)
    :QVariant(b)
{
}

Custom::GVariant::GVariant(int i)
    :QVariant(i)
{
}

Custom::GVariant::GVariant(uint i)
    :QVariant(i)
{
}

Custom::GVariant::GVariant(char *c)
    :QVariant(c)
{
}

Custom::GVariant::GVariant(bool b)
    :QVariant(b)
{
}

Custom::GVariant::GVariant(double d)
    :QVariant(d)
{
}

Custom::GVariant::GVariant(float f)
    :QVariant(f)
{
}

Custom::GVariant::GVariant(const QDate &d)
    :QVariant(d)
{
}

Custom::GVariant::GVariant(const QTime &t)
    :QVariant(t)
{
}

Custom::GVariant::GVariant(const QDateTime &d)
    :QVariant(d)
{
}

Custom::GVariant::GVariant(const QBitArray &b)
    :QVariant(b)
{
}

Custom::GVariant::GVariant(const QStringList &s)
    :QVariant(s)
{
}

Custom::GVariant::GVariant(const QRegExp &r)
    :QVariant(r)
{
}

Custom::GVariant::GVariant(const QUrl &u)
    :QVariant(u)
{
}

Custom::GVariant::GVariant(const QList<QVariant> &l)
    :QVariant(l)
{
}




void Custom::GVariant::WriteXml(QXmlStreamWriter &sw) const
{
    sw.writeStartElement(XMLID);
    sw.writeAttribute("t", QVariant((int)type()).toString());

    switch(type())
    {
    case String:
        sw.writeAttribute("d", Core::Utils::CryptoHelpers::toBase64(
                toString().toStdString()));
        break;
    case ByteArray:
        QByteArray ba = toByteArray();
        sw.writeAttribute("d", Core::Utils::CryptoHelpers::toBase64(
                std::string(ba.constData(), ba.length())));
        break;
    case Int:
        sw.writeAttribute("d", toString());
        break;
    case UInt:
        sw.writeAttribute("d", toString());
        break;
    case Bool:
        sw.writeAttribute("d", toBool() ? "1" : "0");
        break;
    case Double:
        sw.writeAttribute("d", toString());
        break;
    case Date:
        QDate d = toDate();
        sw.writeAttribute("d", QString("%1,%2,%3").arg(d.year()).arg(d.month()).arg(d.day()));
        break;
    case Time:
        QTime t = toTime();
        sw.writeAttribute("d", QString("%1,%2,%3,%4")
                          .arg(t.hour())
                          .arg(t.minute())
                          .arg(t.second())
                          .arg(t.msec()));
        break;
    case DateTime:
        QDateTime dt = toDateTime();
        sw.writeAttribute("d", QString("%1,%2,%3;%4,%5,%6,%7;%8")
                          .arg(dt.date().year())
                          .arg(dt.date().month())
                          .arg(dt.date().day())
                          .arg(dt.time().hour())
                          .arg(dt.time().minute())
                          .arg(dt.time().second())
                          .arg(dt.time().msec())
                          .arg((int)dt.timeSpec()));
        break;
    case BitArray:
        QBitArray bia = toBitArray();
        QString s = "";
        for(int i = bia.size() - 1; i >= 0; i--)
            s.append(bia.at(i) ? "1" : "0");
        sw.writeAttribute("d", s);
        break;
    case StringList:
        foreach(QString z, toStringList())
        {
            sw.writeStartElement("i");
            sw.writeAttribute("d", Core::Utils::CryptoHelpers::toBase64(z.toStdString()));
            sw.writeEndElement();
        }
        break;
    case RegExp:
        QRegExp re = toRegExp();
        sw.writeAttribute("d", re.pattern());
        sw.writeAttribute("e", QString("%1,%2")
                          .arg((int)re.caseSensitivity())
                          .arg((int)re.patternSyntax()));
        break;
    case Url:
        sw.writeAttribute("d", toString());
        break;
    case List:
        foreach(QVariant v, toList())
            Custom::GVariant(v).WriteXml(sw);
        break;
    default:
        break;
    }

    sw.writeEndElement();
}

void Custom::GVariant::ReadXml(QXmlStreamReader &sr)
        throw(GUtil::Core::XmlException)
{
    clear();

    if(sr.readNextStartElement())
    {
        if(sr.name() != XMLID)
            throw Core::XmlException();

        bool ok;
        QString val = sr.attributes().value("t").toString();
        Type type = (Type)val.toInt(&ok);
        if(!ok)
            throw Core::XmlException(QString("Invalid variant type: %1").arg(val));

        convert(type);


        QString d = sr.attributes().value("d").toString();

        switch(type)
        {
        case String:
            setValue(d);
            break;
        case ByteArray:
            setValue(d);
            break;
        case Int:
            setValue(d.toInt());
            break;
        case UInt:
            setValue(d.toUInt());
            break;
        case Bool:
            setValue(d == "1" ? true : false);
            break;
        case Double:
            setValue(d.toDouble());
            break;
        case Date:
            QStringList slDate = d.split(",");
            setValue(QDate(slDate.at(0).toInt(), slDate.at(1).toInt(), slDate.at(2).toInt()));
            break;
        case Time:
            QStringList slTime = d.split(",");
            setValue(QDate(slTime.at(0).toInt(), slTime.at(1).toInt(), slTime.at(2).toInt()));
            break;
        case DateTime:
            QStringList slDateTime1 = d.split(";");
            QStringList slDateTime2 = slDateTime1.at(0).split(",");
            QStringList slDateTime3 = slDateTime1.at(1).split(",");

            break;
        case BitArray:
            break;
        case StringList:
            break;
        case RegExp:
            break;
        case Url:
            break;
        case List:
            break;
        default:
            break;
        }

        while(sr.readNext() != QXmlStreamReader::EndElement);
    }
}
