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

#include "gdatetime.h"
GUTIL_USING_NAMESPACE(Custom);

GDateTime::GDateTime(const QDateTime &other)
    :QDateTime(other)
{}

GDateTime::GDateTime(const QDate &dt)
    :QDateTime(dt)
{}

GDateTime GDateTime::operator + (const GDateTime &other) const
{
    return addMSecs(Origin().msecsTo(other));
}

GDateTime GDateTime::operator - (const GDateTime &other) const
{
    return addMSecs(-Origin().msecsTo(other));
}

GDateTime::TimeBreakdown GDateTime::GetTimeDistanceBreakdown(const GDateTime &start_time,
                                                             const GDateTime &end_time,
                                                             TimeBreakdownFlags f)
{
    TimeBreakdown ret;
    long long msecs( start_time.msecsTo(end_time) );

    if(f.testFlag(Millennia))
        ret.Millennia = _subtract_time_units(msecs, (long long)1000 * 1000 * YEAR_IN_SECONDS);
    if(f.testFlag(Centuries))
        ret.Centuries = _subtract_time_units(msecs, (long long)100 * 1000 * YEAR_IN_SECONDS);
    if(f.testFlag(Decades))
        ret.Decades = _subtract_time_units(msecs, (long long)10 * 1000 * YEAR_IN_SECONDS);
    if(f.testFlag(Years))
        ret.Years = _subtract_time_units(msecs, (long long)1000 * YEAR_IN_SECONDS);
    if(f.testFlag(Days))
        ret.Days = _subtract_time_units(msecs, (long long)1000 * DAY_IN_SECONDS);
    if(f.testFlag(Hours))
        ret.Hours = _subtract_time_units(msecs, (long long)1000 * HOUR_IN_SECONDS);
    if(f.testFlag(Minutes))
        ret.Minutes = _subtract_time_units(msecs, (long long)1000 * MINUTE_IN_SECONDS);
    if(f.testFlag(Seconds))
        ret.Seconds = _subtract_time_units(msecs, (long long)1000);
    if(f.testFlag(MSeconds))
        ret.MSeconds = msecs;

    return ret;
}

long long GDateTime::_subtract_time_units(long long &msecs, long long how_many_msecs_per_unit)
{
    long long ret(msecs / how_many_msecs_per_unit);
    msecs -= ret * how_many_msecs_per_unit;
    return ret;
}
