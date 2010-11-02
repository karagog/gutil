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

#ifndef IQXMLSERIALIZABLE_H
#define IQXMLSERIALIZABLE_H

#include "Core/Interfaces/ixmlserializable.h"

class QXmlStreamWriter;
class QXmlStreamReader;
class QString;
class QByteArray;

namespace GUtil
{
    namespace Interfaces
    {
        class IQXmlSerializable : public GUtil::Core::Interfaces::IXmlSerializable
        {
        public:

            // Implement these to have a consistent xml interface across objects
            virtual void WriteXml(QXmlStreamWriter &) const = 0;
            virtual void ReadXml(QXmlStreamReader &)
                    throw(GUtil::Core::XmlException) = 0;

            virtual QString ToXmlQString() const;
            virtual void FromXmlQString(const QString &)
                    throw(GUtil::Core::XmlException);

            // These functions satisfy the core ixmlserializable interface,
            //   building off the new interface's functions
            virtual std::string ToXmlString() const;
            virtual void FromXmlString(const std::string &)
                    throw(GUtil::Core::XmlException);

            void CopyIQXmlInterface(const IQXmlSerializable &);

        protected:

            IQXmlSerializable(bool human_readable = false);

        };
    }
}

#endif // IQXMLSERIALIZABLE_H
