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

#ifndef LINEARSETALGEBRA_H
#define LINEARSETALGEBRA_H

#include "gutil_macros.h"
#include <limits.h>
#include <vector>

GUTIL_BEGIN_CORE_NAMESPACE(Utils);


// These are classes to facilitate set arithmetic.  You can do things like unions,
//  intersects, compliments to ranges and regions of a linear scale of any type.
//  You can use the provided example of an integer range, or sub-class the Range
//  class yourself and simply override the is_value_null virtual function to
//  tell the class which values are null.

template <class T> class Range
{
    template <class U> friend class Region;
public:

    T LowerBound;
    T UpperBound;

    inline bool IsBounded() const{
        return (!is_value_null(LowerBound) && !is_value_null(UpperBound)) &&
                LowerBound < UpperBound;
    }

    inline bool IsUniverseSet() const{
        return IsNull();
    }
    inline bool IsNull() const{
        return (is_value_null(LowerBound) && is_value_null(UpperBound)) ||
                LowerBound == UpperBound;
    }
    inline const T &NullValue() const{
        return _null_value;
    }

    inline bool Contains(const T &t) const{
        return this->IsUniverseSet() ?
                    true :
                    is_value_null(LowerBound) ?
                        (t < UpperBound || t == UpperBound) :
                        is_value_null(UpperBound) ?
                            (LowerBound < t || LowerBound == t) :
                            LowerBound < UpperBound ?
                                (LowerBound < t || LowerBound == t) && (t < UpperBound || t == UpperBound) :
                                (LowerBound < t || LowerBound == t) || (t < UpperBound || t == UpperBound);
    }

    // When constructing, you must pass in
    Range(const T &lb, const T &ub, const T &null_value = T())
        :LowerBound(lb),
          UpperBound(ub),
          _null_value(null_value)
    {}
    virtual ~Range(){}


protected:

    T _null_value;
    bool is_value_null(const T &v) const{
        return v == _null_value;
    }

};



// This class builds off the concept of a range to support a region,
//  which is a union of several ranges
template <class T> class Region
{
public:

    inline Region(){}
    inline Region(const Range<T> &r) { m_ranges.push_back(r); }
    virtual ~Region(){}
    inline void Clear(){
        m_ranges.clear();
    }

    inline bool IsNull() const{
        return m_ranges.size() == 0;
    }
    inline bool IsUniverseSet() const{
        return m_ranges.size() > 0 && m_ranges[0].IsUniverseSet();
    }

    inline static Region<T> GetUniverseSet(){
        return Range<T>(T(), T());
    }

    // The number of independent ranges it takes to sum into this region
    inline const std::vector< Range<T> > &Ranges() const{
        return m_ranges;
    }

    // Here are your set operators
    inline Region<T> Union(const Region<T> &r) const{ return Union(*this, r); }
    static Region<T> Union(const Region<T> &one, const Region<T> &two);

    inline Region<T> Intersect(const Region<T> &r) const{ return Intersect(*this, r); }
    static Region<T> Intersect(const Region<T> &one, const Region<T> &two);

    Region<T> Complement(const Region<T> &r = GetUniverseSet()) const;

    inline Region<T> SymmetricDifference(const Region<T> &r) const{ return SymmetricDifference(*this, r); }
    static Region<T> SymmetricDifference(const Region<T> &r1, const Region<T> &r2);

    // Some convenient set operators (just facades over the actual functions)
    inline Region<T> operator - (const Region<T> &r) const{
        return r.Complement(*this);
    }
    // In real set notation this is a backslash, but we can't do that in C++
    inline Region<T> operator / (const Region<T> &r) const{
        return r.Complement(*this);
    }
    inline Region<T> operator ~() const{
        return this->Complement();
    }
    inline Region<T> operator + (const Region<T> &r) const{
        return this->Union(r);
    }
    inline Region<T> operator | (const Region<T> &r) const{
        return this->Union(r);
    }
    inline Region<T> operator & (const Region<T> &r) const{
        return this->Intersect(r);
    }

    bool Contains(const T &);


private:

    std::vector< Range<T> > m_ranges;

    // Get the union of two ranges
    static Region<T> _union(const Range<T> &, const Range<T> &);

    // this removes any overlapping ranges within the region
    void _clean();

};



// Here is an example of a class you might use.  It is a real number scale
class IntegerRange :
        public Range<int>
{
public:
    inline IntegerRange(int lb = INT_MIN, int ub = INT_MIN) :
        Range<int>(lb, ub, INT_MIN){}
};

class IntegerRegion :
        public Region<int>
{
public:
    inline IntegerRegion(){}
    inline IntegerRegion(const Region<int> &other) : Region<int>(other){}
};






template <class T>
Region<T> Region<T>::Union(const Region<T> &reg1, const Region<T> &reg2)
{
    Region<T> ret(reg1);
    for(int i = 0; i < (int)reg2.m_ranges.size(); i++)
        ret.m_ranges.push_back(reg2.m_ranges[i]);
    ret._clean();
    return ret;
}

template <class T>
void Region<T>::_clean()
{
    // Repeatedly try to merge every possible combination of ranges until it gets
    //  to the smallest possible representation
    bool something_changed;
    do
    {
        something_changed = false;
        for(int i = 0; !something_changed && i < (int)m_ranges.size(); i++)
        {
            for(int j = i + 1; !something_changed && j < (int)m_ranges.size(); j++)
            {
                Region<T> reg(_union(m_ranges[i], m_ranges[j]));

                // If there are fewer than 2 ranges needed to represent the union,
                //  then we accept the merged range and restart our double-loop
                if(reg.m_ranges.size() == 1)
                {
                    m_ranges[i] = reg.m_ranges[0];
                    m_ranges.erase(m_ranges.begin() + j);
                    something_changed = true;
                }
            }
        }
    }while(something_changed);
}

template <class T>
Region<T> Region<T>::_union(const Range<T> &r1, const Range<T> &r2)
{
    Region<T> ret;
    Q_ASSERT(r1.NullValue() == r2.NullValue());
    const T &null_val(r1.NullValue());

    // If either is all time, then the result is automatically all-time
    if(r1.IsUniverseSet() || r2.IsUniverseSet())
        ret.m_ranges.push_back(r1);
    else
    {
        bool range1_unbounded(false),
                range2_unbounded(false);
        T tmpLower, tmpUpper;
        bool tmpLower_set(false), tmpUpper_set(false);
        if(r1.is_value_null(r1.LowerBound))
        {
            tmpUpper = r1.UpperBound;
            range1_unbounded = true;
            tmpUpper_set = true;
        }
        else if(r1.is_value_null(r1.UpperBound))
        {
            tmpLower = r1.LowerBound;
            range1_unbounded = true;
            tmpLower_set = true;
        }
        else if(r1.UpperBound < r1.LowerBound)
        {
            tmpLower = r1.LowerBound;
            tmpUpper = r1.UpperBound;
            range1_unbounded = true;
            tmpUpper_set = true;
            tmpLower_set = true;
        }


        if(r2.is_value_null(r2.LowerBound))
        {
            tmpUpper = tmpUpper_set ?
                        gMax(tmpUpper, r2.UpperBound) :
                        r2.UpperBound;
            range2_unbounded = true;
        }
        else if(r2.is_value_null(r2.UpperBound))
        {
            tmpLower = tmpLower_set ?
                        gMin(tmpLower, r2.LowerBound) :
                        r2.LowerBound;
            range2_unbounded = true;
        }
        else if(r2.UpperBound < r2.LowerBound)
        {
            tmpLower = tmpLower_set ?
                        gMin(tmpLower, r2.LowerBound) :
                        r2.LowerBound;
            tmpUpper = tmpUpper_set ?
                        gMax(tmpUpper, r2.UpperBound) :
                        r2.UpperBound;
            range2_unbounded = true;
        }


        if(range1_unbounded && range2_unbounded)
        {
            ret.m_ranges.push_back(Range<T>(tmpLower, tmpUpper, null_val));
        }
        // We get to this else block if at least one range is bounded
        else
        {
            // If one range is bounded and another is unbounded
            if(range1_unbounded || range2_unbounded)
            {
                const Range<T> *unbounded_range(range1_unbounded ? &r1 : &r2);
                const Range<T> *bounded_range(range1_unbounded ? &r2 : &r1);

                if( bounded_range->LowerBound < unbounded_range->UpperBound ||
                        bounded_range->LowerBound == unbounded_range->UpperBound)
                    ret.m_ranges.push_back(Range<T>(T(),
                                                    gMax(unbounded_range->UpperBound, bounded_range->UpperBound),
                                                    null_val));
                else if(unbounded_range->LowerBound < bounded_range->UpperBound ||
                        unbounded_range->LowerBound == bounded_range->UpperBound)
                    ret.m_ranges.push_back(Range<T>(gMin(unbounded_range->LowerBound, bounded_range->LowerBound),
                                                    T(),
                                                    null_val));
                else
                {
                    // Though one range is unbounded, they are still disjoint, so add
                    ret.m_ranges.push_back(r1);
                    ret.m_ranges.push_back(r2);
                }
            }
            // If both ranges are bounded
            else
            {
                // If the bounded ranges are disjoint, simply add them
                if(r1.UpperBound < r2.LowerBound ||
                        r2.UpperBound < r1.LowerBound)
                {
                    ret.m_ranges.push_back(r1);
                    ret.m_ranges.push_back(r2);
                }

                // If the bounded ranges intersect, their union is easy
                else
                {
                    ret.m_ranges.push_back(Range<T>(gMin(r1.LowerBound, r2.LowerBound),
                                                    gMax(r1.UpperBound, r2.UpperBound),
                                                    null_val));
                }
            }
        }
    }

    return ret;
}

template <class T>
Region<T> Region<T>::Intersect(const Region<T> &r1, const Region<T> &r2)
{
    // We can obtain the intersect by using a combination of unions and complements
    // (A & B) == ~(~A | ~B)
    return ~(~r1 | ~r2);
}

template <class T>
Region<T> Region<T>::Complement(const Region<T> &r) const
{
    // We can get the complement by using Universal complements and Unions alone.
    //  This is important, because we'll be using complements to determine intersects,
    //  so we don't want to create a circular dependency.

    if(r.IsUniverseSet())
    {
        // In this case, we're taking the 'absolute complement' which is simply
        //  the inverse of our region.

        // To get a region's complement wrt the Universe, we iterate through each
        //  range and reverse each of their Lowerbounds and Upperbounds.  Then we
        //  assess the situation and assemble contiguous ranges and throw out
        //  the overlapping areas.

        // Swap the lower bound and upper bound for every range
        for(int i = 0; i < (int)m_ranges.size(); i++)
        {
            T mem( m_ranges[i].LowerBound );
            m_ranges[i].LowerBound = m_ranges[i].UpperBound;
            m_ranges[i].UpperBound = mem;
        }

        // Now go through one range at a time, merging/removing indexes as needed
        //  Append new ranges to the end of the list, remove items directly from
        //  the list (that's why we iterate backwards through the list)
        const int cnt(m_ranges.size());
        for(int i = cnt - 1; i >= 0; i--)
        {

        }
    }
    else
    {
        // To get the relative complement of this in 'r' (r - this), we can express
        //  it as (this & ~r) which is equal to ~(~this | r)
        return ~(~*this | r);
    }
}

template <class T>
Region<T> Region<T>::SymmetricDifference(const Region<T> &r1, const Region<T> &r2)
{
    return (r1 - r2) | (r2 - r1);
}

template <class T>
bool Region<T>::Contains(const T &dt)
{
    for(int i = 0; i < (int)m_ranges.size(); i++)
    {
        if(m_ranges[i].Contains(dt))
            return true;
    }

    return false;
}


GUTIL_END_CORE_NAMESPACE;

#endif // LINEARSETALGEBRA_H
