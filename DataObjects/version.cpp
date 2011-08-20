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
GUTIL_USING_NAMESPACE(DataObjects);

Version::Version(const QString &version_string)
    :_p_MajorVersion(-1),
      _p_MinorVersion(-1),
      _p_ReleaseVersion(-1)
{
    if(!version_string.isNull())
    {
        int tmp;
        bool ok(false);
        QStringList lst( version_string.split('.') );
        SetMajorVersion(0);
        SetMinorVersion(0);
        SetReleaseVersion(0);

        if(lst.count() > 0)
        {
            tmp = lst.at(0).toInt(&ok);
            if(ok) SetMajorVersion(tmp);
        }
        if(lst.count() > 1)
        {
            tmp = lst.at(1).toInt(&ok);
            if(ok) SetMinorVersion(tmp);
        }
        if(lst.count() > 2)
        {
            tmp = lst.at(2).toInt(&ok);
            if(ok) SetReleaseVersion(tmp);
        }
    }
}

Version::Version(const Version &v)
{
    *this = v;
}

bool Version::IsNull() const
{
    return GetMajorVersion() == -1 && GetMinorVersion() == -1 && GetReleaseVersion() == -1;
}

QString Version::ToString() const
{
    return QString("%1.%2.%3")
            .arg(GetMajorVersion())
            .arg(GetMinorVersion())
            .arg(GetReleaseVersion());
}

Version &Version::operator =(const Version &o)
{
    SetMajorVersion( o.GetMajorVersion() );
    SetMinorVersion( o.GetMinorVersion() );
    SetReleaseVersion( o.GetReleaseVersion() );
    return *this;
}

bool Version::operator ==(const Version &o) const
{
    return GetMajorVersion() == o.GetMajorVersion() &&
            GetMinorVersion() == o.GetMinorVersion() &&
            GetReleaseVersion() == o.GetReleaseVersion();
}

bool Version::operator !=(const Version &o) const
{
    return !(*this == o);
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
