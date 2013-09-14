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

#ifndef GUTIL_VECTOR_H
#define GUTIL_VECTOR_H

#include "gutil_globals.h"
#include "gutil_flexibletypecomparer.h"
#include "gutil_Icollections.h"
#include <new>
#include <limits.h>
#include <malloc.h>
#include <cstring>
NAMESPACE_GUTIL1(DataObjects);


/** This is the implementation for the Vector class.

    Manages a simple vector of memory, which grows exponentially with powers of two.
    You call reserve() with the number of items you want to have, and it will reserve
    at least as many, only conducting memory reallocations when necessary.

    The VectorImp stores its total capacity, and also a pointer to the end of the array,
    in the memory locations directly preceding the base of the array.  You interact
    with these values via the Capacity(), Length() and DataEnd() functions.  If you
    have a pointer to an array of data managed by a VectorImp, then you can use the
    static versions of these functions to access this meta-data.

    \note The size of a VectorImp on the stack is only 4 bytes, the same as an int or
    a pointer.  This was a design choice, to allocate the necessary meta-data
    at runtime only when you actually start putting things in the vector.
*/
template<class T>class VectorImp
{
    /** The layout of the data pointer is as follows:
     *  {
     *                  GUINT32 capacity;
     *                  GUINT32 size;
     *      m_data ->   T array[size];
     *  }
     *
     *  So the pointer is actually pointing to the start of the array, and the size/capacity memory
     *  is just before the pointer.  This is an optimization, based on the assumption that the most-often
     *  referenced data will be the array pointer.
     *
     *  The size of the memory on the heap is  (2 * sizeof(GUINT32)) + (capacity * sizeof(T))
    */
    void *m_data;

public:
    class iterator;
    class const_iterator;

    /** Constructs an empty vector. */
    inline VectorImp() :m_data(NULL) {}

    /** Constructs an empty vector capable of holding the given number of items. */
    inline explicit VectorImp(GUINT32 capacity)
        :m_data(NULL)
    {
        if(0 < capacity)
            Reserve(capacity);
    }


    /**  Constructs a vector of the given size, where all elements are copies of the provided object. */
    inline explicit VectorImp(const T &o, GUINT32 size = 1)
        :m_data(NULL)
    {
        Reserve(size);
        set_length(size);
        T *cur(Data());
        for(GUINT32 i(0); i < size; ++i, ++cur)
            new(cur) T(o);
    }


    /**  Constructs a vector from the array of data. */
    inline VectorImp(T const*arr, GUINT32 size)
        :m_data(NULL)
    {
        if(size > 0)
        {
            Reserve(size);
            set_length(size);
            T *cur(Data());
            for(GUINT32 i(0); i < size; ++i)
                new(cur++) T(*(arr++));
        }
    }


    /** Creates a vector with the elements from another vector, taken by iterating from
        iter_begin to iter_end.
    */
    inline VectorImp(const typename VectorImp<T>::const_iterator &iter_begin,
                        const typename VectorImp<T>::const_iterator &iter_end)
        :m_data(NULL)
    {
        const GUINT32 sz( iter_end - iter_begin );
        if(sz > 0)
            Reserve(sz);
        T *ptr(Data());
        for(VectorImp<T>::const_iterator cur(iter_begin); cur != iter_end;)
            new(ptr++) T(*(cur++));
    }


    /** The copy constructor conducts a deep copy, invoking the copy constructors on each item. */
    VectorImp(const VectorImp<T> &o)
        :m_data(NULL)
    {
        if(0 < o.Capacity())
        {
            Reserve(o.Capacity());
            set_length(o.Length());

            // Call the copy constructor for each item to initialize the memory
            T *cur( Data() );
            T const *ocur( o.ConstData() );
            for(GUINT32 i(0); i < Length(); ++i)
                new(cur++) T(*(ocur++));
        }
    }

    ~VectorImp(){ Clear(); }

    /** Assignment operator invokes our copy constructor after clearing the container. */
    VectorImp &operator = (const VectorImp<T> &o){
        Empty();
        Insert(o, 0);
        return *this;
    }

    /** Returns whether the vector is null (has not yet been initialized). */
    inline bool IsNull() const{ return !(m_data && ConstData()); }

    /** Returns whether the vector is empty (has no items in it, but the capacity may have been initialized). */
    inline bool IsEmpty() const{ return IsNull() || Length() == 0; }

    /** Inserts the item at the end of the vector. */
    inline T &Insert(const T &item){ return Insert(item, Length()); }

    /** Insert the item at the position before the iterator.
        \note Invalidates all iterators, because the addition may cause a resize of the internal
        memory, which potentially moves the array.
    */
    inline T &Insert(const T &item, const iterator &iter){ return Insert(item, iter.Index()); }

    /** Insert the item at the index position.  It will throw an exception if the index is out of bounds.
        \note Invalidates all iterators, because the addition may cause a resize of the internal
        memory, which potentially moves the array.
    */
    T &Insert(const T &item, GUINT32 indx)
    {
        const GUINT32 len( Length() );

        if(len < indx)
            THROW_NEW_GUTIL_EXCEPTION(IndexOutOfRangeException);

        // Allocate more memory if we have to
        if(len == Capacity())
            Reserve(len + 1);

        T *dest( Data() + indx );

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
                T *cur(Data() + len - 1);

                // Call the constructor on the memory location at the end
                new(Data() + len) T(*cur);

                if(len > 0)
                {
                    for(GUINT32 i(len - 1); i > indx; --i, --cur)
                        *cur = *(cur - 1);
                }

                // Then assign the item to the proper location
                Data()[indx] = item;
            }
        }
        else
        {
            // Call the copy constructor to initialize the memory at the end of the array
            new(dest) T(item);
        }

        set_length( len + 1 );

        return this->operator [](indx);
    }

    /** Insert the vector at the index position.
        \note Invalidates all iterators, because the addition may cause a resize of the internal
        memory, which potentially moves the array.
    */
    inline void Insert(const VectorImp<T> &vec, GUINT32 indx){
        Insert(vec.ConstData(), vec.Length(), indx);
    }

    /** Insert the array at the index position.  It will throw an exception if the index is out of bounds.
        \note Invalidates all iterators, because the addition may cause a resize of the internal
        memory, which potentially moves the array.
    */
    void Insert(T const *vec, GUINT32 size, GUINT32 indx)
    {
        const GUINT32 len( Length() );

        if(len < indx)
            THROW_NEW_GUTIL_EXCEPTION(IndexOutOfRangeException);

        // Allocate more memory if we have to
        if((len + size) > Capacity())
            Reserve(Length() + size);

        T *dest( Data() + indx );

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
                    new(Data() + len + size - i) T(*(Data() + len - i));

                T *cur(Data() + len - 1);
                if(len > 0)
                {
                    for(GUINT32 i(len - 1); i > (indx + size); --i, --cur)
                        *cur = *(cur - size);
                }

                // Then assign the items to the proper location
                for(GUINT32 i(0); i < size; ++i)
                {
                    if(i < len)
                        Data()[indx + i] = vec[i];
                    else
                        new(Data() + indx + i) T(vec[i]);
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

    /** Adds the item to the end of the vector. */
    inline void PushBack(const T &item){ Insert(item, Length()); }

    /** Adds the item to the start of the vector. */
    inline void PushFront(const T &item){ Insert(item, 0); }

    /** Removes the item at the back of the vector. */
    inline void PopBack(){ RemoveAt(Length() - 1); }

    /** Removes the item at the front of the vector. */
    inline void PopFront(){ RemoveAt(0); }

    /** Returns the item at the back of the vector. */
    inline const T &Back() const{ return ConstData()[Length() - 1]; }

    /** Returns the item at the back of the vector. */
    inline T &Back(){ return Data()[Length() - 1]; }

    /** Returns the item at the front of the vector. */
    inline const T &Front() const{ return *ConstData(); }

    /** Returns the item at the front of the vector. */
    inline T &Front(){ *Data(); }

    /** Convenient operator that appends to the end of the vector, and returns a reference to this. */
    inline VectorImp<T> &operator << (const T &item){ PushBack(item); return *this; }

    /** Convenient operator that removes the last item in the vector and copies it to the given reference. */
    inline void operator >> (T &cpy){ cpy = Back(); PopBack(); }

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

        T *const targ( Data() + indx );

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
                (Data() + i)->~T();
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

    /** Reserves room for at least this many items.
        Pass 0 to free all memory.
    */
    inline void Reserve(GUINT32 n){ ReserveExactly( _compute_capacity(n) ); }

    /** Reserves room for exactly this number of items. */
    void ReserveExactly(GUINT32 new_capacity)
    {
        if(new_capacity == Capacity())
            return;

        if(new_capacity < Capacity())
        {
            // Need to call the destructors on the items we're (potentially) deleting
            if(new_capacity < Length())
            {
                for(T *targ( Data() + new_capacity ); targ != DataEnd(); ++targ)
                    targ->~T();
                set_length( new_capacity );
            }
        }

        const GUINT32 new_size_in_bytes(sizeof(GUINT32) + sizeof(GUINT32) + new_capacity * sizeof(T));
        const GUINT32 length_bak = Length();

        if(0 < new_capacity)
        {
            if(IsMovableType<T>::Value)
            {
                // As an optimization for primitive types (ones that are not affected by binary moves)
                //  we call realloc, because a hidden memory relocation doesn't affect our type.
                void *tmp = realloc(m_data ? reinterpret_cast<GBYTE *>(m_data) - 2 * sizeof(GUINT32) : NULL, new_size_in_bytes);

                if(NULL == tmp)
                    THROW_NEW_GUTIL_EXCEPTION(BadAllocationException);

                m_data = reinterpret_cast<GBYTE *>(tmp) + 2 * sizeof(GUINT32);
            }
            else
            {
                // Have to manually reallocate and call the copy constructors, because a complex
                //  type may be dependent on their memory locations (self-pointers are one example)
                T *backup( Data() );
                void *new_data( malloc(new_size_in_bytes) );

                if(NULL == new_data)
                    THROW_NEW_GUTIL_EXCEPTION(BadAllocationException);

                new_data = reinterpret_cast<GBYTE *>(new_data) + 2 * sizeof(GUINT32);
                T *cur( reinterpret_cast<T *>(new_data) );
                if(backup)
                {
                    // Copy the items from the backup copy to the new vector
                    for(GUINT32 i(0); i < Length(); ++i)
                        new(cur++) T(*(backup + i));

                    // Have to destruct the items from the backup vector
                    for(GUINT32 i(0); i < Length(); ++i)
                        (backup++)->~T();

                    free(reinterpret_cast<GBYTE *>(m_data) - 2 * sizeof(GUINT32));
                }

                m_data = new_data;
            }

            // Initialize the meta-data at the newly allocated pointer
            *_length() = length_bak;
            *_capacity() = new_capacity;
        }
        else
        {
            // Free the pointer because 0 capacity was requested
            if(m_data)
            {
                free(reinterpret_cast<GBYTE *>(m_data) - 2 * sizeof(GUINT32));
                m_data = NULL;
            }
        }
    }

    /** Resizes the vector.  If the new size is larger than the current, then the default
        object will be copied to give a value to the newly created objects.
    */
    void Resize(GUINT32 new_size, const T &default_object = T()){
        if(new_size > Capacity())
            Reserve(new_size);
        if(new_size < Length())
        {
            const VectorImp<T>::iterator iter( begin() + new_size );
            for(GUINT32 i(Length()); i > new_size; --i)
                Remove(iter);
        }
        else if(new_size > Length())
        {
            T *cur(Data() + Length());
            for(GUINT32 i(Length()); i < new_size; ++i)
                new(cur++) T(default_object);
        }
        set_length(new_size);
    }

    /** Accesses the data at the given index.
        \note Does not do any bounds checking.
    */
    inline T const &operator [](GUINT32 i) const{ return ConstData()[i]; }
    /** Accesses the data at the given index.
        \note Does not do any bounds checking.
    */
    inline T const &operator [](GINT32 i) const{ return ConstData()[i]; }

    /** Accesses the data at the given index.
        \note Does not do any bounds checking.
    */
    inline T &operator [](GUINT32 i){ return Data()[i]; }
    /** Accesses the data at the given index.
        \note Does not do any bounds checking.
    */
    inline T &operator [](GINT32 i){ return Data()[i]; }

    /** Accesses the data at the given index.
        \note Checks the index and throws an exception if it is out of bounds
    */
    inline T &At(GUINT32 i){
        return const_cast<T &>(const_cast<VectorImp<T> const *>(this)->At(i));
    }

    /** Accesses the data at the given index.
        \note Checks the index and throws an exception if it is out of bounds
    */
    inline T const &At(GUINT32 i) const{
        if(i >= Length())
            THROW_NEW_GUTIL_EXCEPTION(IndexOutOfRangeException);
        return ConstData()[i];
    }


    /** Returns a pointer to the start of the array. */
    inline T *Data(){ return const_cast<T *>(ConstData()); }

    /** Returns a pointer to the start of the array. */
    inline T const *ConstData() const{ return reinterpret_cast<T const *>(m_data); }

    /** Returns a pointer to the end of the vector, which is 1 past
        the last item in the vector.  Do not dereference this pointer, it
        is only to allow quick iteration through the list.
    */
    inline T *DataEnd(){ return const_cast<T *>(ConstDataEnd()); }

    /** Returns a pointer to the end of the vector, which is 1 past
        the last item in the vector.  Do not dereference this pointer, it
        is only to allow quick iteration through the list.
    */
    inline T const *ConstDataEnd() const{
        return ConstData() ? ConstData() + *_length() : NULL;
    }

    /** Clears all items and frees all memory. */
    inline void Clear(){ ReserveExactly(0); }

    /** Clears all items but retains the same capacity. */
    inline void Empty(){
        if(m_data)
        {
            T *cur( Data() );
            T *e( DataEnd() );
            while(cur != e){
                cur->~T();
                ++cur;
            }
            set_length(0);
        }
    }

    /** The current length of the vector. */
    inline GUINT32 Length() const{
        return m_data ? *_length() : 0;
    }

    /** How many items of type T we are capable of holding. */
    inline GUINT32 Capacity() const{ return m_data ? *_capacity() : 0; }

    /** Conducts a linear search for the first instance of the item
        starting at the given index, and using the == operator to test equality.
        \return The index on success, UINT_MAX on failure
    */
    inline GUINT32 IndexOf(const T &item, GUINT32 start = 0) const{
        GUINT32 ret( UINT_MAX );
        T const *cur( ConstData() + start );
        T const *e( ConstDataEnd() );
        while(cur < e)
        {
            if(*cur == item)
            {
                ret = cur - ConstData();
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
            T const *cur( ConstData() + start );
            T const *e( ConstData() - 1 );
            while(cur < e)
            {
                if(*cur == item)
                {
                    ret = cur - ConstData();
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
            VectorImp<T> buffer;
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
        inline iterator(T *begin, T *end, T *cur)
            :current(cur),
              m_begin(begin),
              m_end(end)
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
        T const *current;
        T const *m_begin;
        T const *m_end;
    public:
        inline const_iterator()
            :current(0),
              m_begin(0),
              m_end(0)
        {}
        inline const_iterator(T const *begin, T const *end, T const *cur)
            :current(cur),
              m_begin(begin),
              m_end(end)
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

        inline T const &operator *() const{ return *current; }
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

    inline iterator begin(){ return iterator(Data(), DataEnd(), Data()); }
    inline const_iterator begin() const{ return const_iterator(ConstData(), ConstDataEnd(), ConstData()); }
    inline iterator end(){ return iterator(Data(), DataEnd(), DataEnd()); }
    inline const_iterator end() const{ return const_iterator(ConstData(), ConstDataEnd(), ConstDataEnd()); }

    inline iterator rbegin(){ return iterator(Data(), DataEnd(), DataEnd() - 1); }
    inline const_iterator rbegin() const{ return const_iterator(ConstData(), ConstDataEnd(), ConstDataEnd() - 1); }
    inline iterator rend(){ return iterator(Data(), DataEnd(), Data() - 1); }
    inline const_iterator rend() const{ return const_iterator(ConstData(), ConstDataEnd(), ConstData() - 1); }

    /** Returns true if the vectors are equal.
        VectorImp equality is established by checking each individual element from
        this vector with the corresponding element in the other vector and evaluating
        the class' own == operator.
        \note O(N)
    */
    bool operator == (const VectorImp<T> &o) const{
        bool res(false);
        T const *cur1(ConstData()), *cur2(o.ConstData());
        T const *e = ConstDataEnd();
        if(Length() == o.Length() && static_cast<bool>(cur1) == static_cast<bool>(cur2))
        {
            res = true;
            while(cur1 != e)
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
        VectorImp equality is established by checking each individual element from
        this vector with the corresponding element in the other vector and evaluating
        the class' own == operator.
        \note O(N)
    */
    inline bool operator != (const VectorImp<T> &o) const{ return !operator == (o); }

    /** Does a constant-time data swap between vectors.

        After calling this, lhs will own rhs' data, and vice versa.  It is
        simply a pointer swap.

        \note After calling, all iterators for both vectors will still be valid,
        they will only reference the opposite VectorImp's data.
    */
    inline static void Swap(VectorImp<T> &lhs, VectorImp<T> &rhs){
        void *tmp( lhs.m_data );
        lhs.m_data = rhs.m_data;
        rhs.m_data = tmp;
    }


protected:

    /** Subclasses can use this convenience function as a setter for the length variable.
        \note You should only manually use this with POD types, because if you
        have a vector of more complex classes then some of their destructors may not get called.
        \note You must have already allocated the data pointer, otherwise this will dereference a null pointer.
    */
    inline void set_length(GUINT32 len){ *_length() = len; }


private:

    inline static int _compute_capacity(int n){ return n <= 0 ? 0 : GEN_BITMASK_32( FSB32( n ) ); }

    // Be careful when calling these; they do not check if the pointer is valid
    inline GUINT32 *_length() const{ return reinterpret_cast<GUINT32 *>(m_data) - 1; }
    inline GUINT32 *_capacity() const{ return reinterpret_cast<GUINT32 *>(m_data) - 2; }

    void _merge_sort(const iterator &b, const iterator &e, bool ascending, VectorImp<T> &buffer, const Interfaces::IComparer<T> &cmp){
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
                    buffer.Insert(*i2);
                    ++i2;
                }
                else
                {
                    buffer.Insert(*i1);
                    ++i1;
                }
            }
            while(i1 != m)
            {
                buffer.Insert(*i1);
                ++i1;
            }
            while(i2 != e)
            {
                buffer.Insert(*i2);
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



/** This is used to define all the same constructors (and virtual destructor) as the base class so you don't have to
 *  repeat so much code.
 *
 *  \param class_name The name of top-level class for which to define constructors.
 *  \param base_class The base class to whose constructors you will forward parameters.
*/
#define GUTIL_VECTOR_CONSTRUCTORS(class_name, base_class) \
    public: \
    inline class_name() {} \
    inline explicit class_name(GUINT32 capacity) :base_class(capacity){} \
    inline explicit class_name(const T &o, GUINT32 size = 1) :base_class(o, size){} \
    inline class_name(T const*arr, GUINT32 size) :base_class(arr, size){} \
    inline class_name(const typename VectorImp<T>::const_iterator &iter_begin, const typename VectorImp<T>::const_iterator &iter_end) :base_class(iter_begin, iter_end){} \
    inline class_name(const VectorImp<T> &o) :base_class(o) {} \
    virtual ~class_name(){}




/** Defines the basic vector type without any interfaces.
 *
 *  Use this as the most memory efficient version.
 *
 *  \tparam T The type contained by the vector.
 *  \tparam IFace The interface class, or void if no interface.
*/
template<class T, typename IFace = void> class Vector : public VectorImp<T>
{ GUTIL_VECTOR_CONSTRUCTORS(Vector, VectorImp<T>) };


/** Defines a template specialization of the Vector class, with a random access container interface. */
template<class T> class Vector<T, IRandomAccessContainer<T> > :
        public Vector<T>,
        public IRandomAccessContainer<T>
{
    GUTIL_VECTOR_CONSTRUCTORS(Vector, Vector<T>)

    virtual T &At(GUINT32 i){ return VectorImp<T>::At(i); }
    virtual T const &At(GUINT32 i) const{ return VectorImp<T>::At(i); }

    virtual T &Insert(const T &item, GUINT32 i){ return VectorImp<T>::Insert(item, i); }
    virtual void Remove(GUINT32 i){ VectorImp<T>::RemoveAt(i); }

    virtual GUINT32 Size() const{ return VectorImp<T>::Length(); }
    virtual void Clear(){ VectorImp<T>::Empty(); }

};



/** Defines a template specialization of the Vector class, with a stack interface. */
template<class T> class Vector<T, IStack<T> > :
        public Vector<T>,
        public IStack<T>
{
    GUTIL_VECTOR_CONSTRUCTORS(Vector, Vector<T>)

    virtual T &Push(const T &item){ return VectorImp<T>::Insert(item); }
    virtual void Pop(){ VectorImp<T>::RemoveAt(VectorImp<T>::Length() - 1); }
    virtual T &Top(){ return VectorImp<T>::Data()[VectorImp<T>::Length() - 1]; }
    virtual T const &Top() const{ return VectorImp<T>::ConstData()[VectorImp<T>::Length() - 1]; }

    virtual GUINT32 Size() const{ return VectorImp<T>::Length(); }
    virtual void Clear(){ VectorImp<T>::Empty(); }

};


/** Defines a template specialization of the Vector class, with a queue interface. */
template<class T> class Vector<T, IQueue<T> > :
        public Vector<T>,
        public IQueue<T>
{
    GUTIL_VECTOR_CONSTRUCTORS(Vector, Vector<T>)

    virtual T &Enqueue(const T &item){ return VectorImp<T>::PushBack(item); }
    virtual void Dequeue(){ VectorImp<T>::PopFront(); }
    virtual T &Front(){ return VectorImp<T>::Front(); }
    virtual T const &Front() const{ return VectorImp<T>::Front(); }

    virtual GUINT32 Size() const{ return VectorImp<T>::Length(); }
    virtual void Clear(){ VectorImp<T>::Empty(); }

};


/** Defines a template specialization of the Vector class, with a deque interface. */
template<class T> class Vector<T, IDeque<T> > :
        public Vector<T>,
        public IDeque<T>
{
    GUTIL_VECTOR_CONSTRUCTORS(Vector, Vector<T>)

    virtual T &EnqueueFront(const T &item){ return VectorImp<T>::PushFront(item); }
    virtual T &EnqueueBack(const T &item){ return VectorImp<T>::PushBack(item); }
    virtual void DequeueFront(){ VectorImp<T>::PopFront(); }
    virtual void DequeueBack(){ VectorImp<T>::PopBack(); }
    virtual T &Front(){ return VectorImp<T>::Front(); }
    virtual T const &Front() const{ return VectorImp<T>::Front(); }
    virtual T &Back(){ return VectorImp<T>::Back(); }
    virtual T const &Back() const{ return VectorImp<T>::Back(); }

    virtual GUINT32 Size() const{ return VectorImp<T>::Length(); }
    virtual void Clear(){ VectorImp<T>::Empty(); }

};


END_NAMESPACE_GUTIL1;


namespace GUtil
{

template<class T>struct IsMovableType< DataObjects::VectorImp<T> >{ enum{ Value = 1 }; };
template<class T, typename U>struct IsMovableType< DataObjects::Vector<T, U> >{ enum{ Value = 1 }; };

}

#endif // GUTIL_VECTOR_H
