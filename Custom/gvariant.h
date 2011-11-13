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
#include <QByteArray>

GUTIL_BEGIN_NAMESPACE( Custom );


class GVariant;

/** A typedef which defines a vector of GVariants. */
typedef GUtil::Core::DataObjects::Vector<GVariant>
        GVariantVector;

/** A typedef which defines a Map of String to GVariant. */
typedef GUtil::Core::DataObjects::Map<GUtil::Core::DataObjects::String, GVariant>
        GVariantMap;



/** A class derived from QVariant to extend its functionality.

    Most notably there is an Xml Serializable interface.
*/
class GVariant :
        public QVariant,
        public Interfaces::IQXmlSerializable
{
public:
    inline GVariant() {}
    inline GVariant(const QVariant &i) :QVariant(i) {}
    inline GVariant(Type i) :QVariant(i) {}
    inline GVariant(const std::string &i) :QVariant(QString::fromStdString(i)) {}
    inline GVariant(const QString &i) :QVariant(i) {}
    inline GVariant(const QByteArray &i) :QVariant(i) {}
    inline GVariant(const QChar &i) :QVariant(i) {}
    inline GVariant(int i) :QVariant(i) {}
    inline GVariant(uint i) :QVariant(i) {}
    inline GVariant(long long i) :QVariant(i) {}
    inline GVariant(unsigned long long i) :QVariant(i) {}
    inline GVariant(char *i) :QVariant(i) {}
    inline GVariant(const char *i) :QVariant(i) {}
    inline GVariant(bool i) :QVariant(i) {}
    inline GVariant(double i) :QVariant(i) {}
    inline GVariant(float i) :QVariant(i) {}
    inline GVariant(const QDate &i) :QVariant(i) {}
    inline GVariant(const QTime &i) :QVariant(i) {}
    inline GVariant(const QDateTime &i) :QVariant(i) {}
    inline GVariant(const QBitArray &i) :QVariant(i) {}
    inline GVariant(const QRegExp &i) :QVariant(i) {}
    inline GVariant(const QUrl &i) :QVariant(i) {}
    inline GVariant(const QRect &i) :QVariant(i) {}
    inline GVariant(const QSize &i) :QVariant(i) {}
    inline GVariant(const QStringList &i) :QVariant(i) {}
    inline GVariant(const QVariantList &i) :QVariant(i) {}
    inline GVariant(const QVariantMap &i) :QVariant(i) {}
    inline GVariant(const QUuid &i) :QVariant(qVariantFromValue(i)) {}

    /** Converts this GVariant's contents to a QUuid. */
    QUuid toUuid() const;

    void WriteXml(QXmlStreamWriter &) const;
    void ReadXml(QXmlStreamReader &)
            throw(GUtil::Core::XmlException<>);

    static void ToXml(const GVariant &, QXmlStreamWriter &);
    static GVariant FromXml(QXmlStreamReader &);

    static QString ConvertToXmlQString(const GVariant &, bool human_readable = false);
    static GVariant ConvertFromXmlQString(const QString &);

    bool operator == (const GVariant &) const;
    inline bool operator != (const GVariant &o) const{ return !operator == (o); }

};


GUTIL_END_NAMESPACE;


// Register these types with the Qt meta-type system so we can use them with QVariants
Q_DECLARE_METATYPE(GUtil::Custom::GVariant);
Q_DECLARE_METATYPE(QUuid);
Q_DECLARE_METATYPE(float);


#endif // GVARIANT_H
