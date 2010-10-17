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

#ifndef VARIABLECONTAINER_H
#define VARIABLECONTAINER_H

#include "Core/Interfaces/ixmlserializable.h"
#include <QMap>
#include <QByteArray>

namespace GUtil
{
    namespace DataAccess
    {
        namespace DataObjects
        {
            // A class used to hold data and serialize
            //   the values to xml or access them conveniently with string keys
            class DataContainer :   public QMap<QString, QByteArray>,
                                    public Core::Interfaces::IXmlSerializable
            {
            public:
                DataContainer();
                DataContainer(const DataContainer &);

                virtual std::string ToXml();
                virtual void FromXml(const std::string &) throw(GUtil::Core::XmlException);

            };
        }
    }
}

#endif // VARIABLECONTAINER_H
