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

#ifndef XMLHELPERS_H
#define XMLHELPERS_H

#include <QXmlStreamReader>
#include <QXmlStreamWriter>

namespace GUtil
{
    namespace Custom
    {
        class myXmlStreamReader : public QXmlStreamReader
        {
        public:
            myXmlStreamReader();
            myXmlStreamReader(const QString &);
            myXmlStreamReader(QIODevice *);
            myXmlStreamReader(const QByteArray &);
            myXmlStreamReader(const char *);

            // Reads the inner Xml of the current start element, including itself
            //  and all children recursively.  It ends while pointing at the element's
            //  corresponding end element.
            QString InnerXml();

            // Only read the inner text, not including the node itself
            QString InnerText();

        private:
            void _read_inner_xml(QXmlStreamWriter &);
        };
    }
}

#endif // XMLHELPERS_H
