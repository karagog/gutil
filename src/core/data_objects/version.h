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

#ifndef GUTIL_VERSION_H
#define GUTIL_VERSION_H

#include <gutil/macros.h>
#include <gutil/strings.h>

NAMESPACE_GUTIL;


class Version
{
public:

    PROPERTY( MajorVersion, int );
    PROPERTY( MinorVersion, int );
    PROPERTY( ReleaseVersion, int );

    bool IsNull() const{
        return GetMajorVersion() == -1 && GetMinorVersion() == -1 && GetReleaseVersion() == -1;
    }
    String ToString() const{
        return String::Format("%d.%d.%d",
                              GetMajorVersion(),
                              GetMinorVersion(),
                              GetReleaseVersion());
    }

    // Useful operators for comparing versions
    bool operator < (const Version &o) const;
    bool operator > (const Version &o) const;

    bool operator == (const Version &o) const{
        return GetMajorVersion() == o.GetMajorVersion() &&
                GetMinorVersion() == o.GetMinorVersion() &&
                GetReleaseVersion() == o.GetReleaseVersion();
    }
    bool operator != (const Version &o) const{
        return !(*this == o);
    }

    Version()
        :_p_MajorVersion(-1), _p_MinorVersion(-1), _p_ReleaseVersion(-1) {}
    Version(int major, int minor, int release = 0)
        :_p_MajorVersion(major), _p_MinorVersion(minor), _p_ReleaseVersion(release){}

    Version(const String &version_string);
    Version(const char *version_string);

    Version(const Version &o)
        :_p_MajorVersion(o._p_MajorVersion),
          _p_MinorVersion(o._p_MinorVersion),
          _p_ReleaseVersion(o._p_ReleaseVersion)
    {}


    Version &operator = (const Version &o){
        new(this) Version(o);
        return *this;
    }
    Version &operator = (const String &s){
        new(this) Version(s);
        return *this;
    }
    Version &operator = (const char *s){
        new(this) Version(s);
        return *this;
    }

    operator String () const{ return ToString(); }

};


END_NAMESPACE_GUTIL;

#endif // GUTIL_VERSION_H
