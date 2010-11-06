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

#define QVARIANT_XML_ID "QV"

QString DataObjects::QVariantHelpers::ConvertToXmlQString(const QVariant &v, bool h)
{
    QString ret;
    QXmlStreamWriter sw(&ret);
    sw.setAutoFormatting(h);

    WriteXml(v, sw);

    return ret;
}

QVariant DataObjects::QVariantHelpers::ConvertFromXmlQString(const QString &xml)
{
    QXmlStreamReader sr(xml);
    return ReadXml(sr);
}





void DataObjects::QVariantHelpers::WriteXml(const QVariant &v, QXmlStreamWriter &sw)
{
    sw.writeStartElement(QVARIANT_XML_ID);
    sw.writeAttribute("t", QVariant((int)v.type()).toString());

    QString tmps;
    switch(v.type())
    {
    case QVariant::String:
        sw.writeAttribute("d", Utils::QStringHelpers::toBase64(v.toString()));
        break;
    case QVariant::ByteArray:
        sw.writeAttribute("d", Utils::QStringHelpers::toBase64(
                QString::fromStdString(
                        std::string(v.toByteArray().constData(),
                                    v.toByteArray().length()))));
        break;
    case QVariant::Char:
        sw.writeAttribute("d", Utils::QStringHelpers::toBase64(v.toChar()));
        break;
    case QVariant::Int:
        sw.writeAttribute("d", v.toString());
        break;
    case QVariant::UInt:
        sw.writeAttribute("d", v.toString());
        break;
    case QVariant::Bool:
        sw.writeAttribute("d", v.toBool() ? "1" : "0");
        break;
    case QVariant::Double:
        sw.writeAttribute("d", v.toString());
        break;
    case QVariant::Date:
        sw.writeAttribute("d", QString("%1,%2,%3")
                          .arg(v.toDate().year())
                          .arg(v.toDate().month())
                          .arg(v.toDate().day()));
        break;
    case QVariant::Time:
        sw.writeAttribute("d", QString("%1,%2,%3,%4")
                          .arg(v.toTime().hour())
                          .arg(v.toTime().minute())
                          .arg(v.toTime().second())
                          .arg(v.toTime().msec()));
        break;
    case QVariant::DateTime:
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
    case QVariant::BitArray:
        tmps = "";
        for(int i = v.toBitArray().size() - 1; i >= 0; i--)
            tmps.append(v.toBitArray().at(i) ? "1" : "0");
        sw.writeAttribute("d", tmps);
        break;
    case QVariant::StringList:
        sw.writeAttribute("s", QString("%1").arg(v.toStringList().length()));
        foreach(QString z, v.toStringList())
        {
            sw.writeStartElement("i");
            sw.writeAttribute("d", Utils::QStringHelpers::toBase64(z));
            sw.writeEndElement();
        }
        break;
    case QVariant::RegExp:
        sw.writeAttribute("d", v.toRegExp().pattern());
        sw.writeAttribute("e", QString("%1,%2")
                          .arg((int)v.toRegExp().caseSensitivity())
                          .arg((int)v.toRegExp().patternSyntax()));
        break;
    case QVariant::Url:
        sw.writeAttribute("d", v.toString());
        break;
    case QVariant::List:
        sw.writeAttribute("s", QString("%1").arg(v.toList().length()));
        foreach(QVariant u, v.toList())
            WriteXml(u, sw);
        break;
    case QVariant::Map:
        sw.writeAttribute("s", QString("%1").arg(v.toMap().keys().length()));
        foreach(QString z, v.toMap().keys())
        {
            sw.writeStartElement("i");
            sw.writeAttribute("k", Utils::QStringHelpers::toBase64(z));
            WriteXml(v.toMap().value(z), sw);
            sw.writeEndElement();
        }
        break;
    case QVariant::Size:
        sw.writeAttribute("d", QString("%1,%2")
                          .arg(v.toSize().width())
                          .arg(v.toSize().height()));
        break;
    case QVariant::Rect:
        sw.writeAttribute("d", QString("%1,%2,%3,%4")
                          .arg(v.toRect().x())
                          .arg(v.toRect().y())
                          .arg(v.toRect().width())
                          .arg(v.toRect().height()));
        break;
    default:
        break;
    }

    sw.writeEndElement();
}

QVariant DataObjects::QVariantHelpers::ReadXml(QXmlStreamReader &sr)
        throw(GUtil::Core::XmlException)
{
    QVariant ret;

    if(sr.readNextStartElement())
    {
        if(sr.name() != QVARIANT_XML_ID)
            throw Core::XmlException(QString("Unrecognized XML node: %1")
                                     .arg(sr.name().toString()).toStdString());

        QVariant::Type type = (QVariant::Type)sr.attributes().at(0).value().toString().toInt();

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
        case QVariant::String:
            ret.setValue(Utils::QStringHelpers::fromBase64(d));
            break;
        case QVariant::ByteArray:
            tmps = Utils::QStringHelpers::fromBase64(d);
            ret.setValue(QByteArray(tmps.toStdString().c_str(), tmps.length()));
            break;
        case QVariant::Char:
            ret.setValue(Utils::QStringHelpers::fromBase64(d).at(0));
            break;
        case QVariant::Int:
            ret.setValue(d.toInt());
            break;
        case QVariant::UInt:
            ret.setValue(d.toUInt());
            break;
        case QVariant::Bool:
            ret.setValue(d == "1" ? true : false);
            break;
        case QVariant::Double:
            ret.setValue(d.toDouble());
            break;
        case QVariant::Date:
            sltemp1 = d.split(",");
            ret.setValue(QDate(sltemp1.at(0).toInt(),
                           sltemp1.at(1).toInt(),
                           sltemp1.at(2).toInt()));
            break;
        case QVariant::Time:
            sltemp1 = d.split(",");
            ret.setValue(QTime(sltemp1.at(0).toInt(),
                           sltemp1.at(1).toInt(),
                           sltemp1.at(2).toInt(),
                           sltemp1.at(3).toInt()));
            break;
        case QVariant::DateTime:
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
        case QVariant::BitArray:
            baBitArray.resize(d.length());
            for(int i = 0; i < d.length(); i++)
                baBitArray.setBit(d.length() - i - 1, d.at(i) == '1' ? true : false);
            ret.setValue(baBitArray);
            break;
        case QVariant::StringList:
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
        case QVariant::RegExp:
            sltemp1 = sr.attributes().at(2).value().toString().split(",");
            ret.setValue(QRegExp(d, (Qt::CaseSensitivity)sltemp1.at(0).toInt(),
                             (QRegExp::PatternSyntax)sltemp1.at(1).toInt()));
            break;
        case QVariant::Url:
            ret.setValue(QUrl(d));
            break;
        case QVariant::List:
            tmpint = d.toInt();
            for(int i = 0; i < tmpint; i++)
                vl.append(ReadXml(sr));
            ret.setValue(vl);
            break;
        case QVariant::Map:
            tmpint = d.toInt();
            for(int i = 0; i < tmpint; i++)
            {
                if(!sr.readNextStartElement())
                    throw Core::XmlException();

                QString key = Utils::QStringHelpers::fromBase64(
                        sr.attributes().at(0).value().toString());

                vm.insert(key, ReadXml(sr));

                while(sr.readNext() != QXmlStreamReader::EndElement);
            }
            ret.setValue(vm);
            break;
        case QVariant::Size:
            sltemp1 = d.split(",");
            ret.setValue(QSize(sltemp1.at(0).toInt(), sltemp1.at(1).toInt()));
            break;
        case QVariant::Rect:
            sltemp1 = d.split(",");
            ret.setValue(QRect(sltemp1.at(0).toInt(),
                           sltemp1.at(1).toInt(),
                           sltemp1.at(2).toInt(),
                           sltemp1.at(3).toInt()));
            break;
        default:
            break;
        }

        while(sr.readNext() != QXmlStreamReader::EndElement ||
              sr.name() != QVARIANT_XML_ID);
    }

    return ret;
}
