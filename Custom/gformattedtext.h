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

#ifndef GFORMATTEDTEXT_H
#define GFORMATTEDTEXT_H

#include "gutil_macros.h"
#include <QString>
#include <QFont>

GUTIL_BEGIN_NAMESPACE(Custom);

class GFormattedText
{
public:

    QString Text;
    QFont   Font;

    inline GFormattedText(const QString &s = QString(), const QFont &f = QFont())
        :Text(s),
          Font(f)
    {}

};

GUTIL_END_NAMESPACE;

#endif // GFORMATTEDTEXT_H