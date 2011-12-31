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

#include "version.h"

NAMESPACE_GUTIL1(DataObjects);


Version::Version(const String &version_string)
    :_p_MajorVersion(-1),
      _p_MinorVersion(-1),
      _p_ReleaseVersion(-1)
{
    if(!version_string.IsEmpty())
    {
        int tmp;
        bool ok(false);
        Vector<String> lst( version_string.Split('.') );
        SetMajorVersion(0);
        SetMinorVersion(0);
        SetReleaseVersion(0);

        if(lst.Count() > 0)
        {
            tmp = lst[0].ToInt(&ok);
            if(ok) SetMajorVersion(tmp);
        }
        if(lst.Count() > 1)
        {
            tmp = lst[1].ToInt(&ok);
            if(ok) SetMinorVersion(tmp);
        }
        if(lst.Count() > 2)
        {
            tmp = lst[2].ToInt(&ok);
            if(ok) SetReleaseVersion(tmp);
        }
    }
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
