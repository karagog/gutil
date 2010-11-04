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

#include "qvarianthelpers.h"
#include "Utils/qstringhelpers.h"
#include "Core/exception.h"
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

#define XMLID "V"

QString DataObjects::QVariantHelpers::ConvertToXmlQString(const QVariant &v, bool h)
{
    QString ret;
    QXmlStreamWriter sw(&ret);
    sw.setAutoFormatting(h);

    _write_xml(v, sw);

    return ret;
}

DataObjects::QVariant Custom::QVariantHelpers::ConvertFromXmlQString(const QString &xml)
{
    QXmlStreamReader sr(xml);
    return _read_xml(sr);
}





void DataObjects::QVariantHelpers::_write_xml(const QVariant &v, QXmlStreamWriter &sw)
{
    sw.writeStartElement(XMLID);
    sw.writeAttribute("t", QVariant((int)v.type()).toString());

    QString tmps;
    switch(v.type())
    {
    case String:
        sw.writeAttribute("d", Utils::QStringHelpers::toBase64(v.toString()));
        break;
    case ByteArray:
        sw.writeAttribute("d", Utils::QStringHelpers::toBase64(
                QString::fromStdString(
                        std::string(v.toByteArray().constData(),
                                    v.toByteArray().length()))));
        break;
    case Char:
        sw.writeAttribute("d", Utils::QStringHelpers::toBase64(v.toChar()));
        break;
    case Int:
        sw.writeAttribute("d", v.toString());
        break;
    case UInt:
        sw.writeAttribute("d", v.toString());
        break;
    case Bool:
        sw.writeAttribute("d", v.toBool() ? "1" : "0");
        break;
    case Double:
        sw.writeAttribute("d", v.toString());
        break;
    case Date:
        sw.writeAttribute("d", QString("%1,%2,%3")
                          .arg(v.toDate().year())
                          .arg(v.toDate().month())
                          .arg(v.toDate().day()));
        break;
    case Time:
        sw.writeAttribute("d", QString("%1,%2,%3,%4")
                          .arg(v.toTime().hour())
                          .arg(v.toTime().minute())
                          .arg(v.toTime().second())
                          .arg(v.toTime().msec()));
        break;
    case DateTime:
        sw.writeAttribute("d", QString("%1,%2,%3;%4,%5,%6,%7;%8")
                          .arg(v.toDateTime().date().year())
                          .arg(v.toDateTime().date().month())
                          .arg(v.toDateTime().date().day())
                          .arg(v.toDateTime().time().hour())
                          .arg(v.toDateTime().time().minute())
                          .arg(v.toDateTime().time().second())
                          .arg(v.toDateTime().time().msec())
                          .arg((int)v.toDateTime().timeSpec()));
        break;
    case BitArray:
        tmps = "";
        for(int i = v.toBitArray().size() - 1; i >= 0; i--)
            tmps.append(v.toBitArray().at(i) ? "1" : "0");
        sw.writeAttribute("d", tmps);
        break;
    case StringList:
        sw.writeAttribute("s", QString("%1").arg(v.toStringList().length()));
        foreach(QString z, v.toStringList())
        {
            sw.writeStartElement("i");
            sw.writeAttribute("d", Utils::QStringHelpers::toBase64(z));
            sw.writeEndElement();
        }
        break;
    case RegExp:
        sw.writeAttribute("d", v.toRegExp().pattern());
        sw.writeAttribute("e", QString("%1,%2")
                          .arg((int)v.toRegExp().caseSensitivity())
                          .arg((int)v.toRegExp().patternSyntax()));
        break;
    case Url:
        sw.writeAttribute("d", v.toString());
        break;
    case List:
        sw.writeAttribute("s", QString("%1").arg(v.toList().length()));
        foreach(QVariant u, v.toList())
            Conv(u).WriteXml(sw);
        break;
    case Map:
        sw.writeAttribute("s", QString("%1").arg(toMap().keys().length()));
        foreach(QString z, toMap().keys())
        {
            sw.writeStartElement("i");
            sw.writeAttribute("k", Utils::QStringHelpers::toBase64(z));
            QVariantHelpers(toMap().value(z)).WriteXml(sw);
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
        break;
    }

    sw.writeEndElement();
}

QVariant DataObjects::QVariantHelpers::_read_xml(QXmlStreamReader &sr)
        throw(GUtil::Core::XmlException)
{
    QVariant ret;

    if(sr.readNextStartElement())
    {
        if(sr.name() != XMLID)
            throw Core::XmlException();

        Type type = (Type)sr.attributes().at(0).value().toString().toInt();

        ret.convert(type);

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
            ret.setValue(Utils::QStringHelpers::fromBase64(d));
            break;
        case ByteArray:
            tmps = Utils::QStringHelpers::fromBase64(d);
            ret.setValue(QByteArray(tmps.toStdString().c_str(), tmps.length()));
            break;
        case Char:
            ret.setValue(Utils::QStringHelpers::fromBase64(d).at(0));
            break;
        case Int:
            ret.setValue(d.toInt());
            break;
        case UInt:
            ret.setValue(d.toUInt());
            break;
        case Bool:
            ret.setValue(d == "1" ? true : false);
            break;
        case Double:
            ret.setValue(d.toDouble());
            break;
        case Date:
            sltemp1 = d.split(",");
            ret.setValue(QDate(sltemp1.at(0).toInt(),
                           sltemp1.at(1).toInt(),
                           sltemp1.at(2).toInt()));
            break;
        case Time:
            sltemp1 = d.split(",");
            ret.setValue(QTime(sltemp1.at(0).toInt(),
                           sltemp1.at(1).toInt(),
                           sltemp1.at(2).toInt(),
                           sltemp1.at(3).toInt()));
            break;
        case DateTime:
            sltemp1 = d.split(";");
            sltemp2 = sltemp1.at(0).split(",");
            sltemp3 = sltemp1.at(1).split(",");
            ret.setValue(QDateTime(QDate(sltemp2.at(0).toInt(),
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
            ret.setValue(baBitArray);
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
            ret.setValue(sltemp1);
            break;
        case RegExp:
            sltemp1 = sr.attributes().at(2).value().toString().split(",");
            ret.setValue(QRegExp(d, (Qt::CaseSensitivity)sltemp1.at(0).toInt(),
                             (QRegExp::PatternSyntax)sltemp1.at(1).toInt()));
            break;
        case Url:
            setValue(QUrl(d));
            break;
        case List:
            tmpint = d.toInt();
            for(int i = 0; i < tmpint; i++)
            {
                Custom::QVariantHelpers gv;
                gv.ReadXml(sr);
                vl.append(gv);
            }
            ret.setValue(vl);
            break;
        case Map:
            tmpint = d.toInt();
            for(int i = 0; i < tmpint; i++)
            {
                if(!sr.readNextStartElement())
                    throw Core::XmlException();

                QString key = Utils::QStringHelpers::fromBase64(
                        sr.attributes().at(0).value().toString());
                QVariantHelpers gv;
                gv.ReadXml(sr);
                vm.insert(key, gv);

                while(sr.readNext() != QXmlStreamReader::EndElement);
            }
            ret.setValue(vm);
            break;
        case Size:
            sltemp1 = d.split(",");
            ret.setValue(QSize(sltemp1.at(0).toInt(), sltemp1.at(1).toInt()));
            break;
        case Rect:
            sltemp1 = d.split(",");
            ret.setValue(QRect(sltemp1.at(0).toInt(),
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
