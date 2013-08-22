/*Copyright 2010-2013 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_VARIANT_H
#define GUTIL_VARIANT_H

#include "gutil_macros.h"
#include "gutil_exception.h"
#include "gutil_iqxmlserializable.h"
#include <QVariant>
#include <QBitArray>
#include <QRegExp>
#include <QStringList>
#include <QUrl>
#include <QSize>
#include <QRect>
#include <QVariantMap>
#include <QDateTime>
#include <QUuid>
#include <QByteArray>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

NAMESPACE_GUTIL2(QT, DataObjects);


/** A class derived from QVariant to extend its functionality.

    Most notably there is an Xml Serializable interface.
*/
class Variant :
        public QVariant,
        public Interfaces::IQXmlSerializable
{
public:
    inline Variant() {}
    inline Variant(const QVariant &i) :QVariant(i) {}
    inline Variant(Type i) :QVariant(i) {}
    inline Variant(const std::string &i) :QVariant(QString::fromStdString(i)) {}
    inline Variant(const QString &i) :QVariant(i) {}
    inline Variant(const QByteArray &i) :QVariant(i) {}
    inline Variant(const QChar &i) :QVariant(i) {}
    inline Variant(int i) :QVariant(i) {}
    inline Variant(uint i) :QVariant(i) {}
    inline Variant(long long i) :QVariant(i) {}
    inline Variant(unsigned long long i) :QVariant(i) {}
    inline Variant(char *i) :QVariant(i) {}
    inline Variant(const char *i) :QVariant(i) {}
    inline Variant(bool i) :QVariant(i) {}
    inline Variant(double i) :QVariant(i) {}
    inline Variant(float i) :QVariant(i) {}
    inline Variant(const QDate &i) :QVariant(i) {}
    inline Variant(const QTime &i) :QVariant(i) {}
    inline Variant(const QDateTime &i) :QVariant(i) {}
    inline Variant(const QBitArray &i) :QVariant(i) {}
    inline Variant(const QRegExp &i) :QVariant(i) {}
    inline Variant(const QUrl &i) :QVariant(i) {}
    inline Variant(const QRect &i) :QVariant(i) {}
    inline Variant(const QSize &i) :QVariant(i) {}
    inline Variant(const QStringList &i) :QVariant(i) {}
    inline Variant(const QVariantList &i) :QVariant(i) {}
    inline Variant(const QVariantMap &i) :QVariant(i) {}
    inline Variant(const QUuid &i) :QVariant(qVariantFromValue(i)) {}
    
    /** Destructs the Variant. */
    virtual ~Variant();

    /** Converts this Variant's contents to a QUuid. */
    QUuid toUuid() const;

    /** Satisfies the IQXmlSerializeable interface. */
    virtual void WriteXml(QXmlStreamWriter &) const;
    /** Satisfies the IQXmlSerializeable interface. */
    virtual void ReadXml(QXmlStreamReader &);

    inline static void ToXml(const Variant &g, QXmlStreamWriter &sw){
        g.WriteXml(sw);
    }
    inline static Variant FromXml(QXmlStreamReader &sr){
        Variant ret;
        ret.ReadXml(sr);
        return ret;
    }

    static QString ConvertToXmlQString(const Variant &, bool human_readable = false);
    static Variant ConvertFromXmlQString(const QString &);

    bool operator == (const Variant &) const;
    inline bool operator != (const Variant &o) const{ return !operator == (o); }

};


END_NAMESPACE_GUTIL2;


// Register these types with the Qt meta-type system so we can use them with QVariants
Q_DECLARE_METATYPE(GUtil::QT::DataObjects::Variant);
Q_DECLARE_METATYPE(QUuid);
Q_DECLARE_METATYPE(float);


#endif // GUTIL_VARIANT_H