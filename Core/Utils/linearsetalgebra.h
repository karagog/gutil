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

    inline T LowerBound() const{
        return _p_LowerBound;
    }
    inline T UpperBound() const{
        return _p_UpperBound;
    }
    inline void SetLowerBound(const T &val){
        _p_LowerBound = val;
        lb_modified = true;
        _universe = false;
    }
    inline void SetUpperBound(const T &val){
        _p_UpperBound = val;
        ub_modified = true;
        _universe = false;
    }

    inline bool IsBounded() const{
        return _universe ?
                    false :
                    (!lb_modified && !ub_modified) ?
                        true :
                        (_p_LowerBound < _p_UpperBound);
    }

    inline bool IsUniverse() const{
        return _universe;
    }
    inline bool IsNull() const{
        return (!lb_modified && !ub_modified && !_universe);
    }

    inline bool Contains(const T &t) const{
        return _universe ?
                    true :
                    !lb_modified ?
                        (t < _p_UpperBound || t == _p_UpperBound) :
                        !ub_modified ?
                            (_p_LowerBound < t || _p_LowerBound == t) :
                            _p_LowerBound < _p_UpperBound ?
                                (_p_LowerBound < t || _p_LowerBound == t) && (t < _p_UpperBound || t == _p_UpperBound) :
                                (_p_LowerBound < t || _p_LowerBound == t) || (t < _p_UpperBound || t == _p_UpperBound);
    }

    Range()
        :lb_modified(false),
          ub_modified(false),
          _universe(false)
    {}
    Range(const T &lb, const T &ub)
        :lb_modified(true),
          ub_modified(true),
          _p_LowerBound(lb),
          _p_UpperBound(ub),
          _universe(false)
    {}
    // Constructs a range that represents all possible values
    inline static Range<T> Universe(){
        Range<T> ret;
        ret._universe = true;
        return ret;
    }
    virtual ~Range(){}


protected:

    bool lb_modified;
    bool ub_modified;


private:

    T _p_LowerBound;
    T _p_UpperBound;
    bool _universe;

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
        return m_ranges.size() > 0 && m_ranges.front().IsUniverse();
    }

    // The number of independent ranges it takes to sum into this region
    inline const std::vector< Range<T> > &Ranges() const{
        return m_ranges;
    }

    // Here are your set operators
    inline Region<T> Union(const Region<T> &r) const{ return Union(*this, r); }
    static Region<T> Union(const Region<T> &one, const Region<T> &two);

    inline Region<T> Intersect(const Region<T> &r) const{ return Intersect(*this, r); }
    inline static Region<T> Intersect(const Region<T> &one, const Region<T> &two){
        // We can obtain the intersect by using a combination of unions and complements
        // (A & B) == ~(~A | ~B)
        return ~(~one | ~two);
    }

    Region<T> Complement(const Region<T> &r = Range<T>::UniverseSet()) const;

    inline Region<T> SymmetricDifference(const Region<T> &r) const{ return SymmetricDifference(*this, r); }
    inline static Region<T> SymmetricDifference(const Region<T> &r1, const Region<T> &r2){
        // The symmetric difference is (A | B) - (A & B), or (A - B) | (B - A),
        //  or those that are in A or in B, but not in both
        return (r1 - r2) | (r2 - r1);
    }

    // Some convenient set operators (just facades over the actual functions)
    inline Region<T> operator - (const Region<T> &r) const{
        return r.Complement(*this);
    }
    inline Region<T> operator ~() const{
        return Region<T>::Complement();
    }
    inline Region<T> operator + (const Region<T> &r) const{
        return Region<T>::Union(r);
    }
    inline Region<T> operator | (const Region<T> &r) const{
        return Region<T>::Union(r);
    }
    inline Region<T> operator & (const Region<T> &r) const{
        return Region<T>::Intersect(r);
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
    inline IntegerRange(){}
    inline IntegerRange(int lb, int ub)     :Range<int>(lb, ub){}
    inline IntegerRange(const Range<int> &r):Range<int>(r){}
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
    std::vector< Range<T> > &v1( ret.m_ranges );
    const std::vector< Range<T> > &v2( reg2.m_ranges );

    // Merge the lists of ranges
    v1.insert(v1.end(), v2.begin(), v2.end());

    // Remove range overlaps to minimize the number of ranges we keep track of
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
        for(typename std::vector< Range<T> >::iterator iter1(m_ranges.begin());
            !something_changed && iter1 != m_ranges.end();
            iter1++)
        {
            for(typename std::vector< Range<T> >::iterator iter2(iter1 + 1);
                !something_changed && iter2 != m_ranges.end();
                iter2++)
            {
                Region<T> reg( _union(*iter1, *iter2) );

                // If there are fewer than 2 ranges needed to represent the union,
                //  then we accept the merged range and restart our double-loop
                if(reg.m_ranges.size() == 1)
                {
                    *iter1 = reg.m_ranges.front();
                    m_ranges.erase(iter2);
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

    // If either is all time, then the result is automatically all-time
    if(r1.IsUniverse() || r2.IsUniverse())
        ret.m_ranges.push_back(r1);
    else
    {
        bool range1_unbounded(false),
                range2_unbounded(false);
        T tmpLower, tmpUpper;
        bool tmpLower_set(false), tmpUpper_set(false);
        if(!r1.lb_modified)
        {
            tmpUpper = r1._p_UpperBound;
            range1_unbounded = true;
            tmpUpper_set = true;
        }
        else if(!r1.ub_modified)
        {
            tmpLower = r1._p_LowerBound;
            range1_unbounded = true;
            tmpLower_set = true;
        }
        else if(r1._p_UpperBound < r1._p_LowerBound)
        {
            tmpLower = r1._p_LowerBound;
            tmpUpper = r1._p_UpperBound;
            range1_unbounded = true;
            tmpUpper_set = true;
            tmpLower_set = true;
        }


        if(!r2.lb_modified)
        {
            tmpUpper = tmpUpper_set ?
                        gMax(tmpUpper, r2._p_UpperBound) :
                        r2._p_UpperBound;
            range2_unbounded = true;
        }
        else if(!r2.ub_modified)
        {
            tmpLower = tmpLower_set ?
                        gMin(tmpLower, r2._p_LowerBound) :
                        r2._p_LowerBound;
            range2_unbounded = true;
        }
        else if(r2._p_UpperBound < r2._p_LowerBound)
        {
            tmpLower = tmpLower_set ?
                        gMin(tmpLower, r2._p_LowerBound) :
                        r2._p_LowerBound;
            tmpUpper = tmpUpper_set ?
                        gMax(tmpUpper, r2._p_UpperBound) :
                        r2._p_UpperBound;
            range2_unbounded = true;
        }


        if(range1_unbounded && range2_unbounded)
        {
            ret.m_ranges.push_back(Range<T>(tmpLower, tmpUpper));
        }
        // We get to this else block if at least one range is bounded
        else
        {
            // If one range is bounded and another is unbounded
            if(range1_unbounded || range2_unbounded)
            {
                const Range<T> *unbounded_range(range1_unbounded ? &r1 : &r2);
                const Range<T> *bounded_range(range1_unbounded ? &r2 : &r1);

                if( bounded_range->_p_LowerBound < unbounded_range->_p_UpperBound ||
                        bounded_range->_p_LowerBound == unbounded_range->_p_UpperBound)
                {
                    Range<T> r;
                    r.SetUpperBound(gMax(unbounded_range->_p_UpperBound, bounded_range->_p_UpperBound));
                    ret.m_ranges.push_back(r);
                }
                else if(unbounded_range->_p_LowerBound < bounded_range->_p_UpperBound ||
                        unbounded_range->_p_LowerBound == bounded_range->_p_UpperBound)
                {
                    Range<T> r;
                    r.SetLowerBound(gMin(unbounded_range->_p_LowerBound, bounded_range->_p_LowerBound));
                    ret.m_ranges.push_back(r);
                }
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
                if(r1._p_UpperBound < r2._p_LowerBound ||
                        r2._p_UpperBound < r1._p_LowerBound)
                {
                    ret.m_ranges.push_back(r1);
                    ret.m_ranges.push_back(r2);
                }

                // If the bounded ranges intersect, their union is easy
                else
                {
                    ret.m_ranges.push_back(Range<T>(gMin(r1._p_LowerBound, r2._p_LowerBound),
                                                    gMax(r1._p_UpperBound, r2._p_UpperBound)));
                }
            }
        }
    }

    return ret;
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

        if(Region<T>::IsNull())
            m_ranges.push_back(Range<T>::Universe());   // Opposite of null is the universe
        else if(Region<T>::IsUniverseSet())
            m_ranges.clear();   // Opposite of null is universe
        else
        {
            // To get a region's complement wrt the Universe, we iterate through each
            //  range and reverse each of their Lowerbounds and Upperbounds.  Then we
            //  assess the situation and assemble contiguous ranges and throw out
            //  the overlapping areas.

            // Swap the lower bound and upper bound for every range
            for(typename std::vector< Range<T> >::iterator iter(m_ranges.begin());
                iter != m_ranges.end();
                iter++)
            {
                T mem( iter->GetLowerBound() );
                iter->SetLowerBound( iter->GetUpperBound());
                iter->SetUpperBound( mem );
            }

            // Now go through one range at a time, merging/removing indexes as needed
            //  Append new ranges to the end of the list, remove items directly from
            //  the list (that's why we iterate backwards through the list)
            for(typename std::vector< Range<T> >::iterator iter(m_ranges.end() - 1);
                m_ranges.begin() <= iter;
                iter--)
            {

            }
        }
    }
    else
    {
        // To get the relative complement of this in 'r' (r - this), we can express
        //  it as (this & ~r) which is equal to ~(~this | r)
        return ~(Region<T>::Complement() | r);
    }
}

template <class T>
bool Region<T>::Contains(const T &dt)
{
    for(typename std::vector< Range<T> >::const_iterator it(m_ranges.begin());
        it != m_ranges.end();
        it++)
    {
        if(it->Contains(dt))
            return true;
    }

    return false;
}


GUTIL_END_CORE_NAMESPACE;

#endif // LINEARSETALGEBRA_H
