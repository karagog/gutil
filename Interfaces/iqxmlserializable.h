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

#ifndef IQXMLSERIALIZABLE_H
#define IQXMLSERIALIZABLE_H

#include "Core/Interfaces/ixmlserializable.h"
#include <QString>

class QXmlStreamWriter;
class QXmlStreamReader;

namespace GUtil
{
    namespace Interfaces
    {
        /** An abstract interface deriving from the core XML serializable class,
            which supports Qt Strings and XML classes.
        */
        class IQXmlSerializable :
                public GUtil::Core::Interfaces::IXmlSerializable
        {
        public:

            /** Writes the class' data to the QXmlStreamWriter. */
            virtual void WriteXml(QXmlStreamWriter &) const = 0;

            /** Reads and populates the class' data from the QXmlStreamReader. */
            virtual void ReadXml(QXmlStreamReader &)
                    throw(GUtil::Core::XmlException<>) = 0;

            /** Generates a QString of the class' XML data. */
            QString ToXmlQString(bool human_readable = false) const;

            /** Populates the class' data from the QString XML. */
            void FromXmlQString(const QString &)
                    throw(GUtil::Core::XmlException<>);


            /** Satisfies the base class' abstract interface. */
            Core::DataObjects::String ToXmlString(bool human_readable = false) const;

            /** Satisfies the base class' abstract interface. */
            void FromXmlString(const Core::DataObjects::String &)
                    throw(GUtil::Core::XmlException<>);

        };
    }
}

#endif // IQXMLSERIALIZABLE_H
