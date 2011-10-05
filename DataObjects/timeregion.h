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

#ifndef TIMEREGION_H
#define TIMEREGION_H

#include "gutil.h"
#include "Custom/gdatetime.h"
#include "Core/Utils/linearsetalgebra.h"

GUTIL_BEGIN_NAMESPACE(DataObjects);


class TimeRange :
        public Core::Utils::Range<Custom::GDateTime>
{
public:

    TimeRange(long long msec_resolution)
        :Core::Utils::Range<Custom::GDateTime>(Custom::GDateTime::Origin().addMSecs(msec_resolution))
    {}
    TimeRange(const QDateTime &lower_bound,
              const QDateTime &upper_bound,
              long long msec_resolution)
        :Core::Utils::Range<Custom::GDateTime>(lower_bound, upper_bound,
              Custom::GDateTime::Origin().addMSecs(msec_resolution))
    {}

};


class TimeRegion :
        public Core::Utils::Region<Custom::GDateTime>
{
public:
    TimeRegion(long long msec_resolution);
    TimeRegion(const Core::Utils::Range<Custom::GDateTime> &r)
        :Core::Utils::Region<Custom::GDateTime>(r)
    {}
};


GUTIL_END_NAMESPACE;

#endif // TIMEREGION_H
