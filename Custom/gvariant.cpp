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
#include "Utils/qstringhelpers.h"
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QDate>
#include <QBitArray>
#include <QRegExp>
#include <QStringList>
#include <QUrl>
#include <QSize>
#include <QRect>
#include <QVariantMap>
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

Custom::GVariant::GVariant(const QChar &c)
    :QVariant(c)
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
    :QVariant((double)f)
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

Custom::GVariant::GVariant(const QVariantMap &v)
    :QVariant(v)
{
}

Custom::GVariant::GVariant(const QRect &r)
    :QVariant(r)
{
}

Custom::GVariant::GVariant(const QSize &s)
    :QVariant(s)
{
}





void Custom::GVariant::WriteXml(QXmlStreamWriter &sw) const
{
    sw.writeStartElement(XMLID);
    sw.writeAttribute("t", QVariant((int)type()).toString());

    QString tmps;
    switch(type())
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
        sw.writeAttribute("s", QString(toStringList().length()));
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
        sw.writeAttribute("s", QString(toList().length()));
        foreach(QVariant v, toList())
            Custom::GVariant(v).WriteXml(sw);
        break;
    case Map:
        throw Core::NotImplementedException();
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
        break;
    }

    sw.writeEndElement();
}

void Custom::GVariant::ReadXml(QXmlStreamReader &sr)
        throw(GUtil::Core::XmlException)
{
    if(sr.readNextStartElement())
    {
        if(sr.name() != XMLID)
            throw Core::XmlException();

        Type type = (Type)sr.attributes().at(0).value().toString().toInt();

        clear();
        convert(type);

        const QString d = sr.attributes().at(1).value().toString();

        int tmpint;
        QString tmps;
        QStringList sltemp1;
        QStringList sltemp2;
        QStringList sltemp3;
        QBitArray baBitArray;
        QVariantList vl;
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
            tmpint = sr.attributes().at(1).value().toString().toInt();
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
            sltemp1 = sr.attributes().at(1).value().toString().split(",");
            setValue(QRegExp(d, (Qt::CaseSensitivity)sltemp1.at(0).toInt(),
                             (QRegExp::PatternSyntax)sltemp1.at(1).toInt()));
            break;
        case Url:
            setValue(QUrl(d));
            break;
        case List:
            tmpint = sr.attributes().at(1).value().toString().toInt();
            for(int i = 0; i < tmpint; i++)
            {
                Custom::GVariant gv;
                gv.ReadXml(sr);
                vl.append(gv);
            }
            break;
        case Map:
            throw Core::NotImplementedException();
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
            break;
        }

        while(sr.readNext() != QXmlStreamReader::EndElement);
    }
}
