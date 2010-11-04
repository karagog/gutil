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

#ifndef QVARIANTHELPERS_H
#define QVARIANTHELPERS_H

#include <QVariant>

class QXmlStreamReader;
class QXmlStreamWriter;

namespace GUtil
{
    namespace DataObjects
    {
        class QVariantHelpers
        {
        public:

            static QString ConvertToXmlQString(const QVariant &, bool human_readable = false);
            static QVariant ConvertFromXmlQString(const QString &);

        private:
            static void _read_xml(QXmlStreamReader &);
            static void _write_xml(const QVariant &, QXmlStreamWriter &);
        };
    }
}

#endif // GVARIANT_H
