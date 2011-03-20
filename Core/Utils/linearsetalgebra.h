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
#include "Core/exception.h"
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
                        (_p_LowerBound < _p_UpperBound || _p_LowerBound == _p_UpperBound);
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

    inline bool operator == (const Range<T> &other) const{
        return (_universe && other._universe) ||
                (IsNull() && other.IsNull()) ||
                (_p_LowerBound == other._p_LowerBound && _p_UpperBound == other._p_UpperBound);
    }

    Range(const T &min_res)
        :lb_modified(false),
          ub_modified(false),
          m_minres(min_res),
          _universe(false)
    {}
    Range(const T &lb, const T &ub, const T &min_res)
        :lb_modified(true),
          ub_modified(true),
          m_minres(min_res),
          _p_LowerBound(lb),
          _p_UpperBound(ub),
          _universe(false)
    {}
    // Constructs a range that represents all possible values
    inline static Range<T> Universe(const T &min_res){
        Range<T> ret(min_res);
        ret._universe = true;
        return ret;
    }
    inline Range<T> Universe() const{
        return Universe(m_minres);
    }
    virtual ~Range(){}


protected:

    bool lb_modified;
    bool ub_modified;
    T m_minres;


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

    inline Region(const T &minimum_resolution):_p_MinimumResolution(minimum_resolution){}
    inline Region(const Range<T> &r)
        :_p_MinimumResolution(r.m_minres)
    { m_ranges.push_back(r); }
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

    inline Region<T> NullSet() const{
        return Region<T>(_p_MinimumResolution);
    }
    inline Region<T> UniverseSet() const{
        return Region<T>(Range<T>::Universe(_p_MinimumResolution));
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

    Region<T> Complement(const Region<T> &r) const;
    inline Region<T> Complement() const{ return Complement(Region<T>::UniverseSet()); }

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
    inline bool operator == (const Region<T> &other) const{
        // To compare A and B, we can express it as (A - B) == NULL
        return Region<T>::Complement(other).IsNull();
    }

    bool Contains(const T &);


private:

    std::vector< Range<T> > m_ranges;
    T _p_MinimumResolution;

    // Get the union of two ranges
    static Region<T> _union(const Range<T> &, const Range<T> &);

    // this removes any overlapping ranges within the region
    void _clean();

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
    if(r1.m_minres != r2.m_minres)
        THROW_NEW_GUTIL_EXCEPTION2(GUtil::Core::Exception,
                                   "Incompatible ranges.  They have different resolutions");

    Region<T> ret(r1.m_minres);

    // If either is all time, then the result is automatically all-time
    if(r1.IsUniverse() || r2.IsUniverse())
        ret.m_ranges.push_back(r1);
    else
    {
        bool range1_unbounded(false),
                range2_unbounded(false);
        bool is_universe(false);
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
            if(range1_unbounded && r1.lb_modified &&
                        (r1._p_LowerBound < r2._p_UpperBound ||
                         r1._p_LowerBound == r2._p_UpperBound))
            {
                is_universe = true;
            }
            else
            {
                tmpUpper = tmpUpper_set ?
                            gMax(tmpUpper, r2._p_UpperBound) :
                            r2._p_UpperBound;
                range2_unbounded = true;
            }
        }
        else if(!r2.ub_modified)
        {
            if(range1_unbounded && r1.ub_modified &&
                    (r2._p_LowerBound < r1._p_UpperBound ||
                     r2._p_LowerBound == r1._p_UpperBound))
            {
                is_universe = true;
            }
            else
            {
                tmpLower = tmpLower_set ?
                            gMin(tmpLower, r2._p_LowerBound) :
                            r2._p_LowerBound;
                range2_unbounded = true;
            }
        }
        else if(r2._p_UpperBound < r2._p_LowerBound)
        {
            if(range1_unbounded &&
                    ((r1.lb_modified &&
                      (r1._p_LowerBound < r2._p_UpperBound ||
                       r1._p_LowerBound == r2._p_UpperBound))
                     ||
                     (r1.ub_modified &&
                      (r2._p_LowerBound < r1._p_UpperBound ||
                       r2._p_LowerBound == r1._p_UpperBound))))
            {
                is_universe = true;
            }
            else
            {
                tmpLower = tmpLower_set ?
                            gMin(tmpLower, r2._p_LowerBound) :
                            r2._p_LowerBound;
                tmpUpper = tmpUpper_set ?
                            gMax(tmpUpper, r2._p_UpperBound) :
                            r2._p_UpperBound;
                range2_unbounded = true;
            }
        }


        if(is_universe)
        {
            ret.m_ranges.push_back(r1.Universe());
        }
        else if(range1_unbounded && range2_unbounded)
        {
            ret.m_ranges.push_back(Range<T>(tmpLower, tmpUpper, r1.m_minres));
        }
        // We get to this else block if at least one range is bounded
        else
        {
            // If one range is bounded and another is unbounded
            if(range1_unbounded || range2_unbounded)
            {
                bool disjoint(true);
                const Range<T> *unbounded_range(range1_unbounded ? &r1 : &r2);
                const Range<T> *bounded_range(range1_unbounded ? &r2 : &r1);

                ret.m_ranges.push_back(Range<T>(r1.m_minres));
                Range<T> &range_ref(ret.m_ranges.back());

                if(unbounded_range->ub_modified)
                {
                    T val;
                    if((bounded_range->_p_LowerBound < unbounded_range->_p_UpperBound ||
                        bounded_range->_p_LowerBound == unbounded_range->_p_UpperBound))
                    {
                        val = gMax(unbounded_range->_p_UpperBound,
                                   bounded_range->_p_UpperBound);
                        disjoint = false;
                    }
                    else
                        val = unbounded_range->UpperBound();
                    range_ref.SetUpperBound(val);
                }

                if(unbounded_range->lb_modified)
                {
                    T val;
                    if(unbounded_range->_p_LowerBound < bounded_range->_p_UpperBound ||
                            unbounded_range->_p_LowerBound == bounded_range->_p_UpperBound)
                    {
                        val = gMin(unbounded_range->_p_LowerBound, bounded_range->_p_LowerBound);
                        disjoint = false;
                    }
                    else
                        val = unbounded_range->LowerBound();
                    range_ref.SetLowerBound(val);
                }


                if(disjoint)
                {
                    // In the disjoint case, just add the second range
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
                                                    gMax(r1._p_UpperBound, r2._p_UpperBound),
                                                    r1.m_minres));
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
    Region<T> ret(_p_MinimumResolution);

    if(r.IsUniverseSet())
    {
        // In this case, we're taking the 'absolute complement' which is simply
        //  the inverse of our region.
        ret = *this;

        if(Region<T>::IsNull())
            ret.m_ranges.push_back(Range<T>::Universe(_p_MinimumResolution));   // Opposite of null is the universe
        else if(Region<T>::IsUniverseSet())
            ret.m_ranges.clear();   // Opposite of null is universe
        else
        {
            // To get a region's complement wrt the Universe, we iterate through each
            //  range and reverse each of their Lowerbounds and Upperbounds.  Then we
            //  assess the situation and assemble contiguous ranges and throw out
            //  the overlapping areas.

            // Swap the lower bound and upper bound for every range
            for(typename std::vector< Range<T> >::iterator iter(ret.m_ranges.begin());
                iter != ret.m_ranges.end();
                iter++)
            {
                bool m_lb_modified(iter->lb_modified);
                bool m_ub_modified(iter->ub_modified);
                T mem( iter->LowerBound() );

                iter->SetLowerBound( iter->UpperBound());
                iter->SetUpperBound( mem );
                iter->lb_modified = m_ub_modified;
                iter->ub_modified = m_lb_modified;

                if(iter->lb_modified)
                    iter->SetLowerBound(iter->LowerBound() + _p_MinimumResolution);
                if(iter->ub_modified)
                    iter->SetUpperBound(iter->UpperBound() - _p_MinimumResolution);
            }

            // Now go through one range at a time, merging/removing indexes as needed
            //  Append new ranges to the end of the list, remove items directly from
            //  the list (that's why we iterate backwards through the list)
            typename std::vector< Range<T> >::iterator cur(ret.m_ranges.end());
            for(int i = ret.m_ranges.size() - 1; 0 <= i; i--)
            {
                Range<T> *cur( &ret.m_ranges.at(i) );

                bool min_found(false), max_found(false);
                T min, max;

                // Find the closest neigboring bound to our upper and lower bounds
                for(typename std::vector< Range<T> >::iterator iter(ret.m_ranges.begin());
                    iter != ret.m_ranges.end();
                    iter++)
                {
                    if(*iter == *cur)
                    {
                        if(cur->lb_modified && cur->ub_modified &&
                                (cur->LowerBound() < cur->UpperBound() ||
                                 cur->LowerBound() == cur->UpperBound()))
                        {
                            if(min_found)
                                min = gMin(min, cur->UpperBound());
                            else
                            {
                                min = cur->UpperBound();
                                min_found = true;
                            }

                            if(max_found)
                                max = gMax(max, cur->LowerBound());
                            else
                            {
                                max = cur->LowerBound();
                                max_found = true;
                            }
                        }
                    }
                    else
                    {
                        if(cur->lb_modified)
                        {
                            T tmp(gMin(iter->LowerBound(), iter->UpperBound()));

                            if(cur->LowerBound() <= tmp)
                            {
                                if(min_found)
                                    min = gMin(min, tmp);
                                else
                                {
                                    min = tmp;
                                    min_found = true;
                                }
                            }
                        }

                        if(cur->ub_modified)
                        {
                            T tmp(gMax(iter->LowerBound(), iter->UpperBound()));

                            if(tmp <= cur->UpperBound())
                            {
                                if(max_found)
                                    max = gMax(max, tmp);
                                else
                                {
                                    max = tmp;
                                    max_found = true;
                                }
                            }
                        }
                    }
                }



                if(cur->lb_modified && min_found)
                {
                    if(cur->LowerBound() == min)
                    {
                        // Null out the lower bound, because it's already
                        // represented by another range
                        cur->lb_modified = false;
                    }
                    else
                    {
                        if(cur->ub_modified)
                        {
                            if(cur->UpperBound() != min)
                            {
                                // Our upper bound has already been set, so move
                                //  our lower bound to another range and append it
                                //  to the list
                                cur->lb_modified = false;
                                ret.m_ranges.push_back(Range<T>(cur->LowerBound(), min, _p_MinimumResolution));

                                // Cur gets invalidated by the last push_back,
                                //  so reset it
                                cur = &ret.m_ranges.at(i);
                            }
                        }
                        else
                        {
                            // Our upper bound has not been used yet, so we'll
                            //  set it to the location of the nearest boundary
                            cur->SetUpperBound(min);
                        }
                    }
                }

                if(cur->ub_modified && max_found)
                {
                    if(cur->UpperBound() == max)
                    {
                        cur->ub_modified = false;
                    }
                    else
                    {
                        if(cur->lb_modified)
                        {
                            if(cur->LowerBound() != max)
                            {
                                cur->ub_modified = false;
                                ret.m_ranges.push_back(Range<T>(max, cur->UpperBound(), _p_MinimumResolution));
                                cur = &ret.m_ranges.at(i);
                            }
                        }
                        else
                        {
                            cur->SetLowerBound(max);
                        }
                    }
                }



                if(cur->IsNull())
                {
                    // Erase the null Range
                    ret.m_ranges.erase(ret.m_ranges.begin() + i);
                }
            }
        }
    }
    else
    {
        // To get the relative complement of this in 'r' (r - this), we can express
        //  it as (this & ~r) which is equal to ~(~this | r)
        ret = ~(Region<T>::Complement() | r);
    }

    return ret;
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




// Here is an example of a class you might use.  It is a real number scale
class IntegerRange :
        public Range<int>
{
public:
    inline IntegerRange():Range<int>(1){}
    inline IntegerRange(int lb, int ub)     :Range<int>(lb, ub, 1){}
    inline IntegerRange(const Range<int> &r):Range<int>(r){}
};

class IntegerRegion :
        public Region<int>
{
public:
    inline IntegerRegion()  :Region<int>(1){}
    inline IntegerRegion(const Region<int> &other) :Region<int>(other){}
    inline IntegerRegion(const IntegerRange &other) :Region<int>(other){}
};


GUTIL_END_CORE_NAMESPACE;

#endif // LINEARSETALGEBRA_H
