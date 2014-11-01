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

#include <gutil/macros.h>
#include <gutil/exception.h>
#include <gutil/iqxmlserializable.h>
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
#include <QString>

#ifdef GUTIL_STL
#include <string>
#endif

NAMESPACE_GUTIL1(Qt);


/** A class derived from QVariant to extend its functionality.

    Most notably there is an Xml Serializable interface.
*/
class Variant :
        public QVariant,
        public IQXmlSerializable
{
public:
    Variant() {}
    Variant(const QVariant &i) :QVariant(i) {}
    Variant(Type i) :QVariant(i) {}
    Variant(const QString &i) :QVariant(i) {}
    Variant(const QByteArray &i) :QVariant(i) {}
    Variant(const QChar &i) :QVariant(i) {}
    Variant(int i) :QVariant(i) {}
    Variant(uint i) :QVariant(i) {}
    Variant(long long i) :QVariant(i) {}
    Variant(unsigned long long i) :QVariant(i) {}
    Variant(char *i) :QVariant(i) {}
    Variant(const char *i) :QVariant(i) {}
    Variant(bool i) :QVariant(i) {}
    Variant(double i) :QVariant(i) {}
    Variant(float i) :QVariant(i) {}
    Variant(const QDate &i) :QVariant(i) {}
    Variant(const QTime &i) :QVariant(i) {}
    Variant(const QDateTime &i) :QVariant(i) {}
    Variant(const QBitArray &i) :QVariant(i) {}
    Variant(const QRegExp &i) :QVariant(i) {}
    Variant(const QUrl &i) :QVariant(i) {}
    Variant(const QRect &i) :QVariant(i) {}
    Variant(const QSize &i) :QVariant(i) {}
    Variant(const QStringList &i) :QVariant(i) {}
    Variant(const QVariantList &i) :QVariant(i) {}
    Variant(const QVariantMap &i) :QVariant(i) {}
    Variant(const QUuid &i) :QVariant(qVariantFromValue(i)) {}

#   ifdef GUTIL_STL
    Variant(const std::string &i) :QVariant(QString::fromStdString(i)) {}
#   endif
    
    /** Destructs the Variant. */
    virtual ~Variant();

    /** Converts this Variant's contents to a QUuid. */
    QUuid toUuid() const;

    /** Satisfies the IQXmlSerializeable interface. */
    virtual void WriteXml(QXmlStreamWriter &) const;
    /** Satisfies the IQXmlSerializeable interface. */
    virtual void ReadXml(QXmlStreamReader &);

    static void ToXml(const Variant &g, QXmlStreamWriter &sw){
        g.WriteXml(sw);
    }
    static Variant FromXml(QXmlStreamReader &sr){
        Variant ret;
        ret.ReadXml(sr);
        return ret;
    }

    static QString ConvertToXmlQString(const Variant &, bool human_readable = false);
    static Variant ConvertFromXmlQString(const QString &);

    bool operator == (const Variant &) const;
    bool operator != (const Variant &o) const{ return !operator == (o); }

};


END_NAMESPACE_GUTIL1;


// Register these types with the Qt meta-type system so we can use them with QVariants
Q_DECLARE_METATYPE(GUtil::Qt::Variant);
Q_DECLARE_METATYPE(QUuid);
Q_DECLARE_METATYPE(float);


#endif // GUTIL_VARIANT_H
