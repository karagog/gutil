/*Copyright 2010-2012 George Karagoulis

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
#include "gutil_flags.h"
#include "gutil_globals.h"

NAMESPACE_GUTIL2(QT, DataObjects);


/** A customization of QDateTime.
    DateTime builds on the functionality of QDateTime by adding +/- operators and
    a function to break down the distance between times.  Other than that you can
    treat it identically to QDateTime.
*/
class DateTime :
        public QDateTime
{
public:

    inline DateTime(){}
    inline DateTime(const QDateTime &other)
        :QDateTime(other){}
    explicit inline DateTime(const QDate &dt)
        :QDateTime(dt){}
    explicit inline DateTime(const QTime &dt)
        :QDateTime(QDate(), dt){}
    inline DateTime(const QDate &dt, const QTime &tm)
        :QDateTime(dt, tm){}

    /** The time T=0, or 0 seconds from the epoch */
    inline static DateTime Origin(){
        return fromMSecsSinceEpoch(0);
    }

    /** Adding date times is done by adding/subtracting the distance
        since the last epoch
    */
    inline DateTime operator + (const DateTime &other) const{
        return addMSecs(Origin().msecsTo(other));
    }
    /** Subtracting date times is done by adding/subtracting the distance
        since the last epoch
    */
    inline DateTime operator - (const DateTime &other) const{
        return addMSecs(-Origin().msecsTo(other));
    }

    /** Defines a break-down of an amount of time.  You can use this to represent
        any arbitrary combination of time units
    */
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

        inline TimeBreakdown()
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

    /** An enum for declaring how you want your time breakdown */
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
    GUTIL_DECLARE_FLAGS(TimeBreakdownFlags, TimeBreakdownFlag)

    /** Returns the time "distance" between two points in time. */
    inline TimeBreakdown GetTimeDistanceBreakdown(const DateTime &end_time, TimeBreakdownFlags f) const{
        return GetTimeDistanceBreakdown(*this, end_time, f);
    }
    /** Returns the time "distance" between two points in time. */
    static TimeBreakdown GetTimeDistanceBreakdown(const DateTime &start_time,
                                                  const DateTime &end_time,
                                                  TimeBreakdownFlags);


private:

    // Returns the number of units subtracted
    static long long _subtract_time_units(long long &msecs, long long how_many_msecs_per_unit);

};


END_NAMESPACE_GUTIL2;


GUTIL_DECLARE_FLAG_OPERATORS(GUtil::QT::DataObjects::DateTime::TimeBreakdownFlags,
                             GUtil::QT::DataObjects::DateTime::TimeBreakdownFlag)


#endif // GDATETIME_H
