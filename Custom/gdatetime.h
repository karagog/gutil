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

#ifndef GDATETIME_H
#define GDATETIME_H

#include <QDateTime>
#include <QFlags>
#include "Core/gutil_globals.h"

GUTIL_BEGIN_NAMESPACE(Custom);


class GDateTime :
        public QDateTime
{
public:

    GDateTime(const QDateTime &other = QDateTime());
    GDateTime(const QDate &);

    inline static GDateTime Origin(){
        return fromMSecsSinceEpoch(0);
    }

    // Adding/Subtracting date times is done by adding/subtracting the distance
    //  since the last epoch
    GDateTime operator + (const GDateTime &) const;
    GDateTime operator - (const GDateTime &) const;

    // Another set of classes/functions for breaking down the distance between times
    class TimeBreakdown
    {
    public:

        int MSeconds;
        int Seconds;
        int Minutes;
        int Hours;
        int Days;
        int Years;
        int Decades;
        int Centuries;
        int Millennia;

        TimeBreakdown()
            :MSeconds(-1),
              Seconds(-1),
              Minutes(-1),
              Hours(-1),
              Days(-1),
              Years(-1),
              Decades(-1),
              Centuries(-1),
              Millennia(-1)
        {}

    };

    enum TimeBreakdownFlag{
        MSeconds,
        Seconds,
        Minutes,
        Hours,
        Days,
        Years,
        Decades,
        Centuries,
        Millennia
    };
    Q_DECLARE_FLAGS(TimeBreakdownFlags, TimeBreakdownFlag)

    inline TimeBreakdown GetTimeDistanceBreakdown(const GDateTime &end_time, TimeBreakdownFlags f) const{
        return GetTimeDistanceBreakdown(*this, end_time, f);
    }
    static TimeBreakdown GetTimeDistanceBreakdown(const GDateTime &start_time,
                                                  const GDateTime &end_time,
                                                  TimeBreakdownFlags);


private:

    // Returns the number of units subtracted
    static long long _subtract_time_units(long long &msecs, long long how_many_msecs_per_unit);

};


Q_DECLARE_OPERATORS_FOR_FLAGS(GDateTime::TimeBreakdownFlags)


GUTIL_END_NAMESPACE;

#endif // GDATETIME_H
