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

#ifndef GUTIL_LIST_P_H
#define GUTIL_LIST_P_H

#include "gutil_macros.h"
#include "Core/exception.h"
#include "Core/Interfaces/ivoidwrappers.h"
GUTIL_BEGIN_CORE_NAMESPACE(DataObjects);


/** The type-independent base class for List. */
class list_p
{
public:

    class type_wrapper :
            public Interfaces::IVoidCopyer,
            public Interfaces::IVoidDeleter
    {
    public: virtual ~type_wrapper();
    };

    list_p(type_wrapper *);
    ~list_p();


protected:

    /** Deletes all items from the list.
        \note O(N)
    */
    void clear();

    inline GUINT32 count() const{ return m_size; }

    inline GUINT32 capacity() const{ return capacity(m_pageCount); }

    /** Reserves space for at least this many items. */
    void reserve(GUINT32);

    /** Returns the item at index i.  Throws IndexOutOfRangeException if out of range. */
    void *at(GUINT32) throw(IndexOutOfRangeException);
    /** Returns the item at index i.  Throws IndexOutOfRangeException if out of range. */
    void const*at(GUINT32) const throw(IndexOutOfRangeException);

    /** Returns a pointer to the void pointer at the given index. */
    void **pointer_at(GUINT32) throw(IndexOutOfRangeException);

    void insert(void const *, GUINT32);
    void remove(GUINT32);


private:

    type_wrapper *data_wrapper;

    /** A triple-pointer, which is an array of array of void *'s. */
    void ***d;

    GUINT32 m_size;
    int m_pageCount;

    inline static GUINT32 capacity(int n){ return ~(GINT32(0x80000000) >> (32 - (n + 1))); }

};


GUTIL_END_CORE_NAMESPACE;

#endif // GUTIL_LIST_P_H
