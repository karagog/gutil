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
#include <QVariant>

namespace GUtil
{
    namespace Custom
    {
        class GVariant :    public QVariant,
                            public GUtil::Interfaces::IQXmlSerializable
        {
        public:
            GVariant();
            GVariant(const QVariant &);
            GVariant(Type);
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

            void WriteXml(QXmlStreamWriter &) const;
            void ReadXml(QXmlStreamReader &)
                    throw(GUtil::Core::XmlException);
        };
    }
}

#endif // GVARIANT_H
