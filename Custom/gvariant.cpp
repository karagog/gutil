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

#include "gvariant.h"
#include "Utils/qstringhelpers.h"
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <limits.h>
using namespace GUtil;

#define GVARIANT_XML_ID "GV"

Custom::GVariant::GVariant()
    :QVariant(){}

Custom::GVariant::GVariant(const QVariant &o)
    :QVariant(o){}

Custom::GVariant::GVariant(Type t)
    :QVariant(t){}

Custom::GVariant::GVariant(const std::string &s)
    :QVariant( QString::fromStdString(s) ){}

Custom::GVariant::GVariant(const QString &s)
    :QVariant(s){}

Custom::GVariant::GVariant(const QByteArray &b)
    :QVariant(b){}

Custom::GVariant::GVariant(const QChar &c)
    :QVariant(c){}

Custom::GVariant::GVariant(int i)
    :QVariant(i){}

Custom::GVariant::GVariant(uint i)
    :QVariant(i){}

Custom::GVariant::GVariant(long long i)
    :QVariant(i){}

Custom::GVariant::GVariant(unsigned long long i)
    :QVariant(i){}

Custom::GVariant::GVariant(char *c)
    :QVariant(c){}

Custom::GVariant::GVariant(const char *c)
    :QVariant(c){}

Custom::GVariant::GVariant(bool b)
    :QVariant(b){}

Custom::GVariant::GVariant(double d)
    :QVariant(d){}

Custom::GVariant::GVariant(float f)
    :QVariant(QVariant::fromValue(f)){}

Custom::GVariant::GVariant(const QDate &d)
    :QVariant(d){}

Custom::GVariant::GVariant(const QTime &t)
    :QVariant(t){}

Custom::GVariant::GVariant(const QDateTime &d)
    :QVariant(d){}

Custom::GVariant::GVariant(const QBitArray &b)
    :QVariant(b){}

Custom::GVariant::GVariant(const QStringList &s)
    :QVariant(s){}

Custom::GVariant::GVariant(const QRegExp &r)
    :QVariant(r){}

Custom::GVariant::GVariant(const QUrl &u)
    :QVariant(u){}

Custom::GVariant::GVariant(const QList<QVariant> &l)
    :QVariant(l){}

Custom::GVariant::GVariant(const QVariantMap &v)
    :QVariant(v){}

Custom::GVariant::GVariant(const QRect &r)
    :QVariant(r){}

Custom::GVariant::GVariant(const QSize &s)
    :QVariant(s){}

Custom::GVariant::GVariant(const QUuid &i)
    :QVariant(QUuidType, new QUuid(i)){}

QUuid Custom::GVariant::toUuid() const
{
    QUuid ret;
    if(type() == String || type() == ByteArray)
        ret = toString();
    else if(type() == UserType && userType() == QUuidType)
        ret = value<QUuid>();
    return ret;
}

QString Custom::GVariant::ConvertToXmlQString(const GVariant &v, bool h)
{
    return GVariant(v).ToXmlQString(h);
}

Custom::GVariant Custom::GVariant::ConvertFromXmlQString(const QString &xml)
{
    GVariant ret;
    ret.FromXmlQString(xml);
    return ret;
}



int Custom::GVariant::QUuidType = qMetaTypeId<QUuid>();
int Custom::GVariant::FloatType = qMetaTypeId<float>();

void Custom::GVariant::WriteXml(QXmlStreamWriter &sw) const
{
    int type = userType();

    sw.writeStartElement(GVARIANT_XML_ID);
    sw.writeAttribute("t", QString("%1").arg(type));

    QString tmps;
    switch(type)
    {
    case String:
        sw.writeAttribute("d", Utils::QStringHelpers::toBase64(toString()));
        break;
    case ByteArray:
        sw.writeAttribute("d", Utils::QStringHelpers::toBase64(
                QString::fromStdString(
                        std::string(toByteArray().constData(),
                                    toByteArray().length()))));
        break;
    case Char:
        sw.writeAttribute("d", Utils::QStringHelpers::toBase64(toChar()));
        break;
    case Int:
        sw.writeAttribute("d", toString());
        break;
    case UInt:
        sw.writeAttribute("d", toString());
        break;
    case LongLong:
        sw.writeAttribute("d", toString());
        break;
    case ULongLong:
        sw.writeAttribute("d", toString());
        break;
    case Bool:
        sw.writeAttribute("d", toBool() ? "1" : "0");
        break;
    case Double:
        sw.writeAttribute("d", toString());
        break;
    case Date:
        sw.writeAttribute("d", QString("%1,%2,%3")
                          .arg(toDate().year())
                          .arg(toDate().month())
                          .arg(toDate().day()));
        break;
    case Time:
        sw.writeAttribute("d", QString("%1,%2,%3,%4")
                          .arg(toTime().hour())
                          .arg(toTime().minute())
                          .arg(toTime().second())
                          .arg(toTime().msec()));
        break;
    case DateTime:
        sw.writeAttribute("d", QString("%1,%2,%3;%4,%5,%6,%7;%8")
                          .arg(toDateTime().date().year())
                          .arg(toDateTime().date().month())
                          .arg(toDateTime().date().day())
                          .arg(toDateTime().time().hour())
                          .arg(toDateTime().time().minute())
                          .arg(toDateTime().time().second())
                          .arg(toDateTime().time().msec())
                          .arg((int)toDateTime().timeSpec()));
        break;
    case BitArray:
        tmps = "";
        for(int i = toBitArray().size() - 1; i >= 0; i--)
            tmps.append(toBitArray().at(i) ? "1" : "0");
        sw.writeAttribute("d", tmps);
        break;
    case StringList:
        sw.writeAttribute("s", QString("%1").arg(toStringList().length()));
        foreach(QString z, toStringList())
        {
            sw.writeStartElement("i");
            sw.writeAttribute("d", Utils::QStringHelpers::toBase64(z));
            sw.writeEndElement();
        }
        break;
    case RegExp:
        sw.writeAttribute("d", toRegExp().pattern());
        sw.writeAttribute("e", QString("%1,%2")
                          .arg((int)toRegExp().caseSensitivity())
                          .arg((int)toRegExp().patternSyntax()));
        break;
    case Url:
        sw.writeAttribute("d", toString());
        break;
    case List:
        sw.writeAttribute("s", QString("%1").arg(toList().length()));
        foreach(QVariant v, toList())
            GVariant(v).WriteXml(sw);
        break;
    case Map:
        sw.writeAttribute("s", QString("%1").arg(toMap().keys().length()));
        foreach(QString z, toMap().keys())
        {
            sw.writeStartElement("i");
            sw.writeAttribute("k", Utils::QStringHelpers::toBase64(z));
            GVariant(toMap().value(z)).WriteXml(sw);
            sw.writeEndElement();
        }
        break;
    case Size:
        sw.writeAttribute("d", QString("%1,%2")
                          .arg(toSize().width())
                          .arg(toSize().height()));
        break;
    case Rect:
        sw.writeAttribute("d", QString("%1,%2,%3,%4")
                          .arg(toRect().x())
                          .arg(toRect().y())
                          .arg(toRect().width())
                          .arg(toRect().height()));
        break;
    default:

        // These are our custom types, that have to be handled separately
        if(type == QUuidType)
            sw.writeAttribute("d", value<QUuid>().toString());
        else if(type == FloatType)
            sw.writeAttribute("d", QString("%1").arg(value<float>()));

        break;
    }

    sw.writeEndElement();
}

void Custom::GVariant::ReadXml(QXmlStreamReader &sr)
        throw(GUtil::Core::XmlException)
{
    if(sr.readNextStartElement())
    {
        if(sr.name() != GVARIANT_XML_ID)
            throw Core::XmlException();

        int type = sr.attributes().at(0).value().toString().toInt();

        clear();

        QString d;
        if(sr.attributes().count() > 1)
            d = sr.attributes().at(1).value().toString();

        int tmpint;
        QString tmps;
        QStringList sltemp1;
        QStringList sltemp2;
        QStringList sltemp3;
        QBitArray baBitArray;
        QVariantList vl;
        QVariantMap vm;
        switch(type)
        {
        case String:
            setValue(Utils::QStringHelpers::fromBase64(d));
            break;
        case ByteArray:
            tmps = Utils::QStringHelpers::fromBase64(d);
            setValue(QByteArray(tmps.toStdString().c_str(), tmps.length()));
            break;
        case Char:
            setValue(Utils::QStringHelpers::fromBase64(d).at(0));
            break;
        case Int:
            setValue(d.toInt());
            break;
        case UInt:
            setValue(d.toUInt());
            break;
        case LongLong:
            setValue(d.toLongLong());
            break;
        case ULongLong:
            setValue(d.toULongLong());
            break;
        case Bool:
            setValue(d == "1" ? true : false);
            break;
        case Double:
            setValue(d.toDouble());
            break;
        case Date:
            sltemp1 = d.split(",");
            setValue(QDate(sltemp1.at(0).toInt(),
                           sltemp1.at(1).toInt(),
                           sltemp1.at(2).toInt()));
            break;
        case Time:
            sltemp1 = d.split(",");
            setValue(QTime(sltemp1.at(0).toInt(),
                           sltemp1.at(1).toInt(),
                           sltemp1.at(2).toInt(),
                           sltemp1.at(3).toInt()));
            break;
        case DateTime:
            sltemp1 = d.split(";");
            sltemp2 = sltemp1.at(0).split(",");
            sltemp3 = sltemp1.at(1).split(",");
            setValue(QDateTime(QDate(sltemp2.at(0).toInt(),
                                      sltemp2.at(1).toInt(),
                                      sltemp2.at(2).toInt()),
                               QTime(sltemp3.at(0).toInt(),
                                     sltemp3.at(1).toInt(),
                                     sltemp3.at(2).toInt(),
                                     sltemp3.at(3).toInt()),
                               (Qt::TimeSpec)sltemp1.at(2).toInt()));
            break;
        case BitArray:
            baBitArray.resize(d.length());
            for(int i = 0; i < d.length(); i++)
                baBitArray.setBit(d.length() - i - 1, d.at(i) == '1' ? true : false);
            setValue(baBitArray);
            break;
        case StringList:
            tmpint = d.toInt();
            for(int i = 0; i < tmpint; i++)
            {
                if(!sr.readNextStartElement())
                    throw Core::XmlException();

                sltemp1.append(Utils::QStringHelpers::fromBase64(
                        sr.attributes().at(0).value().toString()));
                while(sr.readNext() != QXmlStreamReader::EndElement);
            }
            setValue(sltemp1);
            break;
        case RegExp:
            sltemp1 = sr.attributes().at(2).value().toString().split(",");
            setValue(QRegExp(d, (Qt::CaseSensitivity)sltemp1.at(0).toInt(),
                             (QRegExp::PatternSyntax)sltemp1.at(1).toInt()));
            break;
        case Url:
            setValue(QUrl(d));
            break;
        case List:
            tmpint = d.toInt();
            for(int i = 0; i < tmpint; i++)
            {
                Custom::GVariant gv;
                gv.ReadXml(sr);
                vl.append(gv);
            }
            setValue(vl);
            break;
        case Map:
            tmpint = d.toInt();
            for(int i = 0; i < tmpint; i++)
            {
                if(!sr.readNextStartElement())
                    throw Core::XmlException();

                QString key = Utils::QStringHelpers::fromBase64(
                        sr.attributes().at(0).value().toString());
                GVariant gv;
                gv.ReadXml(sr);
                vm.insert(key, gv);

                while(sr.readNext() != QXmlStreamReader::EndElement);
            }
            setValue(vm);
            break;
        case Size:
            sltemp1 = d.split(",");
            setValue(QSize(sltemp1.at(0).toInt(), sltemp1.at(1).toInt()));
            break;
        case Rect:
            sltemp1 = d.split(",");
            setValue(QRect(sltemp1.at(0).toInt(),
                           sltemp1.at(1).toInt(),
                           sltemp1.at(2).toInt(),
                           sltemp1.at(3).toInt()));
            break;
        default:

            // These are our custom types, that have to be handled separately
            if(type == QUuidType)
                setValue(QUuid(d));
            else if(type == FloatType)
                setValue(d.toFloat());

            break;
        }

        while(sr.readNext() != QXmlStreamReader::EndElement);
    }
}

void Custom::GVariant::ToXml(const Custom::GVariant &g, QXmlStreamWriter &sw)
{
    g.WriteXml(sw);
}

Custom::GVariant Custom::GVariant::FromXml(QXmlStreamReader &sr)
{
    Custom::GVariant ret;
    ret.ReadXml(sr);
    return ret;
}

bool Custom::GVariant::Equals(const Custom::GVariant &o) const
{
    bool ret;
    int type = userType();

    if(type < (int)UserType)
        ret = ((QVariant &)*this) == o;
    else
    {
        // QVariant doesn't compare custom types for us, it only compares pointers,
        //  so we implement our own compare for the custom types
        if(type == QUuidType)
            ret = value<QUuid>() == o.value<QUuid>();
        else if(type == FloatType)
            ret = value<float>() == o.value<float>();
        else
            ret = false;
    }

    return ret;
}

bool Custom::GVariant::operator == (const QVariant &v) const
{
    return Equals(v);
}

bool Custom::GVariant::operator != (const QVariant &v) const
{
    return NotEquals(v);
}
