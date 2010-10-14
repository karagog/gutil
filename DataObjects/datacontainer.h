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
#include "Core/Interfaces/ireadonlyobject.h"
#include <QMap>
#include <QString>
#include <QByteArray>
#include <QStringList>

namespace GUtil
{
    namespace DataObjects
    {
        // A class used to hold data and serialize
        //   the values to xml or access them conveniently with string keys
        class DataContainer :   public Core::Interfaces::IXmlSerializable,
                                public Core::Interfaces::IReadOnlyObject
        {
        public:
            DataContainer();
            DataContainer(const DataContainer &);

            virtual std::string toXml();
            virtual void fromXml(const std::string &) throw(GUtil::Core::XmlException);

            void setValue(const QString &, const QByteArray &);
            QByteArray getValue(const QString &) const;

            bool remove(const QString &);

            bool contains(const QString &);
            void clear();

            QStringList keys() const;

        protected:
            virtual std::string ReadonlyMessageIdentifier() const;

        private:
            QMap<QString, QByteArray> _data;

        };
    }
}

#endif // VARIABLECONTAINER_H
