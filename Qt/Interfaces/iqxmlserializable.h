/*Copyright 2010-2012 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_IQXMLSERIALIZABLE_H
#define GUTIL_IQXMLSERIALIZABLE_H

#include "gutil_macros.h"
#include <QString>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

NAMESPACE_GUTIL2(QT, Interfaces);


/** An abstract interface deriving from the core XML serializable class,
    which supports Qt Strings and XML classes.
*/
class IQXmlSerializable
{
public:

    /** Writes the class' data to the QXmlStreamWriter. */
    virtual void WriteXml(QXmlStreamWriter &) const = 0;

    /** Reads and populates the class' data from the QXmlStreamReader. */
    virtual void ReadXml(QXmlStreamReader &) = 0;


    /** Generates a QString of the class' XML data. */
    inline QString ToXmlQString(bool human_readable = false) const{
        QString ret;
        QXmlStreamWriter w(&ret);
        w.setAutoFormatting(human_readable);

        WriteXml(w);

        return ret;
    }

    /** Populates the class' data from the QString XML. */
    void FromXmlQString(const QString &xml){
        QXmlStreamReader r(xml);
        ReadXml(r);
    }

};


END_NAMESPACE_GUTIL2;

#endif // GUTIL_IQXMLSERIALIZABLE_H
