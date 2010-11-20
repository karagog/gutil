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
    GVariant(char *);
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
    GVariant(const GVariant &);
    GVariant(const GVariantList &);


    // Here are various cast operators, for convenience:

    // Primitive types throw an InvalidCast exception if cast fails
    operator int () const;
    operator const char *() const;
    operator bool () const;
    operator float () const;
    operator double () const;
    operator std::string() const;
    operator QString() const;
    operator QByteArray() const;
    operator QChar() const;
    operator QTime() const;
    operator QDateTime() const;
    operator QBitArray() const;
    operator QRegExp() const;
    operator QUrl() const;
    operator QRect() const;
    operator QSize() const;
    operator QStringList() const;
    operator QVariantList() const;
    operator QVariantMap() const;


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

};


GUTIL_END_NAMESPACE;


// Register these types with the Qt meta-type system
Q_DECLARE_METATYPE(GUtil::Custom::GVariant);
Q_DECLARE_METATYPE(GUtil::Custom::GVariantList);

#endif // GVARIANT_H
