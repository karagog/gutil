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

#ifndef GUTIL_VERSION_H
#define GUTIL_VERSION_H

#include "gutil.h"
#include <QStringList>

GUTIL_BEGIN_NAMESPACE(DataObjects);


class Version
{

public:

    PROPERTY( MajorVersion, int );
    PROPERTY( MinorVersion, int );
    PROPERTY( ReleaseVersion, int );

    bool IsNull() const;
    QString ToString() const;

    // Useful operators for comparing versions
    bool operator < (const Version &o) const;
    bool operator > (const Version &o) const;

    bool operator == (const Version &o) const;
    bool operator != (const Version &o) const;

    Version(const QString &version_string = QString::null);
    Version(const Version &);

    Version &operator =(const Version &o);
    inline operator QString () const{ return ToString(); }

};


GUTIL_END_NAMESPACE;

#endif // GUTIL_VERSION_H
