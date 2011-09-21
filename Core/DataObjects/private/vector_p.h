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

#ifndef GUTIL_VECTOR_P
#define GUTIL_VECTOR_P

#include "gutil_macros.h"
#include "Core/Interfaces/ivoidwrappers.h"
GUTIL_BEGIN_CORE_NAMESPACE(DataObjects);


/** Manages a simple vector of memory, which grows exponentially with powers of two.
    You call reserve() with the number of items you want to have, and it will reserve
    at least as many, only conducting memory reallocations when necessary.
*/
template<class T>class vector_p
{
public:

    inline vector_p()
        :m_data(0),
          m_capacity(0)
    {}
    inline explicit vector_p(int capacity)
        :m_data(0),
          m_capacity(capacity < 0 ? 0 : capacity)
    {
        if(m_capacity) reserve(m_capacity);
    }
    inline ~vector_p(){ reserve(0); }

    /** Reserves room for at least this many items. */
    void reserve(int n)
    {
        int new_capacity( capacity(n) );
        if(new_capacity == m_capacity)
            return;

        m_capacity = new_capacity;

        T *cpy( m_data );
        m_data = m_capacity > 0 ? new T[m_capacity] : 0;
        if(cpy)
        {
            for(int i(m_capacity - 1); i >= 0; --i)
                m_data[i] = cpy[i];
            delete[] cpy;
        }
    }

    /** You can use this for convenience if you need to know what the capacity would be
        for n number of items.
    */
    inline static int capacity(int n){ return n <= 0 ? 0 : GEN_BITMASK_32( MSB32( n ) ); }

    /** The data pointer, whose length is always equal to Capacity.
        You can store whatever kind of data you want in it.
    */
    inline T* Data(){ return m_data; }
    /** The data pointer, whose length is always equal to Capacity.
        You can store whatever kind of data you want in it.
    */
    inline T* const Data() const{ return m_data; }

    /** The length of the data array, or how many void pointers you are capable of using. */
    inline int Capacity() const{ return m_capacity; }


protected:

    T *m_data;
    int m_capacity;

};


GUTIL_END_CORE_NAMESPACE;

#endif // GUTIL_VECTOR_P
