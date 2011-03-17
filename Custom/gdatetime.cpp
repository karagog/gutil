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

GDateTime::TimeRegion::TimeRegion()
{}

GDateTime::TimeRegion::TimeRegion(const TimeRange &r)
{
    m_ranges.append(r);
}

bool GDateTime::TimeRegion::IsNull() const
{
    return m_ranges.count() == 0;
}

bool GDateTime::TimeRegion::IsAllTime() const
{
    return m_ranges.count() > 0 && m_ranges[0].IsAllTime();
}

GDateTime::TimeRegion GDateTime::TimeRegion::Union(const GDateTime::TimeRegion &reg) const
{
    TimeRegion ret(*this);
    ret.m_ranges.append(reg.m_ranges);
    ret._clean();
    return ret;
}

GDateTime::TimeRegion GDateTime::Union(const TimeRegion &one, const TimeRegion &two)
{
    return one.Union(two);
}

GDateTime::TimeRegion GDateTime::TimeRegion::_union(const TimeRange &r1, const TimeRange &r2)
{
    TimeRegion ret;

    // If either is all time, then the result is automatically all-time
    if(r1.IsAllTime() || r2.IsAllTime())
        ret.m_ranges.append(r1);
    else
    {
        bool range1_unbounded(false),
                range2_unbounded(false);
        QDateTime tmpLower, tmpUpper;
        if(r1.LowerBound.isNull())
        {
            tmpUpper = r1.UpperBound;
            range1_unbounded = true;
        }
        else if(r1.UpperBound.isNull())
        {
            tmpLower = r1.LowerBound;
            range1_unbounded = true;
        }
        else if(r1.UpperBound < r1.LowerBound)
        {
            tmpLower = r1.LowerBound;
            tmpUpper = r1.UpperBound;
            range1_unbounded = true;
        }


        if(r2.LowerBound.isNull())
        {
            tmpUpper = tmpUpper.isValid() ?
                        gMax(tmpUpper, r2.UpperBound) :
                        r2.UpperBound;
            range2_unbounded = true;
        }
        else if(r2.UpperBound.isNull())
        {
            tmpLower = tmpLower.isValid() ?
                        gMin(tmpLower, r2.LowerBound) :
                        r2.LowerBound;
            range2_unbounded = true;
        }
        else if(r2.UpperBound < r2.LowerBound)
        {
            tmpLower = tmpLower.isValid() ?
                        gMin(tmpLower, r2.LowerBound) :
                        r2.LowerBound;
            tmpUpper = tmpUpper.isValid() ?
                        gMax(tmpUpper, r2.UpperBound) :
                        r2.UpperBound;
            range2_unbounded = true;
        }


        if(range1_unbounded && range2_unbounded)
        {
            ret.m_ranges.append(GDateTime::TimeRange(tmpLower, tmpUpper));
        }
        // We get to this else block if at least one range is bounded
        else
        {
            // If one range is bounded and another is unbounded
            if(range1_unbounded || range2_unbounded)
            {
                const TimeRange *unbounded_range(range1_unbounded ? &r1 : &r2);
                const TimeRange *bounded_range(range1_unbounded ? &r2 : &r1);

                if(unbounded_range->UpperBound > bounded_range->UpperBound ||
                        unbounded_range->LowerBound < bounded_range->LowerBound)
                    ret.m_ranges.append(*unbounded_range);
                else if(bounded_range->LowerBound < unbounded_range->UpperBound)
                    ret.m_ranges.append(GDateTime::TimeRange(
                                            gMax(unbounded_range->UpperBound, bounded_range->UpperBound),
                                            gMax(unbounded_range->LowerBound, bounded_range->LowerBound)));
                else if(bounded_range->UpperBound > unbounded_range->LowerBound)
                    ret.m_ranges.append(GDateTime::TimeRange(
                                            gMin(unbounded_range->UpperBound, bounded_range->UpperBound),
                                            gMin(unbounded_range->LowerBound, bounded_range->LowerBound)));
                else
                {
                    // Though one range is unbounded, they are still disjoint, so add
                    ret.m_ranges.append(r1);
                    ret.m_ranges.append(r2);
                }
            }
            // If both ranges are bounded
            else
            {
                // If the bounded ranges are disjoint, simply add them
                if(r1.UpperBound < r2.LowerBound ||
                        r2.UpperBound < r1.LowerBound)
                {
                    ret.m_ranges.append(r1);
                    ret.m_ranges.append(r2);
                }

                // If the bounded ranges intersect, their union is easy
                else
                {
                    ret.m_ranges.append(GDateTime::TimeRange(gMin(r1.LowerBound, r2.LowerBound),
                                                             gMax(r1.UpperBound, r2.UpperBound)));
                }
            }
        }
    }

    return ret;
}

void GDateTime::TimeRegion::_clean()
{
    // Repeatedly try to merge every possible combination of ranges until it gets
    //  to the smallest possible representation
    bool something_changed;
    do
    {
        something_changed = false;
        for(int i = 0; !something_changed && i < m_ranges.count(); i++)
        {
            for(int j = i + 1; !something_changed && j < m_ranges.count(); j++)
            {
                TimeRegion reg(_union(m_ranges[i], m_ranges[j]));

                // If there are fewer than 2 ranges needed to represent the union,
                //  then we accept the merged range and restart our double-loop
                if(reg.m_ranges.count() == 1)
                {
                    m_ranges[i] = reg.m_ranges[0];
                    m_ranges.removeAt(j);
                    something_changed = true;
                }
            }
        }
    }while(something_changed);
}

bool GDateTime::TimeRegion::Contains(const GDateTime &dt)
{
    for(int i = 0; i < m_ranges.count(); i++)
    {
        if(m_ranges[i].Contains(dt))
            return true;
    }

    return false;
}

void GDateTime::TimeRegion::Clear()
{
    m_ranges.clear();
}
