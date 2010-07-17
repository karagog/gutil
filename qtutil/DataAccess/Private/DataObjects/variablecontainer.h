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
                class VariableContainer : public Interfaces::IXmlSerializable
                {
                public:
                    virtual QByteArray toXml();
                    virtual void fromXml(const QByteArray &);

                    void setValue(const QString &, const QString &);
                    QString getValue(const QString &);

                    bool remove(const QString &);

                    bool contains(const QString &);
                    void clear();

                    QString &at(const QString &) const;
                    QString &operator [](const QString &) const;

                private:
                    QMap<QString, QString> _values;
                };
            }
        }
    }
}

#endif // VARIABLECONTAINER_H
