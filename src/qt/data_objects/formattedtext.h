/*Copyright 2010-2013 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_FORMATTEDTEXT_H
#define GUTIL_FORMATTEDTEXT_H

#ifndef GUTIL_NO_GUI_FUNCTIONALITY

#include "gutil_macros.h"
#include <QString>
#include <QFont>

NAMESPACE_GUTIL2(QT, DataObjects);


/** A convenience class that pairs a piece of text with the font with which to format it. */
class FormattedText
{
public:

    /** The text string */
    QString Text;

    /** The font which should be associated with the Text (implementation defined) */
    QFont   Font;

    FormattedText(){}
    FormattedText(const QString &s) :Text(s) {}
    FormattedText(const QString &s, const QFont &f)
        :Text(s), Font(f) {}

    /** Can convert seamlessly to a QString */
    operator QString () const{ return Text; }

};


END_NAMESPACE_GUTIL2;

#endif // GUI_FUNCTIONALITY

#endif // GUTIL_FORMATTEDTEXT_H
