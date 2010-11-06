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

#ifndef DATATUPLE_H
#define DATATUPLE_H

#include "Interfaces/icollection.h"
#include "Interfaces/iqxmlserializable.h"
#include <QVariant>

namespace GUtil
{
    namespace DataObjects
    {
        class DataTuple :   public Interfaces::ICollection<QVariant>,
                            public Interfaces::IQXmlSerializable
        {
        public:
            DataTuple(int size = 0);
            DataTuple(const DataTuple &);
            virtual ~DataTuple();

            virtual void WriteXml(QXmlStreamWriter &) const;
            virtual void ReadXml(QXmlStreamReader &)
                    throw(Core::XmlException);

        };
    }
}

#endif // DATATUPLE_H
