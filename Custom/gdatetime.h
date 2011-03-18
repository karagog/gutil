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
#include <QList>
#include "gutil_macros.h"
#include "Core/Utils/linearsetalgebra.h"

GUTIL_BEGIN_NAMESPACE(Custom);


class TimeRange;

class GDateTime :
        public QDateTime
{
public:

    GDateTime(const QDateTime &other = QDateTime());
    GDateTime(const QDate &);


    // Another set of classes/functions for breaking down the distance between times
    class TimeBreakdown
    {
    public:

        int MSeconds;
        int Seconds;
        int Minutes;
        int Hours;
        int Days;
        int Weeks;
        int Months;
        int Years;
        int Decades;
        int Centuries;
        int Millennia;

        TimeBreakdown()
            :MSeconds(0),
              Seconds(0),
              Minutes(0),
              Hours(0),
              Days(0),
              Weeks(0),
              Months(0),
              Years(0),
              Decades(0),
              Millennia(0)
        {}

        enum TimeBreakdownEnum{
            Traditional,
            DecadesMax,
            YearsMax,
            MonthsMax,
            WeeksMax,
            DaysMax,
            HoursMax,
            MinutesMax,
            SecondsMax,
            MSecondsMax
        };

    };


    TimeBreakdown GetTimeBreakdown(const TimeRange &,
                                   TimeBreakdown::TimeBreakdownEnum tbe = TimeBreakdown::Traditional);

};


// These are trivial declarations of the time range objects, built off
//  the low-level set operator class to support QDateTimes
class TimeRange :
        public Core::Utils::Range<GDateTime>
{
public:
    TimeRange(const GDateTime &lb = GDateTime(),
              const GDateTime &ub = GDateTime())
        :Core::Utils::Range<GDateTime>(lb, ub)
    {}
protected:
    bool is_value_null(const GDateTime &v) const{
        return v.isNull();
    }
};

class TimeRegion :
        public Core::Utils::Region<GDateTime>
{
public:
    inline TimeRegion(){}
    inline TimeRegion(const Core::Utils::Region<GDateTime> &r)
        :Core::Utils::Region<GDateTime>(r)
    {}
};


GUTIL_END_NAMESPACE;

#endif // GDATETIME_H
