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

#include "datetime.h"
NAMESPACE_GUTIL1(QT);


DateTime::TimeBreakdown DateTime::GetTimeDistanceBreakdown(const DateTime &start_time,
                                                             const DateTime &end_time,
                                                             TimeBreakdownFlags f)
{
    TimeBreakdown ret;
    long long msecs( start_time.msecsTo(end_time) );

    if(f.TestFlag(Millennia))
        ret.Millennia = _subtract_time_units(msecs, (long long)1000 * 1000 * YEAR_IN_SECONDS);
    if(f.TestFlag(Centuries))
        ret.Centuries = _subtract_time_units(msecs, (long long)100 * 1000 * YEAR_IN_SECONDS);
    if(f.TestFlag(Decades))
        ret.Decades = _subtract_time_units(msecs, (long long)10 * 1000 * YEAR_IN_SECONDS);
    if(f.TestFlag(Years))
        ret.Years = _subtract_time_units(msecs, (long long)1000 * YEAR_IN_SECONDS);
    if(f.TestFlag(Days))
        ret.Days = _subtract_time_units(msecs, (long long)1000 * DAY_IN_SECONDS);
    if(f.TestFlag(Hours))
        ret.Hours = _subtract_time_units(msecs, (long long)1000 * HOUR_IN_SECONDS);
    if(f.TestFlag(Minutes))
        ret.Minutes = _subtract_time_units(msecs, (long long)1000 * MINUTE_IN_SECONDS);
    if(f.TestFlag(Seconds))
        ret.Seconds = _subtract_time_units(msecs, (long long)1000);
    if(f.TestFlag(MSeconds))
        ret.MSeconds = msecs;

    return ret;
}

long long DateTime::_subtract_time_units(long long &msecs, long long how_many_msecs_per_unit)
{
    long long ret(msecs / how_many_msecs_per_unit);
    msecs -= ret * how_many_msecs_per_unit;
    return ret;
}


END_NAMESPACE_GUTIL1;
