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

#include "gutil.h"
#include "Core/exception.h"
#include "Core/DataObjects/interfaces.h"
#include "Core/Interfaces/imemoryusagereporter.h"
#include <cstring>
#include <malloc.h>
GUTIL_BEGIN_CORE_NAMESPACE(DataObjects);


/** Manages a simple vector of memory, which grows exponentially with powers of two.
    You call reserve() with the number of items you want to have, and it will reserve
    at least as many, only conducting memory reallocations when necessary.

    It is a simplified interface, to minimize the size of the class for better memory performance.
    If you want more features and a better interface, and you don't care about a few extra bytes,
    then use the Vector, which implements convenient accessor functions.

    \sa Vector
*/
template<class T>class SimpleVector
{
public:
    class iterator;
    class const_iterator;

    /** Constructs an empty vector. */
    inline SimpleVector()
        :m_begin(NULL),
          m_capacity(0),
          m_length(0)
    {}


    /** Constructs an empty vector capable of holding the given number of items. */
    inline explicit SimpleVector(int capacity)
        :m_begin(NULL),
          m_capacity(0),
          m_length(0)
    {
        if(capacity > 0)
            Reserve(m_capacity);
    }


    /**  Constructs a vector of the given size, where all elements are copies of the provided object. */
    inline SimpleVector(const T &o, int size)
        :m_begin(NULL),
          m_capacity(0),
          m_length(size)
    {
        if(size > 0)
        {
            Reserve(size);
            T *cur(m_begin);
            for(int i(0); i < m_length; ++i)
                _copy_construct(cur++, o);
        }
    }


    /**  Constructs a vector from the array of data. */
    inline SimpleVector(T const*arr, int size)
        :m_begin(NULL),
          m_capacity(0),
          m_length(size)
    {
        if(size > 0)
        {
            Reserve(size);
            T *cur(m_begin);
            for(int i(0); i < m_length; ++i)
                _copy_construct(cur++, *(arr++));
        }
    }


    /** Creates a vector with the elements from another vector, taken by iterating from
        iter_begin to iter_end.
    */
    inline SimpleVector(const typename SimpleVector<T>::const_iterator &iter_begin,
                        const typename SimpleVector<T>::const_iterator &iter_end)
        :m_begin(NULL),
          m_capacity(0),
          m_length(iter_end - iter_begin)
    {
        if(m_length > 0)
            Reserve(m_length);
        GUINT32 cnt(0);
        for(SimpleVector<T>::const_iterator cur(iter_begin); cur != iter_end; ++cur, ++cnt)
            _copy_construct(m_begin + cnt, *cur);
    }


    /** The copy constructor conducts a deep copy, invoking the copy constructors on each item. */
    SimpleVector(const SimpleVector<T> &o)
        :m_begin(NULL),
          m_capacity(0),
          m_length(o.m_length)
    {
        if(o.m_capacity > 0)
            Reserve(o.m_capacity);

        // Call the copy constructor for each item to initialize the memory
        T *cur( m_begin );
        T *ocur( o.m_begin );
        for(int i(0); i < m_length; ++i)
            _copy_construct(cur++, *(ocur++));
    }
    /** Assignment operator invokes our copy constructor after clearing the container. */
    SimpleVector &operator = (const SimpleVector<T> &o){
        Clear();
        _copy_construct(this, o);
        return *this;
    }
    inline ~SimpleVector(){ Clear(); }

    /** Insert the item at the position before the iterator.
        \note Invalidates all iterators, because the addition may cause a resize of the internal
        memory, which potentially moves the array.
    */
    void Insert(const T &item, const iterator &iter)
    {
        // Allocate more memory if we have to
        if(m_length == m_capacity)
            Reserve(m_length + 1);

        T *dest( m_begin + iter.m_cur );

        // Move the destination out of the way, if they're inserting anywhere but the end
        if(iter.m_cur < m_length)
            memmove(dest + 1, dest, (m_length - iter.m_cur) * sizeof(T));

        // Call the copy constructor to initialize the memory
        _copy_construct(dest, item);

        ++m_length;
    }

    /** Remove the item pointed to by the iterator.
        No internal memory is freed.  You can free it manually by using Reserve(), but as a heuristic
        it is a good estimate of how much memory we need if we remain at our max consumption.
        \note Invalidates the iterator positioned at the last element, and all other iterators
        after the input would notice their current element has shifted
    */
    void Remove(const iterator &iter)
    {
        T *const targ( m_begin + iter.m_cur );

        // Call the destructor on the item
        targ->~T();

        // Copy all the following items over 1
        if(iter.m_cur < (m_length - 1))
            memmove(targ, targ + 1, (m_length - iter.m_cur - 1) * sizeof(T));

        --m_length;
    }

    /** Reserves room for at least this many items.
        Pass 0 to clear memory.
    */
    void Reserve(int n)
    {
        GUINT32 new_capacity( _capacity(n) );
        if(new_capacity == m_capacity)
            return;
        else if(new_capacity < m_capacity)
        {
            // Need to call the destructors on the items we're (potentially) deleting
            if(new_capacity < Length())
            {
                for(GUINT32 i(new_capacity); i < m_length; ++i)
                    m_begin[i].~T();
                m_length = new_capacity;
            }
        }

        m_capacity = new_capacity;

        // We call realloc, so that no constructors are called when the memory is allocated.
        //  We will initialize the memory manually when we're ready to.  The memory we get
        //  from it is indeterminate.
        m_begin = reinterpret_cast<T *>( realloc(m_begin, m_capacity * sizeof(T)) );
    }

    /** Resizes the vector.  If the new size is larger than the current, then the default
        object will be copied to give a value to the newly created objects.
    */
    void Resize(GUINT32 new_size, const T &default_object = T()){
        if(new_size > m_capacity)
            Reserve(new_size);
        if(new_size < m_length)
        {
            const SimpleVector<T>::iterator iter( begin() + new_size );
            for(int i(m_length); i > new_size; --i)
                Remove(iter);
        }
        else if(new_size > m_length)
        {
            T *cur(m_begin + m_length);
            for(int i(m_length); i < new_size; ++i)
                _copy_construct(cur++, default_object);
        }
    }

    /** Accesses the data at the given index.
        \note Does not do any bounds checking.
    */
    inline const T &operator [](int i) const{ return m_begin[i]; }
    /** Accesses the data at the given index.
        \note Does not do any bounds checking.
    */
    inline T &operator [](int i){ return m_begin[i]; }

    /** Clears all items from the stack and frees all memory. */
    inline void Clear(){ Reserve(0); }

    /** The current length of the vector. */
    inline GUINT32 Length() const{ return m_length; }
    /** The current length of the vector. */
    inline GUINT32 Size() const{ return Length(); }

    /** How many items of type T we are capable of holding. */
    inline GUINT32 Capacity() const{ return m_capacity; }

    /** Iterates through the vector. */
    class iterator :
            public std::iterator<std::random_access_iterator_tag, T>
    {
        friend class SimpleVector;
    public:
        inline iterator()
            :m_cur(0),
              m_begin(0),
              m_length(0){}
        inline iterator(T *begin, int length, int index)
            :m_cur(index),
              m_begin(begin),
              m_length(length){}

        inline T &operator *(){ return m_begin[m_cur]; }
        inline const T &operator *() const{ return m_begin[m_cur]; }
        inline T *operator ->(){ return &m_begin[m_cur]; }
        inline T const*operator ->() const{ return &m_begin[m_cur]; }

        inline iterator &operator ++(){ ++m_cur; return *this; }
        inline iterator operator ++(int){ iterator ret(*this); ++m_cur; return ret;}
        inline iterator &operator +=(int n){ m_cur += n; return *this; }
        inline iterator operator +(int n) const{ iterator ret(*this); ret.m_cur += n; return ret; }

        inline iterator &operator --(){ --m_cur; return *this; }
        inline iterator operator --(int){ iterator ret(*this); --m_cur; return ret;}
        inline iterator &operator -=(int n){ m_cur -= n; return *this; }
        inline iterator operator -(int n) const{ iterator ret(*this); ret.m_cur -= n; return ret;}

        /** Returns the distance between iterators (how many items in between). */
        inline int operator - (const iterator &iter) const{
            if(m_begin != iter.m_begin) return INT_MAX;
            return m_cur - iter.m_cur;
        }

        inline operator bool() const{ return m_begin && (m_cur >= 0 && m_cur < m_length); }


    protected:
        GUINT32 m_cur;
    private:
        T *m_begin;
        GUINT32 m_length;
    };

    /** Iterates through the vector, but also guarantees that it won't modify the vector. */
    class const_iterator :
            public std::iterator<std::random_access_iterator_tag, T>
    {
        friend class SimpleVector;
    public:
        inline const_iterator()
            :m_begin(0),
              m_cur(0),
              m_length(0){}
        inline const_iterator(T *begin, int length, int index)
            :m_begin(begin),
              m_cur(index),
              m_length(length){}
        inline const_iterator(const const_iterator &iter)
            :m_begin(iter.m_begin),
              m_cur(iter.m_cur),
              m_length(iter.m_length)
        {}
        inline const_iterator(const iterator &iter)
            :m_begin(iter.m_begin),
              m_cur(iter.m_cur),
              m_length(iter.m_length)
        {}

        inline T &operator *(){ return m_begin[m_cur]; }
        inline const T &operator *() const{ return m_begin[m_cur]; }
        inline T *operator ->(){ return &m_begin[m_cur]; }
        inline T const*operator ->() const{ return &m_begin[m_cur]; }

        inline const_iterator &operator ++(){ ++m_cur; return *this; }
        inline const_iterator operator ++(int){ const_iterator ret(*this); ++m_cur; return ret;}
        inline const_iterator &operator +=(int n){ m_cur += n; return *this; }
        inline const_iterator operator +(int n) const{ const_iterator ret(*this); ret.m_cur += n; return ret; }

        inline const_iterator &operator --(){ --m_cur; return *this; }
        inline const_iterator operator --(int){ const_iterator ret(*this); --m_cur; return ret;}
        inline const_iterator &operator -=(int n){ m_cur -= n; return *this; }
        inline const_iterator operator -(int n) const{ const_iterator ret(*this); ret.m_cur -= n; return ret;}

        /** Returns the distance between iterators (how many items in between). */
        inline int operator - (const const_iterator &iter) const{
            if(m_begin != iter.m_begin) return INT_MAX;
            return m_cur - iter.m_cur;
        }

        inline operator bool() const{ return m_begin && (m_cur >= 0 && m_cur < m_length); }

    protected:
        int m_cur;
    private:
        T *m_begin;
        int m_length;
    };

    inline iterator begin(){ return iterator(m_begin, Length(), 0); }
    inline const_iterator begin() const{ return const_iterator(m_begin, Length(), 0); }
    inline iterator end(){ return iterator(m_begin, Length(), Length()); }
    inline const_iterator end() const{ return const_iterator(m_begin, Length(), Length()); }

    inline iterator rbegin(){ return iterator(m_begin, Length(), Length() - 1); }
    inline const_iterator rbegin() const{ return const_iterator(m_begin, Length(), Length() - 1); }
    inline iterator rend(){ return iterator(m_begin, Length(), -1); }
    inline const_iterator rend() const{ return const_iterator(m_begin, Length(), -1); }


private:

    T *m_begin;

    GUINT32 m_capacity;
    GUINT32 m_length;

    inline static int _capacity(int n){ return n <= 0 ? 0 : GEN_BITMASK_32( MSB32( n ) ); }

    template<class K>
    inline static void _copy_construct(K *targ, const K &cpy){ new(targ) K(cpy); }

};


/** A SimpleVector with extra interfaces attached.
    So it behaves like a Stack, Deque, RandomAccessContainer, etc... at the cost of slightly more
    memory usage to represent the various vtables, but in general you can ignore this.
*/
template<class T>class Vector :
        public SimpleVector<T>,
        public Stack<T>,
        public Deque<T>,
        public RandomAccessContainer<T>
{
public:

    inline Vector(){}
    inline Vector(int capacity) :SimpleVector<T>(capacity){}
    inline Vector(const T &o, int size) :SimpleVector<T>(o, size){}
    inline Vector(T const*arr, int size) :SimpleVector<T>(arr, size){}
    inline Vector(const typename SimpleVector<T>::const_iterator &iter_begin,
                  const typename SimpleVector<T>::const_iterator &iter_end)
        :SimpleVector<T>(iter_begin, iter_end){}


    /** Satisfies the Stack abstract interface. */
    void Push(const T &i){ this->Insert(i, Vector<T>::end()); }
    /** Satisfies the Stack abstract interface. */
    inline void Pop(){ this->Remove( Vector<T>::rbegin() ); }
    /** Satisfies the Stack abstract interface. */
    const T &Top() const{ return (*this)[Vector<T>::Length() - 1]; }
    /** Satisfies the Stack abstract interface. */
    T &Top(){ return (*this)[Vector<T>::Length() - 1]; }
    /** Satisfies the Stack abstract interface. */
    GUINT32 CountStackItems() const{ return Vector<T>::Length(); }
    /** Satisfies the Stack abstract interface. */
    void FlushStack(){ Vector<T>::Clear(); }

    /** Satisfies the Deque abstract interface. */
    void PushBack(const T &i){ this->Insert(i, Vector<T>::end()); }
    /** Satisfies the Deque abstract interface. */
    void PopBack(){ this->Remove( Vector<T>::rbegin() ); }
    /** Satisfies the Deque abstract interface. */
    void PushFront(const T &i){ this->Insert(i, Vector<T>::begin()); }
    /** Satisfies the Deque abstract interface. */
    void PopFront(){ this->Remove( Vector<T>::begin() ); }
    /** Satisfies the Deque abstract interface. */
    T &Front(){ return (*this)[0]; }
    /** Satisfies the Deque abstract interface. */
    const T &Front() const{ return (*this)[0]; }
    /** Satisfies the Deque abstract interface. */
    T &Back(){ return (*this)[Vector<T>::Length() - 1]; }
    /** Satisfies the Deque abstract interface. */
    const T &Back() const{ return (*this)[Vector<T>::Length() - 1]; }
    /** Satisfies the Deque abstract interface. */
    GUINT32 CountDequeItems() const{ return Vector<T>::Length(); }
    /** Satisfies the Deque abstract interface. */
    void FlushDeque(){ Vector<T>::Clear(); }

    /** Accesses the element at the given index.  This DOES do bounds checking, and will
        throw an IndexOutOfRangeException if the index is invalid.
        Satisfies the RandomAccessContainer abstract interface.
    */
    T &At(GUINT32 i){
        if(i < 0 || i >= Vector<T>::Length()) THROW_NEW_GUTIL_EXCEPTION(IndexOutOfRangeException);
        return (*this)[i];
    }
    /** Accesses the element at the given index.  This DOES do bounds checking, and will
        throw an IndexOutOfRangeException if the index is invalid.
        Satisfies the RandomAccessContainer abstract interface.
    */
    const T &At(GUINT32 i) const{
        if(i < 0 || i >= Vector<T>::Length()) THROW_NEW_GUTIL_EXCEPTION(IndexOutOfRangeException);
        return (*this)[i];
    }
    /** Satisfies the RandomAccessContainer abstract interface. */
    GUINT32 CountContainerItems() const{ return Vector<T>::Length(); }
    /** Satisfies the RandomAccessContainer abstract interface. */
    void FlushContainer(){ Vector<T>::Clear(); }

};


GUTIL_END_CORE_NAMESPACE;

#endif // GUTIL_VECTOR_P
