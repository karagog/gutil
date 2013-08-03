/*Copyright 2010-2013 George Karagoulis
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_BITVECTOR_H
#define GUTIL_BITVECTOR_H

#include "gutil_vector.h"

NAMESPACE_GUTIL1(DataObjects);


/** \file

    Declares a template specialization of Vector<bool>, and defines the
    BitVector typedef of said class
*/


/** A template specialization of Vector<bool>, which efficiently implements
    a vector of bits.

    A vector of bits is more-or-less an integer, where each individual bit
    represents a binary value in the array.  When the array size grows past the
    size of one integer, we allocate more integers in a contiguous array.

    This class' code is built into the core library, rather than left in a header file.
    This improves build time and code space.

    \note This is defined outside of the normal Vector class file, which
    is intentional.  You have to include the bitvector header to benefit from
    the improved implementation, and then you can refer to it as a BitVector
*/
template<>
class Vector<bool>
{
    Vector<GINT32> m_data;
    GUINT32 m_size;
public:

    /** Creates a null-capacity vector */
    inline Vector()
        :m_size(0)
    {}

    /** Capacity refers to the number of bits we are capable of representing in the bitvector. */
    inline explicit Vector(GUINT32 capacity)
        :m_data(capacity ? ((capacity - 1) >> 5) + 1 : 0),
          m_size(0)
    {}

    /** Initializes the vector to the given value */
    inline Vector(bool init_val, GUINT32 size)
        :m_data(init_val ? 0xFFFFFFFF : 0, size),
          m_size(size)
    {}

    /** Returns the value at the given index.
        Throws an exception if the index is beyond the array's end
    */
    inline bool At(GUINT32 indx) const{
        if(indx >= m_size) THROW_NEW_GUTIL_EXCEPTION(IndexOutOfRangeException);
        return operator [](indx);
    }

    /** Returns the value at the given index. */
    inline bool operator [] (GINT32 indx) const{ return operator[]((GUINT32)indx); }

    /** Returns the value at the given index. */
    inline bool operator [] (GUINT32 indx) const{
        return m_data[indx >> 5] & (1 << (indx & 0x1F));
    }

    /** Sets the bit to the given value */
    inline void Set(GUINT32 indx, bool val){
        GINT32 &target(m_data[indx >> 5]);
        GINT32 mask(1 << (indx & 0x1F));
        if(val) target |= mask;
        else    target &= ~mask;
    }

    /** Sets the bit to the given value, and returns the value the bit once had
        \note This is not atomic; simply a convenience function
    */
    inline bool FetchAndSet(GUINT32 indx, bool val){
        bool ret( At(indx) );
        Set(indx, val);
        return ret;
    }

    /** Adds the bit to the end of the vector */
    inline void PushBack(bool val){
        GUINT32 old_size( m_size++ );
        if((old_size >> 5) > m_data.Size())
            m_data.PushBack(0);
        Set(old_size, val);
    }

    /** Removes the bit from the end of the vector */
    inline void PopBack(){
        --m_size;
        if(((m_size - 1) >> 5) < m_data.Size())
            m_data.PopBack();
    }


    /** Iterates through the bits
     *  \todo Implement the iterator
    */
    class iterator
    {
    public:

    };




    /** This enum doesn't exist in the normal Vector implementation,
        so your code can check this at compile time to make absolute sure
        you're taking advantage of the boolean vector specialization.

        \note Put this in an assert statement rather than checking it in your
        final code.  This is really only useful for debugging.
        Note also that if the symbol BitVector is available, then the correct
        file must have already been included, so if the compiler doesn't complain
        about BitVector, you should be good to go.
    */
    enum{ IS_SPECIALIZED_BITVECTOR = 1 };

};


/** With this you can refer to Vector<bool> as a BitVector */
typedef Vector<bool> BitVector;


END_NAMESPACE_GUTIL1;

#endif // GUTIL_BITVECTOR_H
