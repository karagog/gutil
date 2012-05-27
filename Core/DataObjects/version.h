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

#ifndef GUTIL_VERSION_H
#define GUTIL_VERSION_H

#include "gutil_macros.h"
#include "gutil_strings.h"

NAMESPACE_GUTIL1(DataObjects);


class Version
{
public:

    PROPERTY( MajorVersion, int );
    PROPERTY( MinorVersion, int );
    PROPERTY( ReleaseVersion, int );

    inline bool IsNull() const{
        return GetMajorVersion() == -1 && GetMinorVersion() == -1 && GetReleaseVersion() == -1;
    }
    inline String ToString() const{
        return String::Format("%d.%d.%d",
                              GetMajorVersion(),
                              GetMinorVersion(),
                              GetReleaseVersion());
    }

    // Useful operators for comparing versions
    bool operator < (const Version &o) const;
    bool operator > (const Version &o) const;

    inline bool operator == (const Version &o) const{
        return GetMajorVersion() == o.GetMajorVersion() &&
                GetMinorVersion() == o.GetMinorVersion() &&
                GetReleaseVersion() == o.GetReleaseVersion();
    }
    inline bool operator != (const Version &o) const{
        return !(*this == o);
    }

    inline Version()
        :_p_MajorVersion(-1), _p_MinorVersion(-1), _p_ReleaseVersion(-1) {}
    inline Version(int major, int minor, int release = 0)
        :_p_MajorVersion(major), _p_MinorVersion(minor), _p_ReleaseVersion(release){}
    Version(const String &version_string);
    inline Version(const Version &o)
        :_p_MajorVersion(o._p_MajorVersion),
          _p_MinorVersion(o._p_MinorVersion),
          _p_ReleaseVersion(o._p_ReleaseVersion)
    {}


    inline Version &operator = (const Version &o){
        new(this) Version(o);
        return *this;
    }
    inline Version &operator = (const String &s){
        new(this) Version(s);
        return *this;
    }

    inline operator String () const{ return ToString(); }

};


END_NAMESPACE_GUTIL1;

#endif // GUTIL_VERSION_H
