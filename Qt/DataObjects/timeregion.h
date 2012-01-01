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

#ifndef GUTIL_TIMEREGION_H
#define GUTIL_TIMEREGION_H

#include "gutil_macros.h"
#include "Qt/DataObjects/gdatetime.h"
#include "Core/Utils/linearsetalgebra.h"

NAMESPACE_GUTIL2(QT, DataObjects);


class TimeRange :
        public GUtil::Utils::Range<GDateTime>
{
public:

    inline TimeRange(long long msec_resolution)
        :Range<GDateTime>(GDateTime::Origin().addMSecs(msec_resolution))
    {}
    inline TimeRange(const QDateTime &lower_bound,
              const QDateTime &upper_bound,
              long long msec_resolution)
        :Range<GDateTime>(lower_bound, upper_bound,
              GDateTime::Origin().addMSecs(msec_resolution))
    {}

};


class TimeRegion :
        public GUtil::Utils::Region<GDateTime>
{
public:
    TimeRegion(long long msec_resolution);
    TimeRegion(const GUtil::Utils::Range<GDateTime> &r)
        :Region<GDateTime>(r)
    {}
};


END_NAMESPACE_GUTIL2;

#endif // GUTIL_TIMEREGION_H
