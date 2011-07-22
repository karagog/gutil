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

#ifndef GVARIANT_H
#define GVARIANT_H

#include "Interfaces/iqxmlserializable.h"
#include "Core/Interfaces/iequatable.h"
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

GUTIL_BEGIN_NAMESPACE( Custom );


class GVariant;

// A convenient typedef
typedef QList< GVariant > GVariantList;


class GVariant :
        public QVariant,
        public Interfaces::IQXmlSerializable,
        public Core::Interfaces::IEquatable<GVariant>
{
public:
    GVariant();
    GVariant(const QVariant &);
    GVariant(Type);
    GVariant(const std::string &);
    GVariant(const QString &);
    GVariant(const QByteArray &);
    GVariant(const QChar &);
    GVariant(int);
    GVariant(uint);
    GVariant(long long);
    GVariant(unsigned long long);
    GVariant(char *);
    GVariant(const char *);
    GVariant(bool);
    GVariant(double);
    GVariant(float);
    GVariant(const QDate &);
    GVariant(const QTime &);
    GVariant(const QDateTime &);
    GVariant(const QBitArray &);
    GVariant(const QRegExp &);
    GVariant(const QUrl &);
    GVariant(const QRect &);
    GVariant(const QSize &);
    GVariant(const QStringList &);
    GVariant(const QVariantList &);
    GVariant(const QVariantMap &);
    GVariant(const QUuid &);

    QUuid toUuid() const;

    void WriteXml(QXmlStreamWriter &) const;
    void ReadXml(QXmlStreamReader &)
            throw(GUtil::Core::XmlException);

    static void ToXml(const GVariant &, QXmlStreamWriter &);
    static GVariant FromXml(QXmlStreamReader &);

    static QString ConvertToXmlQString(const GVariant &, bool human_readable = false);
    static GVariant ConvertFromXmlQString(const QString &);


    virtual bool Equals(const GVariant &) const;

    // A special (redundant) operator to prevent ambiguity when comparing with
    //  other GVariants
    bool operator == (const QVariant &) const;
    bool operator != (const QVariant &) const;

    // In this way, we grant the observable collection permission to be referenced
    //  by us; we'll guarantee that these functions will call their version of it
    virtual GVariant &operator =(const GVariant &o){return (GVariant &)(*((QVariant *)this) = o);}
    virtual void clear(){QVariant::clear();}
    virtual void convert(Type t){QVariant::convert(t);}

    // In this function we go through our equals operator, because template functions can't
    //  be virtual
    template <class T> inline void setValue(const T &value){ *this = value; }



    // These are types that I define, because for some reason Qt doesn't already
    static int TypeQUuid;
    static int TypeFloat;

};


GUTIL_END_NAMESPACE;


// Register these types with the Qt meta-type system so we can use them with QVariants
Q_DECLARE_METATYPE(GUtil::Custom::GVariant);
Q_DECLARE_METATYPE(QUuid);


#endif // GVARIANT_H
