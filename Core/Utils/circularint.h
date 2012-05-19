/*Copyright 2012 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_CIRCULARINT_H
#define GUTIL_CIRCULARINT_H

#include "gutil_globals.h"

NAMESPACE_GUTIL1(Utils);


/** Provides an integer that rolls over a max and min value.

    The integer has a defined range with minimum and maximum values.  If
    you increment (or decrement) past these limits, the integer automatically
    "rolls over" to the opposite boundary.

    For example, if the range of the CircularInt is [0,5] and the integer's
    value is currently 5, by incrementing this number the next value will be 0.

    \note The size of this object is 12 bytes, as compared to a normal integer's 4 bytes.
*/
class CircularInt
{
    GINT32 m_value;
    GINT32 m_min, m_max;
public:

    /** Creates a new CircularInt with the given min and max values.
        \note Nothing prevents you from setting nonsensical values.
    */
    inline CircularInt(GINT32 min, GINT32 max, GINT32 starting_val)
    { Reset(min, max, starting_val); }

    /** Resets the bounds and the value on the fly.
        \note Nothing prevents you from setting nonsensical values.
    */
    inline void Reset(GINT32 min, GINT32 max, GINT32 start_val){
        m_value = start_val;
        m_min = min;
        m_max = max;
    }

    /** Returns the minimum value of the integer. */
    inline GINT32 MinimumValue() const{ return m_min; }

    /** Returns the maximum value of the integer. */
    inline GINT32 MaximumValue() const{ return m_max; }

    /** Increments the value and rolls it over if greater than the max. */
    inline void Increment(){
        if(++m_value > m_max)
            m_value = m_min;
    }

    /** Decrements the value and rolls it over if less than the min. */
    inline void Decrement(){
        if(--m_value < m_min)
            m_value = m_max;
    }


    /** Prefix increment. */
    inline GINT32 operator ++(){
        Increment();
        return m_value;
    }
    /** Postfix increment. */
    inline GINT32 operator ++(int){
        GINT32 ret(m_value);
        Increment();
        return ret;
    }

    /** Prefix decrement. */
    inline GINT32 operator --(){
        Decrement();
        return m_value;
    }
    /** Postfix decrement. */
    inline GINT32 operator --(int){
        GINT32 ret(m_value);
        Decrement();
        return ret;
    }

    /** Increments the specified number of times. */
    inline GINT32 operator += (GUINT32 n){
        G_LOOP( n ){ Increment(); }
        return m_value;
    }

    /** Decrements the specified number of times. */
    inline GINT32 operator -= (GUINT32 n){
        G_LOOP( n ){ Decrement(); }
        return m_value;
    }

    /** Casts us as an integer. */
    inline operator GINT32 () const{ return m_value; }

};


END_NAMESPACE_GUTIL1;

#endif // GUTIL_CIRCULARINT_H
