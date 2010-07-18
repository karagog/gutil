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

#include <QMap>
#include <QString>
#include <QByteArray>
#include <Interfaces/ixmlserializable.h>

namespace GQtUtil
{
    namespace DataAccess
    {
        namespace Private
        {
            namespace DataObjects
            {
                // A class used to hold data and serialize
                //   the values to xml or access them conveniently with string keys
                class DataContainer : public Interfaces::IXmlSerializable
                {
                public:
                    virtual QByteArray toXml();
                    virtual void fromXml(const QByteArray &);

                    void setValue(const QString &, const QByteArray &);
                    QByteArray getValue(const QString &);

                    bool remove(const QString &);

                    bool contains(const QString &);
                    void clear();

                    QByteArray &at(const QString &);
                    QByteArray &operator [](const QString &);

                private:
                    QMap<QString, QByteArray> _values;
                };
            }
        }
    }
}

#endif // VARIABLECONTAINER_H
