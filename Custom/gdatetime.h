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

GUTIL_BEGIN_NAMESPACE(Custom);


// GDateTime has static functions to conduct set operations on regions of time.
//  It allows you to easily find if a time range intersects another one, or
//  lots of other interesting things while manipulating times ranges as sets.

class GDateTime :
        public QDateTime
{
public:

    GDateTime(const QDateTime &other = QDateTime());

    class TimeRange
    {
    public:

        QDateTime LowerBound;
        QDateTime UpperBound;

        inline bool IsAllTime() const{
            return IsNull() || UpperBound == LowerBound;
        }
        inline bool IsNull() const{
            return LowerBound.isNull() && UpperBound.isNull();
        }

        inline bool Contains(const QDateTime &dt) const{
            return dt.isNull() ?
                        false :
                        IsAllTime() ?
                            true :
                            LowerBound.isNull() ?
                                dt <= UpperBound :
                                UpperBound.isNull() ?
                                    LowerBound <= dt :
                                    LowerBound < UpperBound ?
                                        LowerBound <= dt && dt <= UpperBound :
                                        LowerBound <= dt || dt <= UpperBound;
        }


        TimeRange(const QDateTime &lb = QDateTime(),
                  const QDateTime &ub = QDateTime())
            :LowerBound(lb),
              UpperBound(ub)
        {}

    };


    // This class builds off the concept of a time range to support a region of
    //  time, which is a sum of time ranges
    class TimeRegion
    {
    public:

        TimeRegion();
        TimeRegion(const TimeRange &);

        bool IsNull() const;
        bool IsAllTime() const;

        bool Contains(const QDateTime &);

        TimeRegion Union(const TimeRegion &) const;
        TimeRegion Intersect(const TimeRegion &) const;
        TimeRegion Complement(const TimeRegion &r = TimeRange()) const;


    private:

        QList<TimeRange> m_ranges;

        // Get the union of two time ranges
        static TimeRegion _union(const TimeRange &, const TimeRange &);

        // this removes any overlapping ranges within the region
        void _clean();

    };




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


GUTIL_END_NAMESPACE;

#endif // GDATETIME_H
