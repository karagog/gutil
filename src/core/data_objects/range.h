/*Copyright 2010-2015 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_RANGE_H
#define GUTIL_RANGE_H

#include <gutil/dlist.h>
#include <gutil/nullable.h>
#include <gutil/flexibletypecomparer.h>

NAMESPACE_GUTIL;


/** Describes one bound of a range/region.

    The bound can be null, and the value of the bound itself could be included in
    the range or not, depending on the value of the boolean

    In general, you don't instantiate your own bounds; they are created automatically
    inside the Range/Region implementation.  But you can if you want.

    \sa range.h
*/
template<class T>
class Bound
{
public:

    /** Creates a null bound */
    Bound() :m_includeValue(true){}

    /** Creates a bound initialized to the given value, which is optionally
        included in the bound itself by the 'include_value' variable
    */
    Bound(const T &value, bool include_value)
        :m_value(value), m_includeValue(include_value){}

    /** Returns the current value of the bound
        \note This value is only valid if the bound is not null
    */
    const T &Value() const{ return m_value.Value(); }

    /** Returns true if the bound value is included in the bound itself */
    bool ValueIncludedInBound() const{ return m_includeValue; }

    /** Returns true if the bound is null */
    bool IsNull() const{ return m_value.IsNull(); }

    /** Returns true if this bound equals the other */
    bool operator == (const Bound<T> &other) const{ return m_value == other.m_value; }
    /** Returns true if this bound does not equal the other */
    bool operator != (const Bound<T> &other) const{ return m_value != other.m_value; }

    /** Clears the bound value and sets it to null */
    void Clear(){ m_value.Clear(); m_includeValue = true; }

    /** Sets the value of the bound, as well as the inclusion flag */
    void SetValue(const T &val, bool include_value){
        m_value = val; m_includeValue = include_value;
    }


private:

    Nullable<T> m_value;
    bool m_includeValue;

};



/** Efficiently represents a range of items of type T.

    The class assumes that items of type T have a well-defined ordering between them, then it
    only needs to remember the start and the end to represent a potentially infinite range.

    The range can be bounded on either side, and if the lower bound is higher than the upper
    bound, then the range will be inverted (unbounded on either end, but with a part missing
    in the center)

    \sa range.h
*/
template <class T>
class Range
{
    enum __bound_type{
        __lower_bound,
        __upper_bound,
        __lower_and_upper_bound
    };
public:

    /** Creates a single-bound range with the given lower bound.

        \param lb The lower bound
        \param including_value Indicates whether the bound is inclusive. Passing
        false would declare that the bound itself should not be included in the range
    */
    static Range<T> CreateLowerBound(const T &lb, bool including_value = true){
        return Range<T>(lb, including_value, true);
    }
    /** Creates a single-bound range with the given lower bound */
    static Range<T> CreateLowerBound(const Bound<T> &lb){ return Range<T>(lb, true); }

    /** Creates a single-bound range with the given upper bound.

        \param ub The upper bound
        \param including_value Indicates whether the bound is inclusive. Passing
        false would declare that the bound itself should not be included in the range
    */
    static Range<T> CreateUpperBound(const T &ub, bool including_value = true){
        return Range<T>(ub, including_value, false);
    }
    /** Creates a single-bound range with the given upper bound */
    static Range<T> CreateUpperBound(const Bound<T> &ub){ return Range<T>(ub, false); }

    /** Creates a doubly-bound range with the given lower/upper bounds.

        \param lb The lower bound
        \param ub The upper bound
        \param include_lower_bound Indicates whether the lower bound is inclusive
        \param include_upper_bound Indicates whether the upper bound is inclusive
    */
    static Range<T> CreateDoubleBound(const T &lb, const T &ub,
                                             bool include_lower_bound = true,
                                             bool include_upper_bound = true){
        return Range<T>(lb, ub, include_lower_bound, include_upper_bound);
    }
    /** Creates a doubly-bound range with the given lower/upper bounds */
    static Range<T> CreateDoubleBound(const Bound<T> &lb, const Bound<T> &ub){
        return Range<T>(lb.Value(), ub.Value(), lb.ValueIncludedInBound(), ub.ValueIncludedInBound());
    }

    /** Constructs a range that represents all possible values */
    static Range<T> Universe(){ return Range<T>(false); }

    /** Constructs an empty range, which contains no values */
    static Range<T> EmptySet(){ return Range<T>(); }

    /** Constructs an empty range, which contains no values */
    Range() :m_null(true){}



    /** Returns a pointer to the lower bound value, or NULL if there is no such bound */
    Bound<T> const &LowerBound() const{ return m_LowerBound; }

    /** Returns a pointer to the upper bound value, or NULL if there is no such bound */
    Bound<T> const &UpperBound() const{ return m_UpperBound; }

    /** Returns whether the range has a lower bound */
    bool HasLowerBound() const{ return !m_LowerBound.IsNull(); }
    /** Returns whether the range has an upper bound */
    bool HasUpperBound() const{ return !m_UpperBound.IsNull(); }

    /** Sets the lower bound */
    void SetLowerBound(const T &lb, bool include_value_in_bound = true){
        SetLowerBound(Bound<T>(lb, include_value_in_bound));
    }
    /** Sets the lower bound */
    void SetLowerBound(const Bound<T> &b){
        m_LowerBound = b;
        m_null = false;
    }

    /** Sets the upper bound */
    void SetUpperBound(const T &ub, bool include_value_in_bound = true){
        SetUpperBound(Bound<T>(ub, include_value_in_bound));
    }
    /** Sets the upper bound */
    void SetUpperBound(const Bound<T> &b){
        m_UpperBound = b;
        m_null = false;
    }

    /** Clears both bounds and makes the range null */
    void Clear(){
        ClearLowerBound();
        ClearUpperBound();
        m_null = true;
    }

    /** Clears the lower bound */
    void ClearLowerBound(){
        m_LowerBound.Clear();
    }
    /** Clears the upper bound */
    void ClearUpperBound(){
        m_UpperBound.Clear();
    }

    /** Returns true if the range is bounded on both ends.
        \note This is not simply the opposite of IsUnbounded()
    */
    bool IsBounded() const{
        return IsEmpty() || (HasLowerBound() && HasUpperBound() && 0 >= CompareLowerAndUpperBounds(m_LowerBound, m_UpperBound));
    }

    /** Returns true if the range is goes to negative and positive infinity.
        \note This may be due to the range being equal to the Universe, or it could
        also come from the lower bound being greater than the upper bound.
        \note This is not simply the opposite of IsBounded()
    */
    bool IsUnbounded() const{ return IsUniverse() || (HasLowerBound() && HasUpperBound() && 0 < CompareLowerAndUpperBounds(LowerBound(), UpperBound())); }

    /** Returns true if the range is unbounded in the positive direction (no upper bound, but may or may not have lower bound) */
    bool IsUnboundedPositive() const{ return !IsEmpty() && !HasUpperBound(); }
    /** Returns true if the range is unbounded in the negative direction (no lower bound, but may or may not have upper bound) */
    bool IsUnboundedNegative() const{ return !IsEmpty() && !HasLowerBound(); }

    /** Returns true if the range is totally unbounded (equal to its universe) */
    bool IsUniverse() const{ return !IsEmpty() && !(HasLowerBound() || HasUpperBound()); }

    /** Returns true if the range has not been initialized (means it is an
        empty range; or the complement of the Universe())
    */
    bool IsEmpty() const{ return m_null; }

    /** Returns true if the range contains the given item */
    bool Contains(const T &t) const{
        bool satisfies_lower, satisfies_upper;
        if(this->IsEmpty())
            satisfies_lower = satisfies_upper = false;
        else{
            satisfies_lower = satisfies_upper = true;
            if(!this->IsUniverse()){
                int tmpcmp;
                if(this->HasLowerBound()){
                    tmpcmp = CompareValues(m_LowerBound.Value(), t);
                    satisfies_lower = 0 > tmpcmp || (m_LowerBound.ValueIncludedInBound() && 0 == tmpcmp);
                }
                if(this->HasUpperBound()){
                    tmpcmp =  CompareValues(t, m_UpperBound.Value());
                    satisfies_upper = 0 > tmpcmp || (m_UpperBound.ValueIncludedInBound() && 0 == tmpcmp);
                }
            }
        }

        bool ret;
        if(IsUnbounded())   ret = satisfies_lower || satisfies_upper;
        else                ret = satisfies_lower && satisfies_upper;
        return ret;
    }

    /** Sets this range equal to its complement */
    Range<T> &Complement(){
        if(IsEmpty()) operator = (Range<T>::Universe());
        else if(IsUniverse()) operator = (Range<T>::EmptySet());
        else
        {
            // Just swap the upper and lower bounds to get the compliment
            Bound<T> cpy(m_LowerBound);
            if(m_UpperBound.IsNull())   m_LowerBound.Clear();
            else                        m_LowerBound.SetValue(m_UpperBound.Value(), !m_UpperBound.ValueIncludedInBound());
            if(cpy.IsNull())            m_UpperBound.Clear();
            else                        m_UpperBound.SetValue(cpy.Value(), !cpy.ValueIncludedInBound());
        }
        return *this;
    }

    /** Returns the complement of this range */
    Range<T> Complemented() const{ Range<T> ret(*this); ret.Complement(); return ret; }

    /** Returns true if this is a subset of the other range */
    bool IsSubset(const Range<T> &other) const{ return IsSubset(*this, other); }

    /** Returns true if the first is a subset of the second range */
    static bool IsSubset(const Range<T> &first, const Range<T> &second){
        if(second.IsUniverse() || first.IsEmpty()) return true;
        else if(first.IsUniverse() || second.IsEmpty()) return false;

        else if(second.IsUnbounded())
            return (first.IsUnbounded() &&
                    0 <= CompareLowerBounds(first.LowerBound(), second.LowerBound()) &&
                    0 >= CompareUpperBounds(first.UpperBound(), second.UpperBound())) ||
                    (first.IsBounded() &&
                     (0 <= CompareLowerBounds(first.LowerBound(), second.LowerBound()) ||
                      0 >= CompareUpperBounds(first.UpperBound(), second.UpperBound()))) ||
                    (first.IsUnboundedPositive() && 0 <= CompareLowerBounds(first.LowerBound(), second.LowerBound())) ||
                    (first.IsUnboundedNegative() && 0 >= CompareUpperBounds(first.UpperBound(), second.UpperBound()));
        else if(second.IsUnboundedPositive())
            return (first.IsBounded() || first.IsUnboundedPositive()) &&
                    0 <= CompareLowerBounds(first.LowerBound(), second.LowerBound());
        else if(second.IsUnboundedNegative())
            return (first.IsBounded() || first.IsUnboundedNegative()) &&
                    0 >= CompareUpperBounds(first.UpperBound(), second.UpperBound());
        else // if(second.IsBounded())
            return first.IsBounded() &&
                    0 <= CompareLowerBounds(first.LowerBound(), second.LowerBound()) &&
                    0 >= CompareUpperBounds(first.UpperBound(), second.UpperBound());
    }

    /** Returns true if this is a superset of the other range */
    bool IsSuperset(const Range<T> &other){ return IsSuperset(*this, other); }

    /** Returns true if the first is a superset of the second range */
    bool IsSuperset(const Range<T> &first, const Range<T> &second){ return IsSubset(second, first); }


    /** Returns true if the ranges are equal
        Null ranges are never equal.
    */
    bool operator == (const Range<T> &o) const{
        return !IsEmpty() && !o.IsEmpty() &&
                (HasLowerBound() ? o.HasLowerBound() && 0 == CompareLowerBounds(m_LowerBound, o.m_LowerBound) : !o.HasLowerBound()) &&
                (HasUpperBound() ? o.HasUpperBound() && 0 == CompareUpperBounds(m_UpperBound, o.m_UpperBound) : !o.HasUpperBound());
    }
    /** Returns true if the ranges are not equal
        Null ranges are never equal.
    */
    bool operator != (const Range<T> &o) const{ return !this->operator == (o); }

    /** Returns the complement of this range */
    Range<T> operator ~ () const{ return this->Complemented(); }

    /** Returns true if this is a subset of the other range */
    bool operator <= (const Range<T> &other){ return IsSubset(*this, other); }
    /** Returns true if this is a superset of the other range */
    bool operator >= (const Range<T> &other){ return IsSuperset(*this, other); }

    /** For consistent value comparisons, this is the compare function
        everyone uses in the Range/Region implementation
    */
    static int CompareValues(const T &lhs, const T &rhs){
        return GUtil::Compare(lhs, rhs);
    }

    /** Compares the two lower bounds (neither can be null) */
    static int CompareLowerBounds(const Bound<T> &lhs, const Bound<T> &rhs){ return _compare_bounds(lhs, rhs, __lower_bound); }

    /** Compares the two upper bounds (neither can be null) */
    static int CompareUpperBounds(const Bound<T> &lhs, const Bound<T> &rhs){ return _compare_bounds(lhs, rhs, __upper_bound); }

    /** Compares the two upper bounds (neither can be null) */
    static int CompareLowerAndUpperBounds(const Bound<T> &lb, const Bound<T> &ub){ return _compare_bounds(lb, ub, __lower_and_upper_bound); }


    /** Returns true if this range intersects the other */
    bool Intersects(const Range<T> &other) const{ return Intersects(*this, other); }

    /** Returns true if the first range intersects the second */
    static bool Intersects(const Range<T> &first, const Range<T> &second){
        if(first.IsEmpty() || second.IsEmpty()) return false;
        else if(first.IsUniverse() || second.IsUniverse()) return true;

        int lb_cmp(CompareLowerBounds(first.LowerBound(), second.LowerBound()));
        int ub_cmp(CompareUpperBounds(first.UpperBound(), second.UpperBound()));
        int lb1_ub2(CompareLowerAndUpperBounds(first.LowerBound(), second.UpperBound()));
        int lb2_ub1(CompareLowerAndUpperBounds(second.LowerBound(), first.UpperBound()));

        if(first.IsUnboundedPositive())
            return second.IsUnboundedPositive() || (second.HasLowerBound() && 0 >= lb_cmp) || (second.HasUpperBound() && 0 >= lb1_ub2);
        else if(first.IsUnboundedNegative())
            return second.IsUnboundedNegative() || (second.HasUpperBound() && 0 <= ub_cmp) || (second.HasLowerBound() && 0 >= lb2_ub1);
        else if(first.IsUnbounded())
            return second.IsUnbounded() || second.IsUnboundedNegative() || second.IsUnboundedPositive() ||
                    0 >= lb_cmp || 0 >= lb1_ub2 || 0 <= ub_cmp || 0 >= lb2_ub1;

        // Both the first and second are bounded
        else if(second.IsBounded())
        {
            GASSERT(first.IsBounded());
            return (0 >= lb_cmp && 0 >= lb2_ub1) ||   // second LB in between first bounds
                    (0 <= ub_cmp && 0 >= lb1_ub2) ||  // second UB in between first bounds
                    (0 <= lb_cmp && 0 >= ub_cmp);     // second range encompasses first
        }

        // first is bounded, but not second, so we can call ourselves again and
        //  reverse the arguments to get the right answer
        else
            return Intersects(second, first);
    }


    /** Returns true if this range is adjacent to the other, but not intersecting.

        This is true only when two bounds are touching (share the same value) but
        only one bound is inclusive.  Therefore, if you join the ranges in a Union,
        the resultant can be represented in a single range
    */
    bool IsAdjacent(const Range<T> &other) const{ return IsAdjacent(*this, other); }

    /** Returns true if the ranges are adjacent, but not intersecting.

        This is true only when two bounds are touching (share the same value) but
        only one bound is inclusive.  Therefore, if you join the ranges in a Union,
        the resultant can be represented in a single range
    */
    static bool IsAdjacent(const Range<T> &first, const Range<T> &second){
        return (first.HasUpperBound() && second.HasLowerBound() &&
                0 == CompareValues(first.UpperBound().Value(), second.LowerBound().Value()) && (first.UpperBound().ValueIncludedInBound() ^ second.LowerBound().ValueIncludedInBound())) ||
                (first.HasLowerBound() && second.HasUpperBound() &&
                 0 == CompareValues(first.LowerBound().Value(), second.UpperBound().Value()) && (first.LowerBound().ValueIncludedInBound() ^ second.UpperBound().ValueIncludedInBound()));
    }


private:

    Bound<T> m_LowerBound;
    Bound<T> m_UpperBound;
    bool m_null;


    /** The outside world must use one of the static functions to construct a range */
    explicit Range(bool isNull) :m_null(isNull){}

    /** Used to construct single-bound ranges */
    Range(const T &bound, bool inc, bool lower_bound)
        :m_null(false){
        if(lower_bound) m_LowerBound.SetValue(bound, inc);
        else m_UpperBound.SetValue(bound, inc);
    }

    /** Used to construct single-bound ranges */
    Range(const Bound<T> &bound, bool lower_bound)
        :m_null(bound.IsNull()){
        if(!m_null){
            if(lower_bound) m_LowerBound = bound;
            else            m_UpperBound = bound;
        }
    }

    /** Used to construct two-bound ranges */
    Range(const T &lb, const T &ub, bool inc_lb, bool inc_ub)
        :m_LowerBound(lb, inc_lb),
          m_UpperBound(ub, inc_ub),
          m_null(false)
    {
        // Take care of special case which result in a Universe set
        if(0 == CompareValues(lb, ub) && (inc_lb ^ inc_ub)){
            m_LowerBound.Clear();
            m_UpperBound.Clear();
        }
    }

    static int _compare_bounds(const Bound<T> &lhs, const Bound<T> &rhs, __bound_type bt){
        int cmp( CompareValues(lhs.Value(), rhs.Value()) );

        // If the bounds are equal, then the outcome depends on their exclusivity
        if(0 == cmp)
        {
            // If one or the other bound is inclusive (but not both)
            if(lhs.ValueIncludedInBound() ^ rhs.ValueIncludedInBound()){
                if(lhs.ValueIncludedInBound() && !rhs.ValueIncludedInBound())
                    cmp = bt == __lower_bound ? -1 : 1;
                else if(rhs.ValueIncludedInBound() && !lhs.ValueIncludedInBound())
                    cmp = bt == __upper_bound ? -1 : 1;
            }

            // If both of the bounds are exclusive and we're comparing an LB with UB
            else if(bt == __lower_and_upper_bound && !lhs.ValueIncludedInBound())
                cmp = 1;
        }
        return cmp;
    }

};



/** This class builds off the concept of a range to support a region,
    which is a union of several ranges.

    Since all of the set operators can be represented in terms of Unions and Complements,
    these are the only operators which have an actual implementation.  Every other operation
    (Intersect, Difference, Symmetric Difference, Relative Complement, etc...) is done by
    chaining together combinations of Unions and Complements.

    \sa range.h
*/
template <class T>
class Region
{
public:

    /** Constructs an empty region */
    Region() :m_isUniverse(false){}

    /** Constructs a new region and initializes it to the range */
    Region(const Range<T> &r) :m_isUniverse(r.IsUniverse()){
        _maybe_push_range(r);
    }

    /** Copies the given region */
    Region(const Region<T> &o) :m_ranges(o.m_ranges), m_isUniverse(o.m_isUniverse) {}

    /** Copies the given region */
    Region<T> &operator = (const Region<T> &o){
        m_ranges = o.m_ranges;
        m_isUniverse = o.m_isUniverse;
        return *this;
    }

    /** Assigns this region to be equal to the given range */
    Region<T> &operator = (const Range<T> &o){
        m_ranges.Clear();
        m_isUniverse = o.IsUniverse();
        _maybe_push_range(o);
        return *this;
    }

    /** Creates an empty region */
    static Region<T> EmptySet(){ return Region<T>(); }

    /** Creates a region that covers the whole universe of values */
    static Region<T> Universe(){ return Region<T>(true); }

    /** Clears the region (afterwards it is null)
        \note Does not deallocate internal memory for the range.  There is no way to free the memory,
        short of deleting the Region.  This is for efficiency purposes
    */
    void Clear(){ m_ranges.Empty(); m_isUniverse = false; }

    /** Returns true if the region is empty (contains no values) */
    bool IsEmpty() const{ return !m_isUniverse && 0 == m_ranges.Size(); }

    /** Returns true if the region covers the whole universe of values */
    bool IsUniverse() const{ return m_isUniverse; }

    /** The independent ranges it takes to sum into this region.
        You can use this to iterate through the ranges, but you can't change them directly
    */
    const DList< Range<T> > &Ranges() const{ return m_ranges; }

    /** Returns an iterator pointing to the range which contains the given item.
        If no range contains the value, then an iterator pointing to the end of the
        range vector is returned
    */
    typename DList< Range<T> >::const_iterator RangeContaining(const T &item) const{
        typename DList< Range<T> >::const_iterator iter(m_ranges.begin());
        for(; iter != m_ranges.end(); ++iter)
        {
            // If we found the range which contains the item, break and return it
            if(iter->Contains(item))
                break;

            // For optimization, we can terminate early if we've gone past the last range
            //  which could possibly contain the item, as determined by its lower bound
            else if(0 < Range<T>::CompareValues(iter->LowerBound().Value(), item)){
                iter = m_ranges.end();
                break;
            }
        }
        return iter;
    }

    /** Removes the range at the given index */
    void RemoveRangeAt(GUINT32 indx){ m_ranges.RemoveAt(indx); }

    /** Removes the range pointed to by the iterator.
        The iterator must be valid, and must belong to this region
    */
    void RemoveRange(const typename DList< Range<T> >::const_iterator &iter){ RemoveRangeAt(iter.Index()); }

    /** Returns true if the item exists somewhere within the region */
    bool Contains(const T &item){
        return IsUniverse() || RangeContaining(item) != m_ranges.end();
    }

    /** Returns true if this range equals the other */
    bool Equals(const Region<T> &other) const{ return Equals(*this, other); }
    /** Returns true if the lhs range equals the rhs range */
    static bool Equals(const Region<T> &lhs, const Region<T> &rhs){
        return !(lhs.IsEmpty() ^ rhs.IsEmpty()) &&
                (lhs.IsEmpty() || Intersect(lhs, rhs.Complemented()).IsUniverse());
    }

    /** Returns true if this is a subset of the other */
    bool IsSubset(const Region<T> &other) const{ return IsSubset(*this, other); }
    /** Returns true if one is a subset of the other */
    static bool IsSubset(const Region<T> &one, const Region<T> &other){ return one.Equals(Intersect(one, other)); }

    /** Returns true if this is a superset of the other range */
    bool IsSuperset(const Region<T> &other) const{ return IsSuperset(*this, other); }
    /** Returns true if the first is a superset of the second range */
    static bool IsSuperset(const Region<T> &first, const Region<T> &second){ return IsSubset(second, first); }

    /** Sets this equal to the union of it with the given range
        \todo Optimize so you don't have to create a new region just to Unionize the range
    */
    Region<T> &Union(const Range<T> &r){ return Union(Region<T>(r)); }
    /** Sets this equal to the union of it with the other region */
    Region<T> &Union(const Region<T> &r);
    /** Returns the union of the two regions */
    static Region<T> Union(const Region<T> &one, const Region<T> &other){ Region<T> ret(one); return ret.Union(other); }

    /** Sets this equal to the intersection of this with the given range */
    Region<T> &Intersect(const Range<T> &other){ return Intersect(Region<T>(other)); }
    /** Sets this equal to the intersection of this with the other region */
    Region<T> &Intersect(const Region<T> &other){ return (Complement().Union(other.Complemented())).Complement(); }
    /** Returns the region of intersection between one region and the other */
    static Region<T> Intersect(const Region<T> &one, const Region<T> &other){ Region<T> ret(one); return ret.Intersect(other); }

    /** Sets this equal to its universal complement (Universe - this) */
    Region<T> &Complement();
    /** Sets this equal to the complement of this with respect to the other region */
    Region<T> &Complement(const Region<T> &r){ return r.IsUniverse() ? this->Complement() : this->operator = (Difference(r, *this)); }
    /** Returns the region of complement of one with respect to the other (other - one) */
    static Region<T> Complement(const Region<T> &one, const Region<T> &other){ Region<T> ret(one); return ret.Complement(other); }

    /** Returns the complement of this region (with respect to the universe) */
    Region<T> Complemented() const{ Region<T> ret(*this); return ret.Complement(); }

    /** Sets this equal to the difference between it and the given range */
    Region<T> &Difference(const Range<T> &other){ return this->Intersect(other.Complemented()); }
    /** Sets this equal to the difference between it and the other region */
    Region<T> &Difference(const Region<T> &other){ return this->Intersect(other.Complemented()); }
    /** Returns a region that is the difference between lhs and rhs */
    static Region<T> Difference(const Region<T> &lhs, const Region<T> &rhs){ Region<T> ret(lhs); return ret.Difference(rhs); }

    /** Sets this equal to the symmetric difference of it with respect to the given range */
    Region<T> &SymmetricDifference(const Range<T> &r){
        Region<T> intersection(*this);  intersection.Intersect(r);
        return this->Union(r).Difference(intersection);
    }
    /** Sets this equal to the symmetric difference of it with respect to the other region */
    Region<T> &SymmetricDifference(const Region<T> &r){
        Region<T> intersection( Intersect(*this, r) );
        return this->Union(r).Difference(intersection);
    }
    /** Returns the symmetric difference of the two regions */
    static Region<T> SymmetricDifference(const Region<T> &one, const Region<T> &other){ Region<T> ret(one); return ret.SymmetricDifference(other); }

    /** Obtain the difference between two regions */
    Region<T> operator - (const Region<T> &r) const{ return Difference(*this, r); }
    /** Obtain the difference between this region and the given range */
    Region<T> operator - (const Range<T> &r) const{ Region<T> ret(*this); ret.Difference(r); return ret; }
    /** Sets the current region equal to the difference of the two regions */
    Region<T> &operator -= (const Region<T> &r){ return this->Difference(r); }
    /** Sets the current region equal to the difference of this and the given range */
    Region<T> &operator -= (const Range<T> &r){ return this->Difference(r); }

    /** Obtain the union of two regions */
    Region<T> operator + (const Region<T> &r) const{ return Union(*this, r); }
    /** Obtain the union of this region with the given range */
    Region<T> operator + (const Range<T> &r) const{ Region<T> ret(*this); ret.Union(r); return ret; }
    /** Sets the current region equal to the union of the two regions */
    Region<T> &operator += (const Region<T> &r){ return this->Union(r); }
    /** Sets the current region equal to the union of this and the given range */
    Region<T> &operator += (const Range<T> &r){ return this->Union(r); }

    /** Obtain the union of two regions */
    Region<T> operator | (const Region<T> &r) const{ return Union(*this, r); }
    /** Obtain the union of this region with the given range */
    Region<T> operator | (const Range<T> &r) const{ Region<T> ret(*this); ret.Union(r); return ret; }
    /** Sets the current region equal to the union of the two regions */
    Region<T> &operator |= (const Region<T> &r){ return this->Union(r); }
    /** Sets the current region equal to the union of this and the given range */
    Region<T> &operator |= (const Range<T> &r){ return this->Union(r); }

    /** Obtain the intersection of two regions */
    Region<T> operator & (const Region<T> &r) const{ return Intersect(*this, r); }
    /** Obtain the intersection of this region with the given range */
    Region<T> operator & (const Range<T> &r) const{ Region<T> ret(*this); ret.Intersect(r); return ret; }
    /** Sets the current region equal to the intersection of the two regions */
    Region<T> &operator &= (const Region<T> &r){ return this->Intersect(r); }
    /** Sets the current region equal to the intersection of this and the given range */
    Region<T> &operator &= (const Range<T> &r){ return this->Intersect(r); }

    /** Obtain the symmetric difference of two regions */
    Region<T> operator ^ (const Region<T> &r) const{ return SymmetricDifference(*this, r); }
    /** Obtain the symmetric difference of this with the given range */
    Region<T> operator ^ (const Range<T> &r) const{ Region<T> ret(*this); ret.SymmetricDifference(r); return ret; }
    /** Sets the current region equal to the symmetric difference of the two regions */
    Region<T> &operator ^= (const Region<T> &r){ return this->SymmetricDifference(r); }
    /** Sets the current region equal to the symmetric difference of this and the given range */
    Region<T> &operator ^= (const Range<T> &r){ return this->SymmetricDifference(r); }

    /** Obtain the complement of the current region */
    Region<T> operator ~ () const{ return Region<T>::Complemented(); }

    /** Compare two sets for equality, or (A & ~B) == NULL */
    bool operator == (const Region<T> &other) const{ return Equals(*this, other); }
    /** Compare two sets for inequality, or (A & ~B) != NULL */
    bool operator != (const Region<T> &other) const{ return !Equals(*this, other); }

    /** Returns true if this region is included in the other region */
    bool operator <= (const Region<T> &other) const{ return IsSubset(*this, other); }
    /** Returns true if the other region is included in this region */
    bool operator >= (const Region<T> &other) const{ return IsSubset(other, this); }
    /** Returns true if this region is included in the given range */
    bool operator <= (const Range<T> &other) const{ return IsSubset(*this, other); }
    /** Returns true if the given range is included in this region */
    bool operator >= (const Range<T> &other) const{ return IsSubset(other, *this); }


private:

    DList< Range<T> > m_ranges;
    bool m_isUniverse;

    /** Merges the two ranges (Union operation) and stores the result in the first
        \note As a precondition for the merge, the ranges have to be intersecting.
        This is the only way that the result is guaranteed to fit in a Range object.
    */
    static void _merge_ranges(Range<T> &first, const Range<T> &second);

    /** Returns true if the first range is less than the second.
        \note The ranges cannot be intersecting
    */
    static bool _range_less_than(const Range<T> &lhs, const Range<T> &rhs){
        GASSERT(!Range<T>::Intersects(lhs, rhs));
        GASSERT(!lhs.IsUnbounded());
        GASSERT(!rhs.IsUnbounded());
        return !lhs.IsUnboundedPositive() &&
                ((lhs.IsUnboundedNegative() && 0 < Range<T>::CompareLowerAndUpperBounds(rhs.LowerBound(), lhs.UpperBound())) ||
                 0 < Range<T>::CompareLowerAndUpperBounds(rhs.LowerBound(), lhs.UpperBound()));
    }

    /** Runs a check on our initial range to split it into two if it's unbounded */
    void _check_init_range(){
        GASSERT(m_ranges.Size() == 1);
        if(!m_ranges.Front().IsUniverse() && m_ranges.Front().IsUnbounded()){
            Bound<T> lb(m_ranges.Front().LowerBound());
            Bound<T> ub(m_ranges.Front().UpperBound());
            m_ranges.Clear();
            m_ranges.PushBack(Range<T>::CreateUpperBound(ub));
            m_ranges.PushBack(Range<T>::CreateLowerBound(lb));
        }
    }

    /** Adds a range to our list if it's not empty and not the universe */
    void _maybe_push_range(const Range<T> &r){
        if(!r.IsUniverse() && !r.IsEmpty()){
            m_ranges.PushBack(r);
            _check_init_range();
        }
    }

    /** Constructs a region and lets you initialize it as the Universe */
    explicit Region(bool universe) :m_isUniverse(universe){}

};



template <class T>
Region<T> &Region<T>::Union(const Region<T> &other)
{
    // If the other region is empty, or we are the Universe, then the union is automatically this region
    if(this->IsUniverse() || other.IsEmpty()){ /* Do nothing and return this */ }

    // If this region is empty, or they are the Universe, then the union is automatically the other region
    else if(other.IsUniverse() || this->IsEmpty()){
        m_ranges = other.m_ranges;
        m_isUniverse = other.m_isUniverse;
    }

    // Absent any special case, we simply have to merge the two lists of ranges
    else
    {
        typename DList< Range<T> >::iterator our_iter( this->m_ranges.begin() );
        typename DList< Range<T> >::const_iterator their_iter( other.m_ranges.begin() );
        typename DList< Range<T> >::iterator last_iter;

        while(our_iter || their_iter)
        {
            // Our list finished, push one from theirs
            if(!our_iter){
                if(last_iter && (last_iter->Intersects(*their_iter) || last_iter->IsAdjacent(*their_iter)))
                    _merge_ranges(*last_iter, *their_iter);
                else{
                    this->m_ranges.PushBack(*their_iter);
                    last_iter = this->m_ranges.rbegin();
                }
                ++their_iter;
            }

            // Their list finished, push one from ours
            else if(!their_iter){
                if(last_iter && (last_iter->Intersects(*our_iter) || last_iter->IsAdjacent(*our_iter))){
                    _merge_ranges(*last_iter, *our_iter);
                    this->m_ranges.Remove(our_iter);
                }
                else{
                    SET_IF_NOT_EQUAL(last_iter, our_iter);
                    ++our_iter;
                }
            }


            // If we made it this far in the If-Then branches, then both our list
            //  and their list still have ranges in them

            // Our next range intersects the last range
            else if(last_iter && (last_iter->Intersects(*our_iter) || last_iter->IsAdjacent(*our_iter))){
                _merge_ranges(*last_iter, *our_iter);
                this->m_ranges.Remove(our_iter);
            }

            // Their next range intersects the last range
            else if(last_iter && (last_iter->Intersects(*their_iter) || last_iter->IsAdjacent(*their_iter))){
                _merge_ranges(*last_iter, *their_iter);
                ++their_iter;
            }

            // The next two ranges intersect each other (but not the last range)
            else if(our_iter->Intersects(*their_iter) || our_iter->IsAdjacent(*their_iter)){
                _merge_ranges(*our_iter, *their_iter);
                SET_IF_NOT_EQUAL(last_iter, our_iter);
                ++their_iter, ++our_iter;
            }

            // The two ranges do not intersect with each other, or the last range
            else{
                if(_range_less_than(*their_iter, *our_iter)){
                    this->m_ranges.Insert(*their_iter, our_iter);
                    ++their_iter;
                }
                SET_IF_NOT_EQUAL(last_iter, our_iter);
                ++our_iter;
            }


            // We can terminate early here, if we find even one merged range that
            //  becomes the Universe
            if(last_iter && last_iter->IsUniverse()){
                (*this) = Region<T>::Universe();
                break;
            }
        }
    }

    return *this;
}

template<class T>
void Region<T>::_merge_ranges(Range<T> &first, const Range<T> &second)
{
    GASSERT( !first.IsUniverse() );
    GASSERT( !second.IsUniverse() );
    GASSERT( !first.IsEmpty() );
    GASSERT( !second.IsEmpty() );
    GASSERT( !first.IsUnbounded() );
    GASSERT( !second.IsUnbounded() );
    GASSERT( Range<T>::Intersects(first, second) || Range<T>::IsAdjacent(first, second));

    if(first.IsUnboundedPositive())
    {
        if(second.IsUnboundedNegative()){
            // Since both ranges are intersecting as a precondition, we know the result is the Universe
            first.ClearLowerBound();
        }
        else{
            GASSERT(second.IsUnboundedPositive() || second.IsBounded());
            if(0 < Range<T>::CompareLowerBounds(first.LowerBound(), second.LowerBound()))
                first.SetLowerBound(second.LowerBound());
        }
    }
    else if(first.IsUnboundedNegative())
    {
        if(second.IsUnboundedPositive()){
            // Since both ranges are intersecting as a precondition, we know the result is the Universe
            first.ClearUpperBound();
        }
        else{
            GASSERT(second.IsUnboundedNegative() || second.IsBounded());
            if(0 > Range<T>::CompareUpperBounds(first.UpperBound(), second.UpperBound()))
                first.SetUpperBound(second.UpperBound());
        }
    }
    else // first.IsBounded()
    {
        GASSERT( first.IsBounded() );

        if(second.IsUnboundedPositive()){
            first.ClearUpperBound();
            if(0 < Range<T>::CompareLowerBounds(first.LowerBound(), second.LowerBound()))
                first.SetLowerBound(second.LowerBound());
        }
        else if(second.IsUnboundedNegative()){
            first.ClearLowerBound();
            if(0 > Range<T>::CompareUpperBounds(first.UpperBound(), second.UpperBound()))
                first.SetUpperBound(second.UpperBound());
        }
        else{ // second.IsBounded()
            GASSERT(second.IsBounded());
            if(0 < Range<T>::CompareLowerBounds(first.LowerBound(), second.LowerBound()))
                first.SetLowerBound(second.LowerBound());
            if(0 > Range<T>::CompareUpperBounds(first.UpperBound(), second.UpperBound()))
                first.SetUpperBound(second.UpperBound());
        }
    }
}





template <class T>
Region<T> &Region<T>::Complement()
{
    // The complement of the Universe is an empty region
    if(this->IsUniverse())              m_isUniverse = false;

    // The complement of an empty region is the Universe
    else if(this->IsEmpty())            m_isUniverse = true;

    // Absent a special case, we complement each range individually and merge
    //  them together to form the complement region
    else
    {
        // Complement each range individually
        for(auto &r : this->m_ranges) r.Complement();

        // Then combine/divide ranges where appropriate
        Bound<T> ub_mem;
        for(typename DList< Range<T> >::iterator iter( this->m_ranges.rbegin() );
            iter != this->m_ranges.rend();   --iter)
        {
            GASSERT(!iter->IsUniverse());
            GASSERT(!iter->IsEmpty());
            GASSERT(!iter->IsBounded());

            if(iter->HasLowerBound())
            {
                if(ub_mem.IsNull()){
                    if(iter->HasUpperBound()){
                        ub_mem = iter->UpperBound();
                        iter->ClearUpperBound();
                    }
                }
                else // mem not null
                {
                    // The lower bound should always be greater than the upper bound if it has one (Asserted at the beginning)
                    if(iter->HasUpperBound())
                    {
                        // Swap upper bounds with the next range and continue
                        Bound<T> tmp(iter->UpperBound());
                        iter->SetUpperBound(ub_mem);
                        ub_mem = tmp;
                    }
                    else  // !cur.HasUpperBound()
                    {
                        iter->SetUpperBound(ub_mem);
                        ub_mem.Clear();
                    }
                }
            }
            else  // !cur.HasLowerBound()
            {
                GASSERT(!iter->UpperBound().IsNull());
                GASSERT(ub_mem.IsNull());

                // If we're on the last range, then we can leave a lone upper bound,
                //  otherwise we move it to the range which comes before the current
                if(iter != this->m_ranges.begin()){
                    ub_mem = iter->UpperBound();
                    this->m_ranges.Remove(iter);
                }
            }
        }

        // Prepend the bound memory if it propagated out of the for loop
        if(!ub_mem.IsNull())
            this->m_ranges.PushFront(Range<T>::CreateUpperBound(ub_mem));
    }
    return *this;
}


END_NAMESPACE_GUTIL;


/** Returns the region of intersection between two ranges */
template<class T>GUtil::Region<T> operator * (const GUtil::Range<T> &one,
                                                                  const GUtil::Range<T> &two)
{ return GUtil::Region<T>(one) *= two; }

/** Returns the region of intersection between two ranges */
template<class T>GUtil::Region<T> operator * (const GUtil::Range<T> &one,
                                                                  const GUtil::Region<T> &two)
{ return GUtil::Region<T>(one) *= two; }

/** Returns the region of intersection between two ranges */
template<class T>GUtil::Region<T> operator & (const GUtil::Range<T> &one,
                                                                  const GUtil::Range<T> &two)
{ return GUtil::Region<T>(one) &= two; }

/** Returns the region of intersection between two ranges */
template<class T>GUtil::Region<T> operator & (const GUtil::Range<T> &one,
                                                                  const GUtil::Region<T> &two)
{ return GUtil::Region<T>(one) &= two; }

/** Returns the region of union between two ranges */
template<class T>GUtil::Region<T> operator + (const GUtil::Range<T> &one,
                                                                  const GUtil::Range<T> &two)
{ return GUtil::Region<T>(one) += two; }

/** Returns the region of union between two ranges */
template<class T>GUtil::Region<T> operator + (const GUtil::Range<T> &one,
                                                                  const GUtil::Region<T> &two)
{ return GUtil::Region<T>(one) += two; }

/** Returns the region of union between two ranges */
template<class T>GUtil::Region<T> operator | (const GUtil::Range<T> &one,
                                                                  const GUtil::Range<T> &two)
{ return GUtil::Region<T>(one) |= two; }

/** Returns the region of union between two ranges */
template<class T>GUtil::Region<T> operator | (const GUtil::Range<T> &one,
                                                                  const GUtil::Region<T> &two)
{ return GUtil::Region<T>(one) |= two; }

/** Returns the region of the difference between two ranges */
template<class T>GUtil::Region<T> operator - (const GUtil::Range<T> &one,
                                                                  const GUtil::Range<T> &two)
{ return GUtil::Region<T>(one) -= two; }

/** Returns the region of the difference between two ranges */
template<class T>GUtil::Region<T> operator - (const GUtil::Range<T> &one,
                                                                  const GUtil::Region<T> &two)
{ return GUtil::Region<T>(one) -= two; }

/** Returns the region of symmetric difference between two ranges */
template<class T>GUtil::Region<T> operator ^ (const GUtil::Range<T> &one,
                                                                  const GUtil::Range<T> &two)
{ return GUtil::Region<T>(one) ^= two; }

/** Returns the region of symmetric difference between two ranges */
template<class T>GUtil::Region<T> operator ^ (const GUtil::Range<T> &one,
                                                                  const GUtil::Region<T> &two)
{ return GUtil::Region<T>(one) ^= two; }


/** Returns true if the range is a subset of the region */
template<class T>bool operator <= (const GUtil::Range<T> &rng,
                                          const GUtil::Region<T> &reg)
{ return reg.IsSuperset(rng); }

/** Returns true if the range is a superset of the region */
template<class T>bool operator >= (const GUtil::Range<T> &rng,
                                          const GUtil::Region<T> &reg)
{ return reg.IsSubset(rng); }




/** \file

    An explanation of the Range/Region implementation.

    The Range/Region implementation is useful for representing a 1-dimensional
    universe of values, like a numberline or a timeline.  You can specify an
    arbitrarily-sized range of values simply by giving a lower bound and/or
    an upper bound.

    With this lower-bound/upper-bound implementation, you can efficiently represent
    large sets of numbers, only having to keep track of the bound values.  For example,
    if you wanted to use a normal STL set to contain all the numbers greater than 5,
    then you must individually insert every integer greater than 5.  In the case of
    floating point numbers, you would not feasibly be able to represent a range at all.
    With this 1-dimensional universe implementation, it is trivial to have a set
    which contains all values greater than 5, and it takes a minimal amount of
    memory (you only have to remember the lower bound).

    A Bound object is simply a value in the Universe (can also be NULL).  The Bound
    also knows if it is "inclusive" or "exclusive", which determines whether the
    value of the Bound itself is included in the Range or not (i.e. Greater-than 5,
    or Greater-than-or-equal-to 5)

    The only way to create a Range object is through it's static factory functions.
    This is because it would be too complex to try to convey so much information
    just using constructors with enums/boolean flags.

    So a range can have a lower-bound, upper-bound or no bounds, but it is not
    capable of representing more complex (or disjoint) regions of the Universe.
    The region class exists for this purpose.  This is essentially just a
    list of ranges.  Using the Region class, you can represent regions like: all
    values between 0 and 5, and also every value greater than 100.

    All of the set arithmetic functions are implemented for both Range and Region,
    including Union, Intersect, Complement, Difference, Symmetric Difference.  For
    most of the operations, they will always output a Region, even if the inputs
    were ranges.

    \note Only the Union and Complement operations actually have core implementations;
    All the other operations are built from combinations of these core functions.
    For example, the Intersect function is implemented as a simple combination
    of Complement and Union calls: ~(~A U ~B)

*/



#endif // GUTIL_RANGE_H
