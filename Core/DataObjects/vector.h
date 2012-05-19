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

#ifndef GUTIL_VECTOR_H
#define GUTIL_VECTOR_H

#include "gutil_macros.h"
#include "gutil_globals.h"
#include "gutil_exception.h"
#include "gutil_interfaces.h"
#include "gutil_flexibletypecomparer.h"
#include <new>
#include <limits.h>
#include <malloc.h>
#include <cstring>
NAMESPACE_GUTIL1(DataObjects);


/** Manages a simple vector of memory, which grows exponentially with powers of two.
    You call reserve() with the number of items you want to have, and it will reserve
    at least as many, only conducting memory reallocations when necessary.

    The Vector stores its total capacity, and also a pointer to the end of the array,
    in the memory locations directly preceding the base of the array.  You interact
    with these values via the Capacity(), Length() and DataEnd() functions.  If you
    have a pointer to an array of data managed by a Vector, then you can use the
    static versions of these functions to access this meta-data.

    \note The size of a Vector on the stack is only 4 bytes, the same as an int or
    a pointer.  This was a design choice, to allocate the necessary meta-data
    at runtime only when you actually start putting things in the vector.
*/
template<class T>class Vector
{
public:
    class iterator;
    class const_iterator;

    /** Constructs an empty vector. */
    inline Vector() :m_begin(NULL) {}

    /** Constructs an empty vector capable of holding the given number of items. */
    inline explicit Vector(GUINT32 capacity)
        :m_begin(NULL)
    {
        if(capacity != 0)
            Reserve(capacity);
    }


    /**  Constructs a vector of the given size, where all elements are copies of the provided object. */
    inline explicit Vector(const T &o, GUINT32 size = 1)
        :m_begin(NULL)
    {
        Reserve(size);
        set_length(size);
        T *cur(m_begin);
        for(GUINT32 i(0); i < size; ++i, ++cur)
            new(cur) T(o);
    }


    /**  Constructs a vector from the array of data. */
    inline Vector(T const*arr, GUINT32 size)
        :m_begin(NULL)
    {
        if(size > 0)
        {
            Reserve(size);
            set_length(size);
            T *cur(m_begin);
            for(GUINT32 i(0); i < size; ++i)
                new(cur++) T(*(arr++));
        }
    }


    /** Creates a vector with the elements from another vector, taken by iterating from
        iter_begin to iter_end.
    */
    inline Vector(const typename Vector<T>::const_iterator &iter_begin,
                        const typename Vector<T>::const_iterator &iter_end)
        :m_begin(NULL)
    {
        const GUINT32 sz( iter_end - iter_begin );
        if(sz > 0)
            Reserve(sz);
        T *ptr(m_begin);
        for(Vector<T>::const_iterator cur(iter_begin); cur != iter_end;)
            new(ptr++) T(*(cur++));
    }


    /** The copy constructor conducts a deep copy, invoking the copy constructors on each item. */
    Vector(const Vector<T> &o)
        :m_begin(NULL)
    {
        const GUINT32 sz( o.Length() );
        if(o.Capacity() > 0)
        {
            Reserve(o.Capacity());
            set_length(sz);
        }

        // Call the copy constructor for each item to initialize the memory
        T *cur( m_begin );
        T *ocur( o.m_begin );
        for(GUINT32 i(0); i < Length(); ++i)
            new(cur++) T(*(ocur++));
    }
    /** Assignment operator invokes our copy constructor after clearing the container. */
    Vector &operator = (const Vector<T> &o){
        Empty();
        Insert(o, 0);
        return *this;
    }
    inline ~Vector(){ Clear(); }

    /** Returns whether the vector is null (has not yet been initialized). */
    inline bool IsNull() const{ return !m_begin; }

    /** Returns whether the vector is empty (has no items in it, but the capacity may have been initialized). */
    inline bool IsEmpty() const{ return IsNull() || Size() == 0; }

    /** Insert the item at the position before the iterator.
        \note Invalidates all iterators, because the addition may cause a resize of the internal
        memory, which potentially moves the array.
    */
    inline void Insert(const T &item, const iterator &iter){ Insert(item, iter.Index()); }

    /** Insert the item at the index position.
        \note Invalidates all iterators, because the addition may cause a resize of the internal
        memory, which potentially moves the array.
    */
    void Insert(const T &item, GUINT32 indx)
    {
        const GUINT32 len( Length() );

        // Allocate more memory if we have to
        if(len == Capacity())
            Reserve(len + 1);

        T *dest( m_begin + indx );

        // Move the destination out of the way, if they're inserting anywhere but the end
        if(indx < len)
        {
            if(IsMovableType<T>::Value)
            {
                memmove(dest + 1, dest, (len - indx) * sizeof(T));
                new(dest) T(item);
            }
            else
            {
                T *cur(m_begin + len - 1);

                // Call the constructor on the memory location at the end
                new(m_begin + len) T(*cur);

                if(len > 0)
                {
                    for(GUINT32 i(len - 1); i > indx; --i, --cur)
                        *cur = *(cur - 1);
                }

                // Then assign the item to the proper location
                m_begin[indx] = item;
            }
        }
        else
        {
            // Call the copy constructor to initialize the memory at the end of the array
            new(dest) T(item);
        }

        set_length( len + 1 );
    }

    /** Insert the vector at the index position.
        \note Invalidates all iterators, because the addition may cause a resize of the internal
        memory, which potentially moves the array.
    */
    inline void Insert(const Vector<T> &vec, GUINT32 indx){
        Insert(vec.ConstData(), vec.Size(), indx);
    }

    /** Insert the array at the index position.
        \note Invalidates all iterators, because the addition may cause a resize of the internal
        memory, which potentially moves the array.
    */
    void Insert(const T *vec, GUINT32 size, GUINT32 indx)
    {
        const GUINT32 len( Length() );

        // Allocate more memory if we have to
        if((len + size) > Capacity())
            Reserve(Length() + size);

        T *dest( m_begin + indx );

        // Move the destination out of the way, if they're inserting anywhere but the end
        if(indx < len)
        {
            if(IsMovableType<T>::Value)
            {
                memmove(dest + size, dest, (len - indx) * sizeof(T));
                for(GUINT32 i(0); i < size; ++i)
                    new(dest++) T(*(vec++));
            }
            else
            {
                // Call the constructors on the memory location at the end
                for(GUINT32 i(1); i <= len && i <= size; ++i)
                    new(m_begin + len + size - i) T(*(m_begin + len - i));

                T *cur(m_begin + len - 1);
                if(len > 0)
                {
                    for(GUINT32 i(len - 1); i > (indx + size); --i, --cur)
                        *cur = *(cur - size);
                }

                // Then assign the items to the proper location
                for(GUINT32 i(0); i < size; ++i)
                {
                    if(i < len)
                        m_begin[indx + i] = vec[i];
                    else
                        new(m_begin + indx + i) T(vec[i]);
                }
            }
        }
        else
        {
            // Call the copy constructors to initialize the memory at the end of the array
            for(GUINT32 i(0); i < size; ++i)
                new(dest++) T(*(vec++));
        }

        set_length(len + size);
    }

    /** Remove the item pointed to by the iterator.

        \note Invalidates the iterator positioned at the last element, and all other iterators
        after the input would notice their current element has shifted
    */
    inline void Remove(const const_iterator &iter){ RemoveAt(iter.Index()); }

    /** Remove the item pointed to by the iterator.

        \note Invalidates the iterator positioned at the last element, and all other iterators
        after the input would notice their current element has shifted
    */
    inline void Remove(const iterator &iter){ RemoveAt(iter.Index()); }

    /** Remove the given number of items starting at the index location.

        \note Invalidates the iterator positioned at the last element, and all other iterators
        after the input would notice their current element has shifted
        \note O(1) if you remove from the end.  O(N) otherwise.
    */
    void RemoveAt(GUINT32 indx, GUINT32 num = 1)
    {
        const GUINT32 len( Length() );

        if(len == 0 || indx + num > len)
            THROW_NEW_GUTIL_EXCEPTION(IndexOutOfRangeException);

        T *const targ( m_begin + indx );

        // Call the destructors on the items
        for(GUINT32 i(0); i < num; ++i)
            (targ + i)->~T();

        // Copy all the following items over 1
        if(IsMovableType<T>::Value)
            memmove(targ, targ + num, (len - (indx + num)) * sizeof(T));
        else
        {
            // If T is not a primitive type then this is potentially really expensive!!

            // First shift the subsequent items over by the number of items removed
            {
                T *cur( targ );

                // Have to call the constructor on the items we removed, because we already destructed
                //  these memory location above
                for(GUINT32 i(indx); (i < indx + num) && (i < len - num); ++i, ++cur)
                    new(cur) T(*(cur + num));

                // Then for the rest of the move, we use the assignment operator, because those
                //  items have already been initialized.
                for(GUINT32 i(indx + num); i < len - num; ++i, ++cur)
                    *cur = *(cur + num);
            }

            // And on the items at the end we call the destructor
            for(GUINT32 i(GUtil::Max(len - num, indx + num)); i < len; ++i)
                (m_begin + i)->~T();
        }

        set_length(len - num);
    }

    /** Removes the first instance of the item.  The type T must have a comparison operator.
        \note O(n)
    */
    inline void RemoveOne(const T &o)
    {
        for(iterator iter( begin() ); iter != end(); ++iter)
            if(o == *iter)
            {
                Remove(iter);
                break;
            }
    }

    /** Removes the last instance of the item.  The type T must have a comparison operator.
        \note O(n)
    */
    inline void RemoveLast(const T &o)
    {
        for(iterator iter( rbegin() ); iter != rend(); --iter)
            if(o == *iter)
            {
                Remove(iter);
                break;
            }
    }

    /** Removes all instances of the item.  The type T must have a comparison operator.
        \note O(n)
    */
    inline void RemoveAll(const T &o)
    {
        for(iterator iter( rbegin() ); iter != rend(); --iter)
            if(o == *iter)
                Remove(iter);
    }

    /** Pushes the item on the back of the vector */
    inline void PushBack(const T &o){ Insert(o, Length()); }

    /** Pushes the other vector on the back of this one */
    inline void PushBack(const Vector<T> &v){ Insert(v, Length()); }

    /** Removes the item on the back of the list. */
    inline void PopBack(){ RemoveAt(Length() - 1); }

    /** Reserves room for at least this many items.
        Pass 0 to free all memory.
    */
    inline void Reserve(GUINT32 n){ ReserveExactly( _capacity(n) ); }

    /** Reserves room for exactly this number of items. */
    void ReserveExactly(GUINT32 new_capacity)
    {
        const GUINT32 len( Length() );
        if(new_capacity < Capacity())
        {
            // Need to call the destructors on the items we're (potentially) deleting
            if(new_capacity < len)
            {
                for(T *targ( m_begin + new_capacity ); targ != DataEnd(); ++targ)
                    targ->~T();
                set_length( new_capacity );
            }
        }
        else if(new_capacity == Capacity())
            // No need to reallocate
            return;

        GUINT32 new_size_in_bytes(new_capacity * sizeof(T));
        if(new_capacity > 0)
            new_size_in_bytes += sizeof(GUINT32) + sizeof(T*);

        void *real_begin(m_begin);
        if(m_begin)
            real_begin = reinterpret_cast<T **>(real_begin) - 2;

        if(IsMovableType<T>::Value)
        {
            // As an optimization for primitive types (ones that are not affected by binary moves)
            //  we call realloc, because a hidden memory relocation doesn't affect our type.
            real_begin = realloc(real_begin, new_size_in_bytes);

            if(new_size_in_bytes > 0 && real_begin == NULL)
                THROW_NEW_GUTIL_EXCEPTION(BadAllocationException);

            if(real_begin == NULL)
                m_begin = NULL;
            else
                m_begin = reinterpret_cast<T *>( reinterpret_cast<T **>(real_begin) + 2 );
        }
        else
        {
            if(new_size_in_bytes > 0)
            {
                // Have to manually reallocate and call the copy constructors, because a complex
                //  type may be dependent on their memory locations (self-pointers are one example)
                T *backup( m_begin );
                const GUINT32 len( Length() );
                void *new_begin( malloc(new_size_in_bytes) );

                if(new_begin == NULL)
                    THROW_NEW_GUTIL_EXCEPTION(BadAllocationException);

                T *cur( m_begin = reinterpret_cast<T *>( reinterpret_cast<GUINT32 *>(new_begin) + 2) );
                if(backup)
                {
                    // Copy the items from the backup copy to the new vector
                    for(GUINT32 i(0); i < len; ++i)
                        new(cur++) T(*(backup + i));

                    // Have to destruct the items from the backup vector
                    for(GUINT32 i(0); i < len; ++i)
                        (backup++)->~T();
                }
            }

            if(real_begin)
                free(real_begin);
            if(new_size_in_bytes == 0)
                m_begin = NULL;
        }

        if(new_capacity > 0)
            _set_capacity( new_capacity );

        // We have to do this every time, because the end pointer changes with
        //  the start pointer
        set_length( len );
    }

    /** Resizes the vector.  If the new size is larger than the current, then the default
        object will be copied to give a value to the newly created objects.
    */
    void Resize(GUINT32 new_size, const T &default_object = T()){
        if(new_size > Capacity())
            Reserve(new_size);
        if(new_size < Length())
        {
            const Vector<T>::iterator iter( begin() + new_size );
            for(GUINT32 i(Length()); i > new_size; --i)
                Remove(iter);
        }
        else if(new_size > Length())
        {
            T *cur(m_begin + Length());
            for(GUINT32 i(Length()); i < new_size; ++i)
                new(cur++) T(default_object);
        }
        set_length(new_size);
    }

    /** Accesses the data at the given index.
        \note Does not do any bounds checking.
    */
    inline const T &operator [](GUINT32 i) const{ return m_begin[i]; }
    /** Accesses the data at the given index.
        \note Does not do any bounds checking.
    */
    inline const T &operator [](GINT32 i) const{ return m_begin[i]; }
    /** Accesses the data at the given index.
        \note Does not do any bounds checking.
    */
    inline T &operator [](GUINT32 i){ return m_begin[i]; }
    /** Accesses the data at the given index.
        \note Does not do any bounds checking.
    */
    inline T &operator [](GINT32 i){ return m_begin[i]; }

    /** Returns a pointer to the item at the given offset. */
    inline const T *operator + (GUINT32 offset) const{ return m_begin + offset; }
    /** Returns a pointer to the item at the given offset. */
    inline const T *operator + (GINT32 offset) const{ return m_begin + offset; }
    /** Returns a pointer to the item at the given offset. */
    inline T *operator + (GUINT32 offset){ return m_begin + offset; }
    /** Returns a pointer to the item at the given offset. */
    inline T *operator + (GINT32 offset){ return m_begin + offset; }

    /** Accesses the data at the given index.
        \note Checks the index and throws an exception if it is out of bounds
    */
    inline T &At(GUINT32 i){
        if(Length() == 0 || i >= Length())
            THROW_NEW_GUTIL_EXCEPTION(IndexOutOfRangeException);
        return m_begin[i];
    }
    /** Accesses the data at the given index.
        \note Checks the index and throws an exception if it is out of bounds
    */
    inline const T &At(GUINT32 i) const{
        if(Length() == 0 || i >= Length())
            THROW_NEW_GUTIL_EXCEPTION(IndexOutOfRangeException);
        return m_begin[i];
    }

    /** Returns a pointer to the start of the array. */
    inline const T *ConstData() const{ return m_begin; }
    /** Returns a pointer to the start of the array. */
    inline T *Data(){ return m_begin; }

    /** Returns a pointer to the end of the vector, which is 1 past
        the last item in the vector.  Do not dereference this pointer, it
        is only to allow quick iteration through the list.
    */
    inline T *DataEnd() const{ return DataEnd(m_begin); }

    /** Returns a pointer to the end of the vector, which is 1 past
        the last item in the vector.  Do not dereference this pointer, it
        is only to allow quick iteration through the list.
        \param begin Must be a pointer to an array managed by the Vector class,
        otherwise it will certainly crash your program.
    */
    inline static T *DataEnd(T *begin){ return begin ? *(reinterpret_cast<T**>(begin) - 1) : NULL; }

    /** Clears all items and frees all memory. */
    inline void Clear(){ Reserve(0); }

    /** Clears all items but retains the same capacity. */
    inline void Empty(){
        if(m_begin)
        {
            T *cur( m_begin );
            T *e( m_begin + Length() );
            while(cur != e){
                cur->~T();
                ++cur;
            }
            set_length(0);
        }
    }

    /** The current length of the vector. */
    inline GUINT32 Length() const{ return Length(m_begin); }

    /** The current length of an array which was allocated by the vector.
        \note This only works because the Vector class stores the length of the
        array just before the 0 index.  So you can't pass just any old array into it.
    */
    static inline GUINT32 Length(T *vec){
        return vec ? *(reinterpret_cast<T **>(vec) - 1) - vec : 0;
    }

    /** The current length of the vector. */
    inline GUINT32 Size() const{ return Length(); }
    /** The current length of the vector. */
    inline GUINT32 Count() const{ return Length(); }

    /** How many items of type T we are capable of holding. */
    inline GUINT32 Capacity() const{ return Capacity(m_begin); }

    /** How many items of type T the array is capable of holding.
        \param begin A pointer to the base of an array managed by a Vector
    */
    inline static GUINT32 Capacity(T *begin){
        return begin ?
                    *(reinterpret_cast<GUINT32 *>(
                          reinterpret_cast<GINT8 *>(begin) - sizeof(T *) - sizeof(GUINT32)))
                  : 0;
    }

    /** Conducts a linear search for the first instance of the item
        starting at the given index, and using the == operator to test equality.
        \return The index on success, UINT_MAX on failure
    */
    inline GUINT32 IndexOf(const T &item, GUINT32 start = 0) const{
        GUINT32 ret( UINT_MAX );
        T *cur( m_begin + start );
        T *const e( m_begin + Length() );
        while(cur < e)
        {
            if(*cur == item)
            {
                ret = cur - m_begin;
                break;
            }
            ++cur;
        }
        return ret;
    }

    /** Conducts a linear search for the first instance of the item
        starting at the given index, and using the == operator to test equality.
        \return The index on success, UINT_MAX on failure
    */
    inline GUINT32 LastIndexOf(const T &item, GUINT32 start = UINT_MAX) const{
        GUINT32 ret(UINT_MAX);
        if(Length() > 0)
        {
            if(start == UINT_MAX)
                start = Length() - 1;
            T *cur( m_begin + start );
            T *const e( m_begin - 1 );
            while(cur < e)
            {
                if(*cur == item)
                {
                    ret = cur - m_begin;
                    break;
                }
                --cur;
            }
        }
        return ret;
    }

    /** Returns true if we hold a matching item */
    inline bool Contains(const T &i) const{ return UINT_MAX != IndexOf(i); }

    /** Sorts the vector using the given sorting algorithm.

        \param ascending The results are sorted ascendingly or descendingly
        according to this flag.

        \param comparer You can supply your own compare function, in case
        the less-than operator is not good enough for you.

        \param sort_type The sorting algorithm to use.
    */
    inline void Sort(bool ascending = true, const GUtil::Interfaces::IComparer<T> &comparer = GUtil::DefaultComparer<T>(), GUtil::SortTypeEnum sort_type = MergeSort){
        switch(sort_type)
        {
        case GUtil::MergeSort:
        {
            Vector<T> buffer;
            buffer.ReserveExactly(Length());
            _merge_sort(begin(), end(), ascending, buffer, comparer);
        }
            break;
        default:
            THROW_NEW_GUTIL_EXCEPTION(NotImplementedException);
        }
    }


    /** Iterates through the vector. */
    class iterator
    {
        friend class const_iterator;
    protected:
        T *current;
        T *m_begin;
        T *m_end;
    public:
        inline iterator()
            :current(0),
              m_begin(0),
              m_end(0)
        {}
        inline iterator(T *begin, T *cur)
            :current(cur),
              m_begin(begin),
              m_end(Vector<T>::DataEnd(begin))
        {}

        inline T &operator *() const{ return *current; }
        inline T *operator ->() const{ return current; }

        inline GUINT32 Index() const{ return current ? current - m_begin : 0; }
        inline T *Current() const{ return current; }

        inline iterator &operator ++(){ ++current; return *this; }
        inline iterator operator ++(int){ iterator ret(*this); ++current; return ret;}
        inline iterator &operator +=(GUINT32 n){ current += n; return *this; }
        inline iterator operator +(GUINT32 n) const{ iterator ret(*this); ret.current += n; return ret; }

        inline iterator &operator --(){ --current; return *this; }
        inline iterator operator --(int){ iterator ret(*this); --current; return ret;}
        inline iterator &operator -=(GUINT32 n){ current -= n; return *this; }
        inline iterator operator -(GUINT32 n) const{ iterator ret(*this); ret.current -= n; return ret;}

        /** Returns the difference between iterators. */
        inline GUINT32 operator - (const iterator &other) const{
            if(m_begin != other.m_begin || current < other.current)
                return UINT_MAX;
            return current - other.current;
        }

        inline bool operator == (const iterator &o){ return m_begin == o.m_begin && current == o.current; }
        inline bool operator != (const iterator &o){ return !operator == (o); }
        inline bool operator == (const const_iterator &o){ return m_begin == o.m_begin && current == o.current; }
        inline bool operator != (const const_iterator &o){ return !operator == (o); }

        /** Returns whether the iterator is valid. */
        inline operator bool() const{ return current && m_begin <= current && current < m_end; }
    };

    /** Iterates through the vector, but also guarantees that it won't modify the vector. */
    class const_iterator
    {
        friend class iterator;
    protected:
        T *current;
        T *m_begin;
        T *m_end;
    public:
        inline const_iterator()
            :current(0),
              m_begin(0),
              m_end(0)
        {}
        inline const_iterator(T *begin, T *cur)
            :current(cur),
              m_begin(begin),
              m_end(Vector<T>::DataEnd(begin))
        {}
        inline const_iterator(const const_iterator &iter)
            :current(iter.current),
              m_begin(iter.m_begin),
              m_end(iter.m_end)
        {}
        inline const_iterator(const iterator &iter)
            :current(iter.current),
              m_begin(iter.m_begin),
              m_end(iter.m_end)
        {}

        inline T &operator *(){ return *current; }
        inline T const &operator *() const{ return *current; }
        inline T *operator ->(){ return current; }
        inline T const*operator ->() const{ return current; }

        inline GUINT32 Index() const{ return current ? current - m_begin : 0; }
        inline const T *Current() const{ return current; }

        inline const_iterator &operator ++(){ ++current; return *this; }
        inline const_iterator operator ++(int){ const_iterator ret(*this); ++current; return ret;}
        inline const_iterator &operator +=(GUINT32 n){ current += n; return *this; }
        inline const_iterator operator +(GUINT32 n) const{ const_iterator ret(*this); ret.current += n; return ret; }

        inline const_iterator &operator --(){ --current; return *this; }
        inline const_iterator operator --(int){ const_iterator ret(*this); --current; return ret;}
        inline const_iterator &operator -=(GUINT32 n){ current -= n; return *this; }
        inline const_iterator operator -(GUINT32 n) const{ const_iterator ret(*this); ret.current -= n; return ret;}

        /** Returns the difference between iterators. */
        inline GUINT32 operator - (const const_iterator &other) const{
            if(m_begin != other.m_begin || current < other.current)
                return UINT_MAX;
            return current - other.current;
        }

        inline bool operator == (const const_iterator &o){ return m_begin == o.m_begin && current == o.current; }
        inline bool operator != (const const_iterator &o){ return !operator == (o); }
        inline bool operator == (const iterator &o){ return m_begin == o.m_begin && current == o.current; }
        inline bool operator != (const iterator &o){ return !operator == (o); }

        /** Returns whether the iterator is valid. */
        inline operator bool() const{ return current && m_begin <= current && current < m_end;; }
    };

    inline iterator begin(){ return iterator(m_begin, m_begin); }
    inline const_iterator begin() const{ return const_iterator(m_begin, m_begin); }
    inline iterator end(){ return iterator(m_begin, DataEnd()); }
    inline const_iterator end() const{ return const_iterator(m_begin, DataEnd()); }

    inline iterator rbegin(){ return iterator(m_begin, DataEnd() - 1); }
    inline const_iterator rbegin() const{ return const_iterator(m_begin, DataEnd() - 1); }
    inline iterator rend(){ return iterator(m_begin, m_begin - 1); }
    inline const_iterator rend() const{ return const_iterator(m_begin, m_begin - 1); }

    /** Returns the item at the front of the list */
    inline T &Front(){ return *m_begin; }
    /** Returns the item at the front of the list */
    inline T const &Front() const{ return *m_begin; }

    /** Returns the item at the back of the list */
    inline T &Back(){ return *(DataEnd() - 1); }
    /** Returns the item at the back of the list */
    inline T const &Back() const{ return *(DataEnd() - 1); }

    /** Returns true if the vectors are equal.
        Vector equality is established by checking each individual element from
        this vector with the corresponding element in the other vector and evaluating
        the class' own == operator.
        \note O(N)
    */
    bool operator == (const Vector<T> &o) const{
        bool res(false);
        const T *cur1(ConstData()), *cur2(o.ConstData());
        if(Length() == o.Length() && static_cast<bool>(cur1) == static_cast<bool>(cur2))
        {
            res = true;
            while(cur1 != DataEnd())
            {
                if(*cur1 != *cur2)
                {
                    res = false;
                    break;
                }
                ++cur1, ++cur2;
            }
        }
        return res;
    }
    /** Returns true if the vectors are not equal.
        Vector equality is established by checking each individual element from
        this vector with the corresponding element in the other vector and evaluating
        the class' own == operator.
        \note O(N)
    */
    inline bool operator != (const Vector<T> &o) const{ return !operator == (o); }

    /** Pushes an item on a logical stack, with appealing syntax. */
    inline Vector<T> &operator << (const T &item){ PushBack(item); return *this; }

    /** Pops the top item from a logical stack and copies it into the given variable */
    inline Vector<T> &operator >> (T &cpy){ cpy = *rbegin(); PopBack(); return *this; }

    /** Does a constant-time data swap between vectors.

        After calling this, lhs will own rhs' data, and vice versa.  It is
        simply a pointer swap.

        \note After calling, all iterators for both vectors will still be valid,
        they will only reference the opposite Vector's data.
    */
    inline static void Swap(Vector<T> &lhs, Vector<T> &rhs){
        T *tmp( lhs.m_begin );
        lhs.m_begin = rhs.m_begin;
        rhs.m_begin = tmp;
    }


protected:

    /** Subclasses can use this convenience function as a setter for the length variable.
        \note You should only manually use this with POD types, because if you
        have a vector of more complex classes then some of their destructors may not get called.
    */
    inline void set_length(GUINT32 len){
        // Remembers a pointer to the end of the vector
        if(m_begin)
            *(reinterpret_cast<T **>(m_begin) - 1) = m_begin + len;
    }


private:

    T *m_begin;

    inline void _set_capacity(GUINT32 cap){ *(reinterpret_cast<GUINT32 *>(m_begin) - 2) = cap; }

    inline static int _capacity(int n){ return n <= 0 ? 0 : GEN_BITMASK_32( FSB32( n ) ); }

    void _merge_sort(const iterator &b, const iterator &e, bool ascending, Vector<T> &buffer, const Interfaces::IComparer<T> &cmp){
        GUINT32 diff( e - b );
        if(diff == UINT_MAX);
        else if(diff == 2)
        {
            T *last( e.Current() - 1 );
            if((ascending && 0 < cmp(*b, *last)) ||
               (!ascending && 0 > cmp(*b, *last)))
            {
                T cpy(*b);
                *b = *last;
                *last = cpy;
            }
        }
        else if(diff > 2)
        {
            iterator m(b + (diff / 2));

            // Sort the left and right halves of the list
            _merge_sort(b, m, ascending, buffer, cmp);
            _merge_sort(m, e, ascending, buffer, cmp);

            // Join the two halves, which are already sorted
            iterator i1(b), i2(m);
            while((i1 != m) && (i2 != e))
            {
                if((ascending && 0 < cmp(*i1, *i2)) ||
                   (!ascending && 0 > cmp(*i1, *i2)))
                {
                    buffer.PushBack(*i2);
                    ++i2;
                }
                else
                {
                    buffer.PushBack(*i1);
                    ++i1;
                }
            }
            while(i1 != m)
            {
                buffer.PushBack(*i1);
                ++i1;
            }
            while(i2 != e)
            {
                buffer.PushBack(*i2);
                ++i2;
            }

            // Copy the now-sorted buffer back to the original
            i1 = b;
            i2 = buffer.begin();
            while(i1 != e)
            {
                *i1 = *i2;
                ++i1, ++i2;
            }

            // Clear the items in the buffer
            buffer.Empty();
        }
    }

};


/** Provides an "attachable" stack interface to the vector. */
template<class T>class VectorStack : public Stack<T>
{
    GUTIL_DISABLE_COPY(VectorStack<T>)
public:

    /** Attaches the stack interface to the vector.
        We will not delete your vector; that is up to you, because you may attach multiple
        interfaces to the same vector.
    */
    inline VectorStack(Vector<T> *vec) :m_vector(vec), m_delete(false){}

    /** Creates a new Vector with a stack interface.
        Use this if you don't want to supply a pre-existing vector object.
    */
    inline VectorStack() :m_vector(new Vector<T>), m_delete(true){}

    inline ~VectorStack(){ if(m_delete) delete m_vector; }


    /** Satisfies the Stack abstract interface. */
    void Push(const T &i){ m_vector->PushBack(i); }

    /** Satisfies the Stack abstract interface. */
    void Pop(){ m_vector->PopBack(); }

    /** Satisfies the Stack abstract interface. */
    const T &Top() const{ return (*m_vector)[m_vector->Length() - 1]; }

    /** Satisfies the Stack abstract interface. */
    T &Top(){ return (*m_vector)[m_vector->Length() - 1]; }

    /** Satisfies the Stack abstract interface. */
    GUINT32 CountStackItems() const{ return m_vector->Length(); }

    /** Satisfies the Stack abstract interface. */
    void FlushStack(){ m_vector->Clear(); }


private:

    Vector<T> *m_vector;
    bool m_delete;

};


template<class T>class VectorQueue : public Queue<T>
{
    GUTIL_DISABLE_COPY(VectorQueue<T>)
public:

    /** Attaches the Queue interface to the vector.
        We will not delete your vector; that is up to you, because you may attach multiple
        interfaces to the same vector.
    */
    inline VectorQueue(Vector<T> *vec) :m_vector(vec), m_delete(false){}

    /** Creates a new Vector with a queue interface.
        Use this if you don't want to supply a pre-existing vector object.
    */
    inline VectorQueue() :m_vector(new Vector<T>), m_delete(true){}

    inline ~VectorQueue(){ if(m_delete) delete m_vector; }

    /** Satisfies the Queue abstract interface. */
    void Enqueue(const T &i){ m_vector->PushBack(i); }

    /** Satisfies the Queue abstract interface. */
    void Dequeue(){ m_vector->RemoveAt(0); }

    /** Satisfies the Queue abstract interface. */
    T &Front(){ return (*m_vector)[0]; }

    /** Satisfies the Queue abstract interface. */
    const T &Front() const{ return (*m_vector)[0]; }

    /** Satisfies the Queue abstract interface. */
    GUINT32 CountQueueItems() const{ return m_vector->Length(); }

    /** Satisfies the Queue abstract interface. */
    void FlushQueue(){ m_vector->Clear(); }


private:

    Vector<T> *m_vector;
    bool m_delete;

};


template<class T>class VectorDeque : public Deque<T>
{
    GUTIL_DISABLE_COPY(VectorDeque<T>)
public:

    /** Attaches the Deque interface to the vector.
        We will not delete your vector; that is up to you, because you may attach multiple
        interfaces to the same vector.
    */
    inline VectorDeque(Vector<T> *vec) :m_vector(vec), m_delete(false){}

    /** Creates a new Vector with a queue interface.
        Use this if you don't want to supply a pre-existing vector object.
    */
    inline VectorDeque() :m_vector(new Vector<T>), m_delete(true){}

    inline ~VectorDeque(){ if(m_delete) delete m_vector; }

    /** Satisfies the Deque abstract interface. */
    void PushBack(const T &i){ m_vector->PushBack(i); }

    /** Satisfies the Deque abstract interface. */
    void PopBack(){ m_vector->PopBack(); }

    /** Satisfies the Deque abstract interface. */
    void PushFront(const T &i){ m_vector->Insert(i, 0); }

    /** Satisfies the Deque abstract interface. */
    void PopFront(){ m_vector->RemoveAt(0); }

    /** Satisfies the Deque abstract interface. */
    T &Front(){ return (*m_vector)[0]; }

    /** Satisfies the Deque abstract interface. */
    const T &Front() const{ return (*m_vector)[0]; }

    /** Satisfies the Deque abstract interface. */
    T &Back(){ return (*m_vector)[m_vector->Length() - 1]; }

    /** Satisfies the Deque abstract interface. */
    const T &Back() const{ return (*m_vector)[m_vector->Length() - 1]; }

    /** Satisfies the Deque abstract interface. */
    GUINT32 CountDequeItems() const{ return m_vector->Length(); }

    /** Satisfies the Deque abstract interface. */
    void FlushDeque(){ m_vector->Clear(); }


private:

    Vector<T> *m_vector;
    bool m_delete;

};


template<class T>class VectorRandomAccessContainer : public RandomAccessContainer<T>
{
    GUTIL_DISABLE_COPY(VectorRandomAccessContainer<T>)
public:

    inline VectorRandomAccessContainer(Vector<T> *vec) :m_vector(vec), m_delete(false){}

    inline VectorRandomAccessContainer() :m_vector(new Vector<T>), m_delete(true){}

    inline ~VectorRandomAccessContainer(){ if(m_delete) delete m_vector; }

    /** Satisfies the RandomAccessContainer abstract interface. */
    T &At(GUINT32 i){ return (*m_vector)[i]; }

    /** Satisfies the RandomAccessContainer abstract interface. */
    const T &At(GUINT32 i) const{ return (*m_vector)[i]; }

    /** Satisfies the RandomAccessContainer abstract interface. */
    void InsertAt(const T &i, GUINT32 indx){ m_vector->Insert(i, indx); }

    /** Satisfies the RandomAccessContainer abstract interface. */
    void RemoveAt(GUINT32 indx){ m_vector->RemoveAt(indx); }

    /** Satisfies the RandomAccessContainer abstract interface. */
    GUINT32 CountContainerItems() const{ return m_vector->Length(); }

    /** Satisfies the RandomAccessContainer abstract interface. */
    void FlushContainer(){ m_vector->Clear(); }


private:

    Vector<T> *m_vector;
    bool m_delete;

};


END_NAMESPACE_GUTIL1;


namespace GUtil
{

template<class T>struct IsMovableType< DataObjects::Vector<T> >{ enum{ Value = 1 }; };
template<class T>struct IsMovableType< DataObjects::VectorStack<T> >{ enum{ Value = 1 }; };
template<class T>struct IsMovableType< DataObjects::VectorQueue<T> >{ enum{ Value = 1 }; };
template<class T>struct IsMovableType< DataObjects::VectorDeque<T> >{ enum{ Value = 1 }; };
template<class T>struct IsMovableType< DataObjects::VectorRandomAccessContainer<T> >{ enum{ Value = 1 }; };

}

#endif // GUTIL_VECTOR_H
