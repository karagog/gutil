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

#include "version.h"


/** Constructs a new Version object with the given string */
static void __init_with_string(::GUtil::DataObjects::Version *,
                               const ::GUtil::DataObjects::String &);


NAMESPACE_GUTIL1(DataObjects);


Version::Version(const String &version_string)
{
    __init_with_string(this, version_string);
}

Version::Version(const char *version_string)
{
    __init_with_string(this, String(version_string));
}

bool Version::operator <(const Version &o) const
{
    return GetMajorVersion() < o.GetMajorVersion() ||
            (GetMajorVersion() == o.GetMajorVersion() &&
             (GetMinorVersion() < o.GetMinorVersion() ||
              (GetMinorVersion() == o.GetMinorVersion() &&
               (GetReleaseVersion() < o.GetReleaseVersion()))));
}

bool Version::operator >(const Version &o) const
{
    return GetMajorVersion() > o.GetMajorVersion() ||
            (GetMajorVersion() == o.GetMajorVersion() &&
             (GetMinorVersion() > o.GetMinorVersion() ||
              (GetMinorVersion() == o.GetMinorVersion() &&
               (GetReleaseVersion() > o.GetReleaseVersion()))));
}


END_NAMESPACE_GUTIL1;


void __init_with_string(::GUtil::DataObjects::Version *v,
                        const ::GUtil::DataObjects::String &s)
{
    v->SetMajorVersion(-1);
    v->SetMinorVersion(-1);
    v->SetReleaseVersion(-1);

    // If s is not an empty string
    if(!s.IsEmpty())
    {
        int tmp;
        bool ok(false);
        ::GUtil::DataObjects::Vector< ::GUtil::DataObjects::String > lst( s.Split('.') );
        v->SetMajorVersion(0);
        v->SetMinorVersion(0);
        v->SetReleaseVersion(0);

        if(lst.Length() > 0)
        {
            tmp = lst[0].ToInt(&ok);
            if(ok) v->SetMajorVersion(tmp);
        }
        if(lst.Length() > 1)
        {
            tmp = lst[1].ToInt(&ok);
            if(ok) v->SetMinorVersion(tmp);
        }
        if(lst.Length() > 2)
        {
            tmp = lst[2].ToInt(&ok);
            if(ok) v->SetReleaseVersion(tmp);
        }
    }
}
